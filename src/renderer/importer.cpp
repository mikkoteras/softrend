#include "importer.h"
#include "material_library.h"
#include "util.h"
#include <limits>
#include <iostream>

using namespace std;
using namespace std::experimental::filesystem;
using namespace math;

mesh importer::load_3dsmax_object(const std::experimental::filesystem::path &filename,
                                 material_library &lib, bool echo_comments) {
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
                vector3f point = imp.parse_ws_separated_3d_point();
                m.add_vertex(point);
                ++vertices;
            }
            else if (command == "vn") {
                vector3f point = imp.parse_ws_separated_3d_point();
                m.add_vertex_normal(point);
            }
            else if (command == "vt") {
                vector3f point = imp.parse_ws_separated_uv_coords();
                texture_coordinates.push_back(point);
            }
            else if (command == "mtllib") {
                string filename = imp.accept_until_eol();
                load_3dsmax_materials(filename, lib, echo_comments);
            }
            else if (command == "usemtl") {
                string name = imp.accept_until_eol();
                current_material = lib.get_material(name);
            }
            else if (command == "f") {
                std::vector<int> vertex_indices, normal_indices, texture_indices;
                imp.skip_space();
                bool include_uv_coords = true;
                bool include_normals = true;

                while (!imp.line_ends()) {
                    vertex_indices.push_back(imp.accept_int() - 1); // -1 because indexing starts at 1
                    imp.accept_literal('/');

                    if (imp.next_char_is('/'))
                        include_uv_coords = false;
                    else
                        texture_indices.push_back(imp.accept_int() - 1);

                    imp.accept_literal('/');

                    if (imp.line_ends() || imp.next_char_is(' '))
                        include_normals = false;
                    else
                        normal_indices.push_back(imp.accept_int() - 1);
                }

                if (vertex_indices.size() < 3) {
                    cerr << "importer: too few vertices in poly: " << imp.full_line() << endl;
                    throw importer_exception();
                }

                if (!include_normals) {
                    cerr << "importer: objects without normals not implemented yet" << endl;
                    throw importer_exception();
                }

                if (vertex_indices.size() > 4)
                    cerr << "warning: unexpected number of vertices in poly (" << vertex_indices.size() << ")" << endl;

                if (include_uv_coords)
                    for (unsigned i = 2; i < vertex_indices.size(); ++i) {
                        m.add_triangle(vertex_indices[0],
                                       vertex_indices[i - 1],
                                       vertex_indices[i],
                                       texture_coordinates[texture_indices[0]],
                                       texture_coordinates[texture_indices[i - 1]],
                                       texture_coordinates[texture_indices[i]],
                                       normal_indices[0],
                                       normal_indices[i - 1],
                                       normal_indices[i],
                                       current_material->get_texture_map());
                        ++polys;
                    }
                else
                    for (unsigned i = 2; i < vertex_indices.size(); ++i) {
                        m.add_triangle(vertex_indices[0],
                                       vertex_indices[i - 1],
                                       vertex_indices[i],
                                       normal_indices[0],
                                       normal_indices[i - 1],
                                       normal_indices[i]);
                        ++polys;
                    }
            }

            imp.advance_to_next_line();
        }

        bounding_box box = m.local_bounding_box();
        cout << "imported " << vertices << " vertices, " << polys << " polys" << endl;
        cout << "bounding box = [" << util::to_string(box.min()) << ", " << util::to_string(box.max()) << "]" << endl;
        cout << "maximum semiaxis = " << box.max_semiaxis() << endl;

        return m;
    }
    catch (...) {
        cerr << "error loading " << filename << endl;
        throw;
    }
}

