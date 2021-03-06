#ifndef IMPORTER_H
#define IMPORTER_H

#include "mesh.h"
#include "color3.h"
#include "math/vector.h"
#include <fstream>
#include <sstream>
#include <string>
#include <experimental/filesystem>

class material_library;

class importer {
public:
    static void load_wavefront_object(mesh &target,
                                      const std::experimental::filesystem::path &filename,
                                      material_library &lib,
                                      bool verbose = false);

public:
    class importer_exception {};

private:
    static void load_wavefront_materials(const std::string &filename, material_library &lib, bool verbose = false);

private: // [sic]
    importer(const std::experimental::filesystem::path &source, bool verbose);
    ~importer();

    importer() = delete;
    importer(const importer&) = delete;
    importer(importer&&) = delete;
    const importer &operator=(const importer&) = delete;
    importer &operator=(importer&&) = delete;

private:
    math::vector3f parse_ws_separated_3d_point();
    math::vector2f parse_ws_separated_uv_coords();
    color3 parse_material_vector(); // color or illumination
    void ensure_known_vector_format();

private:
    void advance_to_next_line();
    bool eof();
    bool line_ends();
    void skip_space();
    std::string full_line();
    char peek_char();
    bool next_char_is(char c);
    int accept_int();
    float accept_float();
    char accept_char();
    std::string accept_until_eol();
    void accept_literal(char c);
    std::string accept_command();
    int get_line_number() const;

private:
    static void chomp(std::string &s);
    std::string error_prefix() const;

private:
    static bool uvw_warning_given;

private:
    std::experimental::filesystem::path original_working_directory;

    std::string filename;
    std::ifstream input;
    std::string current_line;
    std::istringstream line_parse;
    bool at_eof;
    bool verbose;
    int line_number;
};

#endif
