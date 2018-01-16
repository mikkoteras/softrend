#include "command_line.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;

command_line::command_line(int argc, char *argv[]) :
    parsed_correctly(false),
    verbose_opt(false),
    width_opt(-1),
    height_opt(-1),
    render_threads_opt(-1),
    scene_mode_opt(none) {

    std::vector<string> args;

    for (int i = 0; i < argc; ++i)
        args.push_back(argv[i]);

    parse(args);
}

command_line::~command_line() {
}

bool command_line::valid() const {
    return parsed_correctly;
}

bool command_line::verbose() const {
    return verbose_opt;
}

int command_line::width() const {
    return width_opt;
}

int command_line::height() const {
    return height_opt;
}

int command_line::render_threads() const {
    return render_threads_opt;
}

command_line::mode command_line::scene_mode() const {
    return scene_mode_opt;
}

const std::string &command_line::object_filename() const {
    return object_filename_opt;
}

const std::string &command_line::scene_name() const {
    return scene_name_opt;
}

void command_line::parse(const std::vector<std::string> &args) {
    unsigned i = 1;
    bool error = false;
    string executable = args[0];

    try {
        while (i < args.size()) {
            const string &arg = args[i];

            if (arg == "-o" || arg == "--object") {
                accept_scene_mode_parameter(args, i++, object_filename_opt);
                scene_mode_opt = auto_scene;
            }
            else if (arg == "-s" || arg == "--scene") {
                accept_scene_mode_parameter(args, i++, scene_name_opt);
                scene_mode_opt = named_scene;
            }
            else if (arg == "-w" || arg == "--width")
                accept_int_parameter(args, i++, width_opt);
            else if (arg == "-h" || arg == "--height")
                accept_int_parameter(args, i++, height_opt);
            else if (arg == "-r")
                accept_int_parameter(args, i++, render_threads_opt);
            else if (arg == "-v" || arg == "--verbose")
                verbose_opt = true;

            i += 1;
        }

        // sanity and defaults
        if (scene_mode_opt == none) {
            cerr << "Scene not specified. Please include either -o <object> or -s <scene>." << std::endl;
            throw command_line_exception();
        }
        
        if (render_threads_opt < 0) {
            render_threads_opt = max(std::thread::hardware_concurrency(), 1u);

            if (verbose())
                cout << "Defaulting to " << render_threads_opt << " render thread(s). " << endl;
        }

        if (width_opt < 0 && height_opt < 0) { // size not specified, use default
            width_opt = 800;
            height_opt = 600;

            if (verbose())
                cout << "Defaulting to window size " << width_opt << " x " << height_opt << "." << endl;
        }
        else if (width_opt < 0 && height_opt >= 0) {
            cerr << "Height option specified but width missing." << std::endl;
            throw command_line_exception();
        }
        else if (width_opt >= 0 && height_opt < 0) {
            cerr << "Width option specified but height missing." << std::endl;
            throw command_line_exception();
        }
        else {
            const int minimum_window_dimension = 100;

            if (width_opt < minimum_window_dimension || height_opt < minimum_window_dimension) {
                cerr << "Window size must be at least " << minimum_window_dimension << " x "
                    << minimum_window_dimension << " pixels." << endl;
                throw command_line_exception();
            }
        }

        if (render_threads_opt == 0) {
            cerr << "Must use at least one render thread." << endl;
            throw command_line_exception();
        }
        
        parsed_correctly = true;
    }
    catch (command_line_exception) {
        error = true;
    }

    if (error)
        print_usage();
}

void command_line::print_usage() const {
    cerr << "usage: softrend (-o|--object <object-file>)|(-s|--scene <scene-name>)" << endl
         << "                [-w|--width <width> -h|--height <height>] [-r <threads>] [-v|--verbose]" << endl;
}

void command_line::accept_scene_mode_parameter(const vector<string> &args, unsigned arg_index, string &slot) {
    string arg = args[arg_index];
    
    if (arg_index + 1 >=  args.size()) {
        cerr << "Argument " << arg << " must be followed by a string." << endl;
        throw command_line_exception();
    }
    else if (scene_mode_opt != none) {
        cerr << "Only one scene may be specified at once." << endl;
        throw command_line_exception();
    }

    string parameter = args[arg_index + 1];
    slot = parameter;
}

void command_line::accept_int_parameter(const vector<string> &args, unsigned arg_index, int &slot) {
    string arg = args[arg_index];
    
    if (arg_index + 1 >=  args.size()) {
        cerr << "Argument " << arg << " must be followed by an integer." << endl;
        throw command_line_exception();
    }
    else if (slot >= 0) {
        cerr << "Argument " << arg << " given multiple times." << endl;
        throw command_line_exception();
    }

    string parameter = args[arg_index + 1];

    if (parameter.find_first_not_of("0123456789") != string::npos) {
        cerr << "Improperly formatted integer \"" << parameter << "\"." << endl;
        throw command_line_exception();
    }

    istringstream number(parameter);
    int result;
    number >> result;
    
    slot = result;
}
