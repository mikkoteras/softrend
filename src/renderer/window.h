#ifndef WINDOW_H
#define WINDOW_H

#include "SDL_ttf.h"

class benchmark_frame;
class framebuffer;
class scene;
class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

class window_exception {};

class window {
public:
    window(int w, int h);
    ~window();

    int run(scene &sc);

private:
    void clear_framebuffer(framebuffer &fb, benchmark_frame &frame_stats);
    void update_scene(scene &sc, framebuffer &fb, benchmark_frame &frame_stats);
    void prepare_sdl_texture(scene &sc, framebuffer &fb, benchmark_frame &frame_stats);
    bool read_user_input(scene &s);

private:
    bool init_sdl();
    void deinit_sdl();
    void render_text_overlay(scene &s);

private:
    int width, height;
    static const int concurrent_stages = 3;

private:
    bool sdl_context_initialized = false;
    SDL_Window *sdl_window = nullptr;
    SDL_Renderer *sdl_renderer = nullptr;
    SDL_Texture *sdl_texture = nullptr; // TODO maybe multiple
    
private:
    bool text_overlay_visible = false;
    TTF_Font *text_overlay_font = nullptr;
};

#endif