void importer::load_3dsmax_materials(const std::string &filename, material_library &lib, bool echo_comments = false) {
    try {
        importer imp(filename);
        
        bool material_constructed = false;
        material constructed_material;
        string constructed_material_name;
        
        while (!imp.eof()) {
            string command = imp.accept_command();
            
            if (command == "#") {
                if (echo_comments)
                    cout << imp.full_line() << endl;
            }
            else if (command == "newmtl") {
                if (material_constructed)
                    lib.add_material(constructed_material_name, constructed_material);

                material_constructed = true;
                constructed_material = material();
                constructed_material_name = imp.accept_until_eol();
            }
            else if (command == "Ka")
                constructed_material.set_ambient_reflectivity(imp.parse_material_vector());
            else if (command == "Kd")
                constructed_material.set_diffuse_reflectivity(imp.parse_material_vector());
            else if (command == "Ks")
                constructed_material.set_specular_reflectivity(imp.parse_material_vector());
            else if (command == "Tf")
                constructed_material.set_transmission_filter(imp.parse_material_vector());
            else if (command == "illum")
                constructed_material.set_illumination_model(imp.accept_int());
            else if (command == "d") {
                bool halo = false;

                if (imp.next_char_is('-')) {
                    string token = imp.accept_command();

                    if (token != "-halo") {
                        cerr << "Unknown argument to d: " << token << endl;
                        throw importer_exception();
                    }

                    halo = true;
                }

                constructed_material.set_dissolve(imp.accept_float(), halo);
            }
            else if (command == "Ns")
                constructed_material.set_specular_exponent(imp.accept_float());
            else if (command == "sharpness")
                constructed_material.set_sharpness(imp.accept_float());
            else if (command == "Ni")
                constructed_material.set_optical_density(imp.accept_float());
            else if (command == "map_Ka") {
                cerr << "map_Ka is not yet supported." << endl;
                throw importer_exception();
            }
            else if (command == "map_Kd") {
                string png_filename = imp.accept_until_eol();
                lib.add_texture(png_filename, png_filename);
                constructed_material.set_texture_map(lib.get_texture(png_filename));
            }
            else if (command == "map_Ks" || command == "map_Ns" || command == "map_d" ||
                     command == "disp" || command == "decal" || command == "bump" || command == "refl") {
                cerr << command << " is not yet supported." << endl;
                throw importer_exception();
            }
            else {
                cerr << "Unknown command " << command << "." << endl;
                throw importer_exception();
            }
            
            imp.advance_to_next_line();
        }

        if (material_constructed)
            lib.add_material(constructed_material_name, constructed_material);
    }
    catch (...) {
        cerr << "error loading " << filename << endl;
    }
}

importer::importer(const path &source) :
    input(source.c_str()),
    at_eof(false) {

    original_working_directory = current_path();
    path p = path(source).parent_path();

    if (!p.empty())
        current_path(p);

    advance_to_next_line();
}

importer::~importer() {
    current_path(original_working_directory);
}

vector3f importer::parse_ws_separated_3d_point() {
    vector3f point;

    for (int i = 0; i < 3; ++i)
        point[i] = accept_float();

    return point;
}

vector3f importer::parse_ws_separated_uv_coords() {
    vector3f point{0.0f, 0.0f, 0.0f};
    point[0] = accept_float();
    point[1] = accept_float();

    if (!line_ends())
        point[2] = accept_float();

    return point;
}

vector3f importer::parse_material_vector() {
    char next = peek_char();
    
    if (next == 's') {
        cerr << "Spectral curves are not supported." << endl;
        throw importer_exception();
    }
    else if (next == 'x') {
        cerr << "CIEXYZ values are not supported." << endl;
        throw importer_exception();
    }
    else {
        vector3f result;

        for (int i = 0; i < 3; ++i)
            result[i] = accept_float();
        
        return result;
    }
}

void importer::advance_to_next_line() {
    if (input.bad()) {
        cerr << "importer::advance_to_next_line: failed." << endl;
        throw importer_exception();
    }

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

char importer::peek_char() {
    if (line_parse.eof()) {
        cerr << "importer: expected character, got EOF." << endl;
        throw importer_exception();
    }

    return static_cast<decltype(input)::int_type>(line_parse.peek());
}

bool importer::next_char_is(char c) {
    return peek_char() == c;
}

int importer::accept_int() {
    if (line_parse.eof()) {
        cerr << "importer: expected integer, got EOF." << endl;
        throw importer_exception();
    }

    int result;
    line_parse >> result;
    return result;
}

float importer::accept_float() {
    if (line_parse.eof()) {
        cerr << "importer: expected floating point number, got EOF." << endl;
        throw importer_exception();
    }

    float result;
    line_parse >> result;
    return result;
}

char importer::accept_char() {
    if (line_parse.eof()) {
        cerr << "importer: expected character, got EOF." << endl;
        throw importer_exception();
    }

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
    if (accept_char() != c) {
        cerr << "importer: unexpected character." << endl;
        throw importer_exception();
    }
}

string importer::accept_command() {
    while (!line_parse.eof() && isspace(line_parse.peek()))
        line_parse.ignore();

    if (line_parse.eof()) {
        cerr << "importer::accept_command(): out of characters: " << current_line << endl;
        throw importer_exception();
    }

    string result;
    char c = line_parse.get();
    result += c;

    if (c != '#' && !isalpha(c) && c != '_' && c != '-') {
        cerr << "importer::accept_command(): unexpected character: " << current_line << endl;
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
