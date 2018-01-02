#include <SDL.h>
#include <SDL_opengl.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

struct global_events {
    bool focused;
    bool fullscreen;
    bool quit;
};

struct global_game_state {
    struct global_events ev;
    unsigned short display_count;
    unsigned short keyboard_count;
    unsigned short mouse_count;
    unsigned short controller_count;
    SDL_EventFilter watch;
    SDL_GLContext gl_context;
    SDL_Window *window;
};

void render_frame(struct global_game_state *state)
{
    //SDL_Log("Ouput[rgb]: %03u:%03u:%03u", i, j, k);
    SDL_GL_MakeCurrent(state->window, state->gl_context);

    sfvec4 magenta = {.r = 0xee, .g = 0x00, .b = 0xff };
    glClearColor( magenta.r, magenta.g, magenta.b, magenta.a );
    glClear( GL_COLOR_BUFFER_BIT );

    SDL_GL_SwapWindow(state->window);
}

int SDLCALL sdl_event_dispatch(void *userdata, SDL_Event* event)
{
    struct global_events *ev = userdata;

    if (event->type == SDL_APP_WILLENTERBACKGROUND || event->type == SDL_APP_DIDENTERBACKGROUND) {
        SDL_WindowEvent window = event->window;
        SDL_Log("Window %d focus lost", window.windowID);
        ev->focused = false;
    }
    if (event->type == SDL_APP_WILLENTERFOREGROUND || event->type == SDL_APP_DIDENTERFOREGROUND ) {
        SDL_WindowEvent window = event->window;
        SDL_Log("Window %d focus received", window.windowID);
        ev->focused = true;
    }
    if(event->type == SDL_QUIT) {
        SDL_Log("Recevied quit event.");
        ev->quit = true;
    }
    if(event->type == SDL_MOUSEMOTION) {
        SDL_MouseMotionEvent motion = event->motion;
        SDL_Log("Mouse moved from %d %d to %d %d", motion.x, motion.y, motion.x + motion.xrel, motion.y + motion.yrel);
    }
    if(event->type == SDL_MOUSEBUTTONUP) {
        SDL_MouseButtonEvent button = event->button;
        char *which_button;
        switch (button.button) {
            case SDL_BUTTON_LEFT:
                which_button = "left";
                break;
            case SDL_BUTTON_MIDDLE:
                which_button = "middle";
                break;
            case SDL_BUTTON_RIGHT:
                which_button = "right";
                break;
            case SDL_BUTTON_X1:
                which_button = "x1";
                break;
            case SDL_BUTTON_X2:
                which_button = "x2";
                break;
            default:
                break;
        }
        SDL_Log("Mouse button %s released", which_button);
    }
    if(event->type == SDL_MOUSEBUTTONDOWN) {
        SDL_MouseButtonEvent button = event->button;
        char *which_button;
        switch (button.button) {
            case SDL_BUTTON_LEFT:
                which_button = "left";
                break;
            case SDL_BUTTON_MIDDLE:
                which_button = "middle";
                break;
            case SDL_BUTTON_RIGHT:
                which_button = "right";
                break;
            case SDL_BUTTON_X1:
                which_button = "x1";
                break;
            case SDL_BUTTON_X2:
                which_button = "x2";
                break;
            default:
                break;
        }
        SDL_Log("Mouse button %s clicked", which_button);
    }
    if(event->type == SDL_MOUSEWHEEL) {
        SDL_MouseWheelEvent wheel = event->wheel;
        if(wheel.y == 1) {
            SDL_Log("Mouse wheel up");
        } else if(wheel.y == -1) {
            SDL_Log("Mouse wheel down");
        }
    }
    if(event->type == SDL_KEYUP) {
        SDL_KeyboardEvent key = event->key;
        SDL_Log("Key %s[%s] released", SDL_GetScancodeName(key.keysym.scancode), SDL_GetKeyName(event->key.keysym.sym));
        if (key.keysym.sym == SDLK_q) {
            ev->quit = true;
        }
        if (key.keysym.sym == SDLK_f) {
            ev->fullscreen = true;
            SDL_Log("Fullscreen");
        }
    }
    if(event->type == SDL_KEYDOWN) {
        SDL_KeyboardEvent key = event->key;
        SDL_Log("Key %s[%s] pressed", SDL_GetScancodeName(key.keysym.scancode), SDL_GetKeyName(event->key.keysym.sym));
    }
    if(event->type == SDL_WINDOWEVENT) {
        SDL_WindowEvent window = event->window;
        switch (window.event) {
            case SDL_WINDOWEVENT_SHOWN:
                SDL_Log("Window %d shown", window.windowID);
                break;
            case SDL_WINDOWEVENT_HIDDEN:
                SDL_Log("Window %d hidden", window.windowID);
                break;
            case SDL_WINDOWEVENT_EXPOSED:
                SDL_Log("Window %d exposed", window.windowID);
                break;
            case SDL_WINDOWEVENT_MOVED:
                //SDL_Log("Window %d moved to %d,%d", window.windowID, window.data1, window.data2);
                break;
            case SDL_WINDOWEVENT_RESIZED:
                SDL_Log("Window %d resized to %dx%d", window.windowID, window.data1, window.data2);
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                SDL_Log("Window %d size changed to %dx%d", window.windowID, window.data1, window.data2);
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                SDL_Log("Window %d minimized", window.windowID);
                ev->focused = false;
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
                SDL_Log("Window %d maximized", window.windowID);
                break;
            case SDL_WINDOWEVENT_RESTORED:
                SDL_Log("Window %d restored", window.windowID);
                break;
            case SDL_WINDOWEVENT_ENTER:
                SDL_Log("Mouse entered window %d", window.windowID);
                ev->focused = true;
                break;
            case SDL_WINDOWEVENT_LEAVE:
                SDL_Log("Mouse left window %d", window.windowID);
                ev->focused = false;
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                SDL_Log("Window %d gained keyboard focus", window.windowID);
                ev->focused = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                SDL_Log("Window %d lost keyboard focus", window.windowID);
                ev->focused = false;
                break;
            case SDL_WINDOWEVENT_CLOSE:
                SDL_Log("Window %d closed", window.windowID);
                break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
            case SDL_WINDOWEVENT_TAKE_FOCUS:
                SDL_Log("Window %d is offered a focus", window.windowID);
                ev->focused = true;
                break;
            case SDL_WINDOWEVENT_HIT_TEST:
                SDL_Log("Window %d has a special hit test", window.windowID);
                break;
#endif
            default:
                SDL_Log("Window %d got unknown event %d",
                    window.windowID, window.event);
                break;
        }
    }
    if(event->type == SDL_SYSWMEVENT) {
        SDL_Log("System event");
    }


    return 1;
}

