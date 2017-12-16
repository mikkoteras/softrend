#ifndef WINDOW_H
#define WINDOW_H

#include "SDL.h"
#include "SDL_ttf.h"

class benchmark_frame;
class framebuffer;
class scene;

class window_exception {};

class window {
public:
    window(int w, int h);
    ~window();

    int run(scene &sc);

private:
    void clear_framebuffer(framebuffer &fb, benchmark_frame &frame_stats);
    void update_scene(scene &sc, framebuffer &fb, benchmark_frame &frame_stats);
    void convert_framebuffer(framebuffer &fb, benchmark_frame &frame_stats);
    void render_framebuffer(scene &sc, framebuffer &fb, benchmark_frame &frame_stats);
    bool read_user_input(scene &s);

private:
    bool init_sdl();
    void deinit_sdl();
    void render_text_overlay(scene &sc);

private:
    int width, height;
    static const int concurrent_stages = 4;

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
