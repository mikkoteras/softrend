#ifndef WINDOW_H
#define WINDOW_H

#include "SDL_ttf.h"

class scene;
class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

class window_exception {};

class window {
public:
    window(int w, int h);
    ~window();

    int run(scene &s);

private:
    bool init_sdl();
    void deinit_sdl();
    void render_text_overlay(scene &s);

private:
    int width, height;
    bool sdl_context_initialized;
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_texture;  // TODO maybe multiple

private:
    bool text_overlay_visible;
    TTF_Font *text_overlay_font;
};

#endif
