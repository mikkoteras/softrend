#ifndef IMPORTER_H
#define IMPORTER_H

#include "mesh.h"
#include "vector.h"
#include <fstream>
#include <sstream>
#include <string>
#include <experimental/filesystem>

class material_library;

class importer {
public:
    static mesh load_3dmax_object(const std::string &filename, material_library &lib, bool echo_comments = false);
    
private:
    static void load_3dmax_materials(const std::string &filename, material_library &lib, bool echo_comments);
    
private: // [sic]
    importer(const std::string &source);
    ~importer();

    importer() = delete;
    importer(const importer&) = delete;
    importer(importer&&) = delete;
    const importer &operator=(const importer&) = delete;
    importer &operator=(importer&&) = delete;

private:
    class importer_exception {};

private:
    math::vector4f parse_ws_separated_3d_point();
    math::vector3f parse_ws_separated_uv_coords(); // uv or uvw
    
private:
    void advance_to_next_line();
    bool eof();
    bool line_ends();
    void skip_space();
    std::string full_line();
    bool next_char_is(char c);
    int accept_int();
    float accept_float();
    char accept_char();
    std::string accept_until_eol();
    void accept_literal(char c);
    std::string accept_command();

private:
    static void chomp(std::string &s);
    
private:
    std::experimental::filesystem::path original_working_directory;
    
    std::ifstream input;
    std::string current_line;
    std::istringstream line_parse;
    bool at_eof;
};

#endif
