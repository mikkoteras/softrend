#ifndef WINDOW_H
#define WINDOW_H

#include "thread_pool.h"
#include "SDL.h"
#include "SDL_ttf.h"

class benchmark_frame;
class command_line;
class framebuffer;
class pipeline_context;
class scene;

class window_exception {};

class window {
public:
    window(const command_line &cl, scene &sc);
    ~window();

    int run();
    void run_threaded(unsigned thread_index);

private:
    void clear_framebuffer(framebuffer &fb, benchmark_frame &frame_stats);
    void update_scene(scene &sc, framebuffer &fb, benchmark_frame &frame_stats);
    void convert_framebuffer(framebuffer &fb, benchmark_frame &frame_stats);
    void render_framebuffer(scene &sc, framebuffer &fb, benchmark_frame &frame_stats);
    bool read_user_input(scene &s);
    void rotate_pipeline_contexts();

private:
    bool init_sdl();
    void deinit_sdl();
    void render_text_overlay(scene &sc);

private:
    int width, height;
    scene *displayed_scene;
    static const int concurrent_stages = 4;
    thread_pool<window> threads;
    std::vector<pipeline_context*> contexts;

private:
    bool sdl_context_initialized = false;
    SDL_Window *sdl_window = nullptr;
    SDL_Renderer *sdl_renderer = nullptr;
    SDL_Texture *sdl_texture = nullptr;

private:
    bool text_overlay_visible = false;
    TTF_Font *text_overlay_font = nullptr;
};

#endif