int sdl_init(struct global_game_state *state)
{
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    int i, display_mode_count;
    SDL_DisplayMode mode;
    Uint32 f;


    Uint32 subsystem_init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
    if (subsystem_init != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    state->display_count = SDL_GetNumVideoDisplays();
    if (state->display_count < 1) {
        SDL_Log("Unable to detect displays: %s", SDL_GetError());
        return 1;
    }
    SDL_Log("Found %u display%s", state->display_count, state->display_count != 1 ? "s" : "");
    SDL_Log("Found %u %s", state->mouse_count, state->mouse_count != 1 ? "mice" : "mouse");
    SDL_Log("Found %u keyboard%s", state->keyboard_count, state->keyboard_count != 1 ? "s" : "");
    SDL_Log("Found %u controller%s", state->controller_count, state->controller_count != 1 ? "s" : "");

    state->window = SDL_CreateWindow("title",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        512,
        512,
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS /* | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_INPUT_GRABBED */
    );
    if (state->window == NULL) {
        // In the case that the window could not be made...
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create window: %s", SDL_GetError());
        return 1;
    } else {
        int ww, wh, glw, glh;
        SDL_GetWindowSize(state->window, &ww, &wh);
        SDL_Log("Window resolution: %dx%d", ww, wh);
        SDL_GL_GetDrawableSize(state->window, &glw, &glh);
        SDL_Log("Draw resolution: %dx%d", glw, glh);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(state->window, -1, 0);
    //SDL_Renderer *renderer = SDL_GetRenderer(state->window);
    if (NULL != renderer) {
        int rw, rh;
        if (SDL_GetRendererOutputSize(renderer, &rw, &rh) == 0) {
            SDL_Log("Render resolution: %dx%d", rw, rh);
        } else {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not get render resolution: %s", SDL_GetError());
        }
    }
    state->gl_context = SDL_GL_CreateContext(state->window);

    SDL_AddEventWatch(sdl_event_dispatch, &(state->ev));
    state->ev.focused = true;

    return 0;
}

void sdl_cleanup(struct global_game_state *state)
{
    SDL_DelEventWatch(sdl_event_dispatch, &(state->ev));
    SDL_GL_DeleteContext(state->gl_context);
    SDL_DestroyWindow(state->window);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    struct global_game_state game = {NULL};

    if (sdl_init(&game) != 0) { return 1; }

    bool out_msg = true;
    while(!game.ev.quit) {
        SDL_Event event;
        SDL_PollEvent(&event);
        if (game.ev.fullscreen) {
            SDL_SetWindowFullscreen(game.window, SDL_WINDOW_FULLSCREEN);
        } else {
            SDL_SetWindowFullscreen(game.window, 0);
        }
        if (game.ev.focused) {
            if (out_msg) {
                SDL_Log("Rendering");
                out_msg = false;
            }
            render_frame(&game);
        } else {
            if (out_msg) {
                SDL_Log("Skipping rendering");
                out_msg = false;
            }
            SDL_Delay(200);
        }
    }

    SDL_Log("Closing.");

    sdl_cleanup(&game);
    return EXIT_SUCCESS;
}
