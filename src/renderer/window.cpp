#include "window.h"
#include "benchmark.h"
#include "framebuffer.h"
#include "scene.h"
#include "SDL.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

window::window(int w, int h) :
    width(w),
    height(h),
    sdl_context_initialized(false),
    sdl_window(nullptr),
    sdl_renderer(nullptr),
    sdl_texture(nullptr),
    text_overlay_visible(true),
    text_overlay_font(nullptr) {

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
    s.start();

    benchmark::timestamp_t ts, fts;
    benchmark &mark = s.get_benchmark();

    while (!quit && !s.stopped()) {
        fts = mark.frame_starting();
        ts = mark.clear_starting();
        fb.clear();
        mark.clear_finished(ts);
        ts = mark.render_starting();
        s.prerender(fb);
        s.render(fb);
        s.postrender();
        mark.render_finished(ts);
        ts = mark.copy_starting();
        SDL_RenderClear(sdl_renderer);
        SDL_UpdateTexture(sdl_texture, nullptr, fb.get_rgba_byte_buffer(), stride);
        SDL_RenderCopy(sdl_renderer, sdl_texture, nullptr, nullptr);
        render_text_overlay(s);
        SDL_RenderPresent(sdl_renderer);
        mark.copy_finished(ts);
        mark.frame_finished(fts);

        SDL_Event event;
        int mouse_x_tally = 0, mouse_y_tally = 0;
        int mouse_horz_wheel_tally = 0, mouse_vert_wheel_tally = 0;

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
                mouse_horz_wheel_tally += event.wheel.x;
                // SDL's wheel ticks point to exactly the heretic direction, dammit
                mouse_vert_wheel_tally -= event.wheel.y;
            }
        }

        if (mouse_x_tally != 0 || mouse_y_tally != 0) {
            uint32_t state = SDL_GetMouseState(nullptr, nullptr);
            s.mouse_move_event(mouse_x_tally, mouse_y_tally, state & SDL_BUTTON(SDL_BUTTON_LEFT));
        }

        if (mouse_horz_wheel_tally != 0 || mouse_vert_wheel_tally != 0)
            s.mouse_wheel_event(mouse_horz_wheel_tally, mouse_vert_wheel_tally);
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

    if (TTF_Init() < 0) {
        std::cerr << "can't init SDL TTF rendering" << std::endl;
        return false;
    }
    
    std::string font_path("/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-R.ttf");
    text_overlay_font = TTF_OpenFont(font_path.c_str(), 16);

    if (!text_overlay_font) {
        std::cerr << "can't load font file " << font_path << std::endl;
        return false;
    }
    
    return true;
}

void window::deinit_sdl() {
    if (text_overlay_font) {
        TTF_CloseFont(text_overlay_font);
        text_overlay_font = nullptr;
    }

    if (TTF_WasInit())
        TTF_Quit();

    
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

void window::render_text_overlay(scene &s) {
    if (!text_overlay_visible)
        return;
    
    SDL_Color text_color = {255, 255, 255};
    vector<string> info(s.get_scene_info().get());

    for (unsigned i = 0; i < info.size(); ++i) {
        int w = 1, h = 1;
        TTF_SizeText(text_overlay_font, info[i].c_str(), &w, &h);
        
        SDL_Surface* message_surface = TTF_RenderText_Solid(text_overlay_font, info[i].c_str(), text_color);
        SDL_Texture* message_texture = SDL_CreateTextureFromSurface(sdl_renderer, message_surface);

        SDL_Rect rect;
        rect.x = 10;
        rect.y = 10 + i * (h + 4);
        rect.w = w;
        rect.h = h;

        SDL_RenderCopy(sdl_renderer, message_texture, nullptr, &rect);
        SDL_DestroyTexture(message_texture);
        SDL_FreeSurface(message_surface);
    }
}
