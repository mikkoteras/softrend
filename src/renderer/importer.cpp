#include "importer.h"
#include "material_library.h"
#include "util.h"
#include <limits>
#include <iostream>

using namespace std;
using namespace math;

mesh importer::load_3dmax_object(const std::string &filename, material_library &lib, bool echo_comments) {
    try {
        mesh m;
        importer imp(filename);

        const float infty = numeric_limits<float>::infinity();
        vector4f box_min{infty, infty, infty, 1};
        vector4f box_max{-infty, -infty, -infty, 1};
        int vertices = 0, polys = 0;
        const material *current_material = nullptr;
        std::vector<vector3f> texture_coordinates;
        
        while (!imp.eof()) {
            string command = imp.accept_command();

            if (command == "#") {
                if (echo_comments)
                    cout << imp.full_line() << endl;
            }
            else if (command == "v") {
                vector4f point = imp.parse_ws_separated_3d_point();
                m.add_vertex(point, color(1, 1, 1, 1));

                box_min = util::min(box_min, point);
                box_max = util::max(box_max, point);
                ++vertices;
            }
            else if (command == "vn") {
                vector4f point = imp.parse_ws_separated_3d_point();
                m.add_vertex_normal(point);
            }
            else if (command == "vt") {
                vector3f point = imp.parse_ws_separated_uv_coords();
                texture_coordinates.push_back(point);
            }
            else if (command == "mtllib") {
                string filename = imp.accept_until_eol();
                load_3dmax_materials(filename, lib, echo_comments);
            }
            else if (command == "usemtl") {
                string name = imp.accept_until_eol();
                current_material = lib.get_material(name);
            }
            else if (command == "f") {
                std::vector<int> vertex_indices, normal_indices, texture_indices;
                imp.skip_space();
                
                while (!imp.line_ends()) {
                    vertex_indices.push_back(imp.accept_int() - 1); // -1 because index starts at 1
                    imp.accept_literal('/');

                    if (imp.next_char_is('/'))
                        texture_indices.push_back(-1);
                    else
                        texture_indices.push_back(imp.accept_int() - 1);
                        
                    imp.accept_literal('/');

                    if (imp.line_ends() || imp.next_char_is(' '))
                        normal_indices.push_back(-1);
                    else
                        normal_indices.push_back(imp.accept_int() - 1);
                }

                if (vertex_indices.size() < 3) {
                    cout << "importer: too few vertices in poly: " << imp.full_line() << endl;
                    throw importer_exception();
                }
            
                if (vertex_indices.size() > 4)
                    cout << "warning: unexpected number of vertices in poly (" << vertex_indices.size() << ")" << endl;

                for (unsigned i = 2; i < vertex_indices.size(); ++i) {
                    m.add_triangle(vertex_indices[0],
                                   vertex_indices[i - 1],
                                   vertex_indices[i],
                                   texture_coordinates[texture_indices[0]], // FIXME can be bad index
                                   texture_coordinates[texture_indices[i - 1]], // FIXME can be bad index
                                   texture_coordinates[texture_indices[i]], // FIXME can be bad index
                                   normal_indices[0], // FIXME can be bad index
                                   normal_indices[i - 1], // FIXME can be bad index
                                   normal_indices[i], // FIXME can be bad index
                                   current_material->get_texture());                    
                    ++polys;
                }
            }

            imp.advance_to_next_line();
        }

        cout << "imported " << vertices << " vertices, " << polys << " polys" << endl;
        cout << "bounding box = [" << util::to_string(box_min) << ", " << util::to_string(box_max) << "]" << endl;
        return m;
    }
    catch (...) {
        cout << "error loading " << filename << endl;
        return mesh();
    }
}

void importer::load_3dmax_materials(const std::string &filename, material_library &lib, bool echo_comments = false) {
    try {
        importer imp(filename);
        string current_material_name;
        
        while (!imp.eof()) {
            string command = imp.accept_command();

            if (command == "#") {
                if (echo_comments)
                    cout << imp.full_line() << endl;
            }
            else if (command == "newmtl")
                current_material_name = imp.accept_until_eol();
            else if (command == "map_Kd") {
                string texture_filename = imp.accept_until_eol();
                
                if (!current_material_name.empty()) {
                    lib.add_texture(texture_filename, texture_filename);
                    lib.add_material(current_material_name, texture_filename);
                }
                
                current_material_name.clear();
            }

            imp.advance_to_next_line();
        }
    }
    catch (...) {
        cout << "error loading " << filename << endl;
    }
} 

importer::importer(const std::string &filename) :
    input(filename.c_str()),
    at_eof(false) {

    advance_to_next_line();
}

importer::~importer() {
}

vector4f importer::parse_ws_separated_3d_point() {
    vector4f point{0, 0, 0, 1};
                
    for (int i = 0; i < 3; ++i)
        point[i] = accept_float();

    return point;
}

vector3f importer::parse_ws_separated_uv_coords() {
    vector3f point{0, 0, 0};
    point[0] = accept_float();
    point[1] = accept_float();

    if (!line_ends())
        point[2] = accept_float();
    
    return point;
}


void importer::advance_to_next_line() {
    // TODO: this will hang forever if the file is bad
    
    getline(input, current_line);
    chomp(current_line);
    
    while (current_line.empty() && !input.eof()) {
        getline(input, current_line);
        chomp(current_line);
    }

    if (input.eof()) {
        at_eof = true;
        current_line = "";
        line_parse = istringstream("");
    }
    else
        line_parse = istringstream(current_line);
}

bool importer::eof() {
    return at_eof;
}

bool importer::line_ends() {
    skip_space();
    return line_parse.eof();
}

void importer::skip_space() {
    while (!line_parse.eof() && isspace(line_parse.peek()))
        line_parse.ignore();
}

string importer::full_line() {
    return current_line;
}

bool importer::next_char_is(char c) {
    if (line_parse.eof())
        throw importer_exception();
    
    return line_parse.peek() == (decltype(input)::int_type)c;
}

int importer::accept_int() {
    if (line_parse.eof())
        throw importer_exception();
    
    int result;
    line_parse >> result;
    return result;
}

float importer::accept_float() {
    if (line_parse.eof())
        throw importer_exception();
    
    float result;
    line_parse >> result;
    return result;
}

char importer::accept_char() {
    if (line_parse.eof())
        throw importer_exception();

    decltype(line_parse)::int_type c = line_parse.get();
    return (char)c;
}

string importer::accept_until_eol() {
    while (!line_parse.eof() && isspace(line_parse.peek()))
        line_parse.ignore();

    std::string result;
    line_parse >> result;
    return result;
}

void importer::accept_literal(char c) {
    if (accept_char() != c)
        throw importer_exception();
}

string importer::accept_command() {
    while (!line_parse.eof() && isspace(line_parse.peek()))
        line_parse.ignore();
    
    if (line_parse.eof()) {
        cout << "importer::accept_command(): out of characters: " << current_line << endl;
        throw importer_exception();
    }
    
    string result;
    char c = line_parse.get();
    result += c;

    if (c != '#' && !isalpha(c) && c != '_') {
        cout << "importer::accept_command(): unexpected character: " << current_line << endl;
        throw importer_exception();
    }

    char next = line_parse.peek();
    
    while (!line_parse.eof() && (isalpha(next) || next == '_')) {
        result += line_parse.get();
        next = line_parse.peek();
    }
    
    return result;
}

void importer::chomp(string &s) {
    int i = s.size() - 1;

    while (i >= 0 && (s[i] == '\r' || s[i] == '\n'))
        --i;

    s.resize(i + 1);
}
