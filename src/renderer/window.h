#ifndef WINDOW_H
#define WINDOW_H

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
    
private:
    int width, height;
    bool sdl_context_initialized;
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_texture;  // TODO maybe multiple
};

#endif
