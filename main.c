#include <SDL.h>
#include <SDL_opengl.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

struct global_events {
    bool focused;
    bool quit;
};

struct global_game_state {
    struct global_events ev;
    SDL_EventFilter watch;
    SDL_GLContext gl_context;
    SDL_Window *window;
};

void render_frame(struct global_game_state *state, unsigned short i, unsigned short j, unsigned short k)
{
    //SDL_Log("Ouput[rgb]: %03u:%03u:%03u", i, j, k);
    SDL_GL_MakeCurrent(state->window, state->gl_context);

    glClearColor( (float)i/(float)0xFF, (float)j/(float)0xff, (float)k/(float)0xff, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    SDL_GL_SwapWindow(state->window);
}

int SDLCALL sdl_event_dispatch(void *userdata, SDL_Event* event)
{
    struct global_events *ev = userdata;
    //SDL_Log("Recevied event %u.", event->type);

    if (event->type == SDL_APP_WILLENTERBACKGROUND || event->type == SDL_APP_DIDENTERBACKGROUND) {
        SDL_Log("Window focus lost");
        ev->focused = false;
    }
    if (event->type == SDL_APP_WILLENTERFOREGROUND || event->type == SDL_APP_DIDENTERFOREGROUND ) {
        SDL_Log("Window focus received");
        ev->focused = true;
    }
    if(event->type == SDL_QUIT) {
        SDL_Log("Recevied quit event.");
        ev->quit = true;
    }

    return 1;
}

int sdl_init(struct global_game_state *state)
{
    Uint32 subsystem_init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
    if (subsystem_init != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    state->window = SDL_CreateWindow("title",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        512,
        512,
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS /* | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_RESIZABLE*/
    );
    if (state->window == NULL) {
        // In the case that the window could not be made...
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create window: %s", SDL_GetError());
        return 1;
    } else {
        int ww, wh, glw, glh, rw, rh;
        SDL_GetWindowSize(state->window, &ww, &wh);
        SDL_Log("Window resolution: %dx%d", ww, wh);
        SDL_GL_GetDrawableSize(state->window, &glw, &glh);
        SDL_Log("Draw resolution: %dx%d", glw, glh);
        SDL_Renderer *renderer = SDL_GetRenderer(state->window);
        if (NULL != renderer) {
            if (SDL_GetRendererOutputSize(renderer, &rw, &rh) == 0) {
                SDL_Log("Resolution: %dx%d", rw, rh);
            } else {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not get render resolution: %s", SDL_GetError());
            }
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

    unsigned short i = 0;
    unsigned short j = 0;
    unsigned short k = 0;

    if (sdl_init(&game) != 0) { return 1; }

    while(!game.ev.quit) {
        SDL_Event event;

        if (!game.ev.focused) {
           SDL_Delay(2);
           continue;
        }

        for (i = 0; i < 0xff; i+=10) {
            for (j = 0; j < 0xff; j+=10) {
                for (k = 0; k < 0xff; k+=10) {
                    while( SDL_PollEvent(&event) ) { }
                    render_frame(&game, i, j, k);
                    if (game.ev.quit) { break; }
                }
                if (game.ev.quit) { break; }
            }
            if (game.ev.quit) { break; }
        }
    }

    if (game.ev.quit) { SDL_Log("Closing."); }

    sdl_cleanup(&game);
    return EXIT_SUCCESS;
}
