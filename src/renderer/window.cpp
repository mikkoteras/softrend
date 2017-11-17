#include "window.h"
#include "benchmark.h"
#include "framebuffer.h"
#include "scene.h"
#include "SDL.h"

window::window(int w, int h) :
    width(w),
    height(h),
    sdl_context_initialized(false),
    sdl_window(nullptr),
    sdl_renderer(nullptr),
    sdl_texture(nullptr) {

    if (!init_sdl()) {
        deinit_sdl();
        throw window_exception();
    }
}

window::~window() {
    deinit_sdl();
}

int window::run(scene &s) {
    s.start();
    bool quit = false;
    framebuffer fb(width, height);
    int stride = 4 * width;
    benchmark b;
    s.start();

    while (!quit) {
        b.update_starting();
        fb.clear();
        b.clear_finished();
        s.render(fb);
        b.render_finished();
        SDL_RenderClear(sdl_renderer);
        SDL_UpdateTexture(sdl_texture, nullptr, fb.get_rgba_byte_buffer(), stride);
        SDL_RenderCopy(sdl_renderer, sdl_texture, nullptr, nullptr);
        SDL_RenderPresent(sdl_renderer);
        b.copy_finished();
        
        SDL_Event event;
        int mouse_x_tally = 0, mouse_y_tally = 0, mouse_wheel_tally = 0;
        
        
        while (!quit && SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;
            else if (event.type == SDL_KEYDOWN) {
                SDL_KeyboardEvent key = event.key;
                
                if (key.keysym.mod & KMOD_CTRL && key.keysym.sym == SDLK_q)
                    quit = true;
                else if (key.repeat == 0)
                    s.key_down_event(key.keysym.sym, key.keysym.mod & KMOD_CTRL);
            }
            else if (event.type == SDL_MOUSEMOTION) {
                // don't bother calling multiple times per frame
                mouse_x_tally += event.motion.xrel;
                mouse_y_tally += event.motion.yrel;
            }
            else if (event.type == SDL_MOUSEWHEEL) {
                // SDL's wheel ticks point to exactly the heretic direction, dammit
                mouse_wheel_tally -= event.wheel.y;
            }
        }

        if (mouse_x_tally != 0 || mouse_y_tally != 0) {
            uint32_t state = SDL_GetMouseState(nullptr, nullptr);
            s.mouse_move_event(mouse_x_tally, mouse_y_tally, state & SDL_BUTTON(SDL_BUTTON_LEFT));
        }

        if (mouse_wheel_tally != 0)
            s.mouse_wheel_event(mouse_wheel_tally);
    }

    return 0;
}

bool window::init_sdl() {
    if (sdl_context_initialized || sdl_window || sdl_renderer || sdl_texture)
        return false; // already running, idiot

    if (SDL_Init(SDL_INIT_VIDEO))
        return false;

    sdl_context_initialized = true;
    sdl_window = SDL_CreateWindow("softrend", 0, 0, width, height, SDL_WINDOW_SHOWN);

    if (!sdl_window)
        return false;

    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!sdl_renderer)
        return false;

    sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);

    if (!sdl_texture)
        return false;

    return true;
}

void window::deinit_sdl() {
    if (sdl_texture) {
        SDL_DestroyTexture(sdl_texture);
        sdl_texture = nullptr;
    }

    if (sdl_renderer) {
        SDL_DestroyRenderer(sdl_renderer);
        sdl_renderer = nullptr;
    }

    if (sdl_window) {
        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;
    }

    if (sdl_context_initialized) {
        SDL_Quit();
        sdl_context_initialized = false;
    }
}
