#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// OpenGL / glew Headers
//#define GLEW_STATIC
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
//#include <GL/glu.h>
#include <GL/gl.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "smath.h"

struct global_events {
    bool focused;
    bool fullscreen;
    bool quit;
    bool focus_changed;
    bool resized;
    sfvec2 size;
    sfvec2 mouse_pos;
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
    sfvec4 color;
    sfvec2 resolution;
};

#define QUOTE(...) #__VA_ARGS__
const char *vertex_shader = " \
#version 130\n \
layout(location = 0) in vec3 vertexPosition_modelspace; \n\
void main() \
{ \
    gl_Position.xyz = vertexPosition_modelspace; \
    gl_Position.w = 1.0; \
} \
";
const char *fragment_shader = " \
#version 130\n \
out vec3 color; \n\
void main() \
{ \
    color = vec3(1,0,0); \
} \
";

int SDLCALL sdl_event_dispatch(void *userdata, SDL_Event* event)
{
    struct global_events *ev = userdata;
    bool previous_focus = ev->focused;

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
        ev->mouse_pos.x = (float)(motion.x + motion.xrel);
        ev->mouse_pos.y = (float)(motion.y + motion.yrel);
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
                ev->resized = true;
                ev->size.w = window.data1;
                ev->size.h = window.data2;
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                SDL_Log("Window %d size changed to %dx%d", window.windowID, window.data1, window.data2);
                ev->resized = true;
                ev->size.w = window.data1;
                ev->size.h = window.data2;
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
    if (ev->focused != previous_focus) {
        ev->focus_changed = true;
        SDL_Log("Focus changed: %s -> %s", previous_focus ? "focused" : "unfocused", ev->focused ? "focused" : "unfocused");
    }

    return 1;
}

GLuint load_shader(const char* shader_source, GLint shader_type)
{
    GLint compile_status = GL_FALSE;
    int log_length;

    GLuint result = glCreateShader(shader_type);
    SDL_Log("Compiling shader: %s", shader_source);
    glShaderSource(result, 1, &shader_source, NULL);
    glCompileShader(result);

    glGetShaderiv(result, GL_COMPILE_STATUS, &compile_status);
    glGetShaderiv(result, GL_INFO_LOG_LENGTH, &log_length);

    if (log_length > 0) {
        char *error = calloc(1, (sizeof(char) * log_length) + 1);
        glGetShaderInfoLog(result, log_length, NULL, error);

        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Shader compile error: %s", error);
    }

    return result;
}

int sdl_init(struct global_game_state *state)
{
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

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
        state->resolution.w,
        state->resolution.h,
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
    // Set our OpenGL version.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Turn on double buffering with a 24bit Z buffer.
    // You may need to change this to 16 or 32 for your system
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // This makes our buffer swap syncronized with the monitor's vertical refresh
    SDL_GL_SetSwapInterval(1);

    // Init GLEW
    // Apparently, this is needed for Apple. Thanks to Ross Vander for letting me know
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize GLEW");
    }
#endif

    // SDL_AddEventWatch(sdl_event_dispatch, &(state->ev));

    int major, minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    SDL_Log("OpenGL version: %d.%d", major, minor);

#if 1
    //sfvec4 black = {.r = 0x00, .g = 0x00, .b = 0x00 };
    sfvec4 magenta = {.r = 0xee, .g = 0x00, .b = 0xff };

    glClearColor(magenta.r, magenta.g, magenta.b, magenta.a );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    SDL_GL_SwapWindow(state->window);
#endif

    SDL_GL_MakeCurrent(state->window, state->gl_context);

    GLuint vert_shader_id = load_shader(vertex_shader, GL_VERTEX_SHADER);
    GLuint frag_shader_id = load_shader(fragment_shader, GL_FRAGMENT_SHADER);

    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vert_shader_id);
    glAttachShader(program_id, frag_shader_id);

    GLint program_status;
    int log_length;
    glGetProgramiv(program_id, GL_LINK_STATUS, &program_status);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0) {
        char *error = calloc(1, (sizeof(char) * log_length) + 1);
        glGetShaderInfoLog(program_status, log_length, NULL, error);

        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Shader compile error: %s", error);
    }

    glDetachShader(program_id, vert_shader_id);
    glDetachShader(program_id, frag_shader_id);

    glDeleteShader(vert_shader_id);
    glDeleteShader(frag_shader_id);

    glUseProgram(program_id);

    return 0;
}

void sdl_cleanup(struct global_game_state *state)
{
    //SDL_DelEventWatch(sdl_event_dispatch, &(state->ev));
    SDL_GL_DeleteContext(state->gl_context);
    SDL_Renderer *renderer = SDL_GetRenderer(state->window);
    if (NULL != renderer) { SDL_DestroyRenderer(renderer); }
    SDL_DestroyWindow(state->window);
    SDL_Quit();
}

static bool initialized = false;
void render_frame(struct global_game_state *state)
{
    GLuint vertexbuffer;
    if (true || !initialized) {
        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        const GLfloat g_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f,  1.0f, 0.0f,
        };
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER,
            sizeof(g_vertex_buffer_data),
            g_vertex_buffer_data,
            GL_STATIC_DRAW
        );

        initialized = true;
    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
#if 0
    state->color.r = clampf(0.0f, state->ev.mouse_pos.x / state->resolution.w, 1.0f);
    state->color.g = clampf(0.0f, state->ev.mouse_pos.y / state->resolution.h, 1.0f);
    state->color.b = clampf(0.0f, state->color.b, 1.0f);

    glClearColor(state->color.r, state->color.g, state->color.b, state->color.a);
    glClear(GL_COLOR_BUFFER_BIT);
#endif
    // Swap buffers
    SDL_GL_SwapWindow(state->window);
}

int main(int argc, char *argv[])
{
    struct global_game_state game = {NULL};
    game.resolution.w = 512;
    game.resolution.h = 512;

    SDL_Event event;

    if (sdl_init(&game) != 0) { return 1; }

    while (!game.ev.quit) {
        while (SDL_PollEvent(&event)) { sdl_event_dispatch(&game.ev, &event); }

        if (game.ev.resized) {
            game.resolution.w = game.ev.size.w;
            game.resolution.h = game.ev.size.h;
        }

        if (game.ev.fullscreen) {
            SDL_SetWindowFullscreen(game.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        } else {
            SDL_SetWindowFullscreen(game.window, 0);
        }
        if (game.ev.focus_changed) {
            if (game.ev.focused) { SDL_Log("Rendering");
            } else { SDL_Log("Stopped rendering"); }
            game.ev.focus_changed = false;
        }
        if (game.ev.focused) {
            render_frame(&game);
        } else {
            SDL_Delay(200);
        }
    }
    SDL_Log("Closing.");

    sdl_cleanup(&game);
    return EXIT_SUCCESS;
}
