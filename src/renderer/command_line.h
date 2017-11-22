#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <string>
#include <vector>

class command_line {
public:
    enum mode { none, auto_scene, named_scene };

public:
    command_line() = delete;
    command_line(int argc, char *argv[]);
    ~command_line();

    bool valid() const;

    bool verbose() const;
    int width() const;
    int height() const;
    mode scene_mode() const;
    const std::string &object_filename() const;
    const std::string &scene_name() const;

private:
    void parse(const std::vector<std::string> &args);
    void print_usage() const;

private:
    class command_line_exception {};
        
    void accept_scene_mode_parameter(const std::vector<std::string> &args, unsigned arg_index, std::string &slot);
    void accept_int_parameter(const std::vector<std::string> &args, unsigned arg_index, int &slot);
    
private:
    bool parsed_correctly;
    bool verbose_opt;
    int width_opt;
    int height_opt;
    mode scene_mode_opt;
    std::string object_filename_opt;
    std::string scene_name_opt;
};

#endif
