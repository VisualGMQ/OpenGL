#define GLEW_STATIC
#include <iostream>
#include <cassert>
#include "GL/glew.h"
#include "SDL.h"

class App {
 public:
    App() {
        SDL_Log("Start App");
        initSystem();
        SDL_Log("Inited System");
    }

    void Run() {
        while (!ShouldClose()) {
            GLfloat bgcolor[4] = {0.1, 0.1, 0.1, 1};
            glClearBufferfv(GL_COLOR, 0, bgcolor);
            eventHandle();
            step();
            SDL_GL_SwapWindow(_window);
            SDL_Delay(30);
        }
    }

    bool ShouldClose() {
        return _isquit;
    }

    ~App() {
        quitSystem();
    }

 private:
    void initSystem() {
        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        _window = SDL_CreateWindow("OpenGL App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL);
        if (!_window) {
            SDL_Log("window create failed");
            assert(_window != nullptr);
        }
        SDL_GL_CreateContext(_window);

        // must call it after SDL_GL_CreateContext
        glewInit();
    }

    void eventHandle() {
        while (SDL_PollEvent(&_event)) {
            if (_event.type == SDL_QUIT)
                _isquit = true;
            if (_event.type == SDL_WINDOWEVENT) {
                if (_event.window.event == SDL_WINDOWEVENT_RESIZED) {   // when window resized, resize viewport at the same time
                    glViewport(0, 0, _event.window.data1, _event.window.data2);
                }
            }
            if (_event.type == SDL_KEYDOWN) {
                if (_event.key.keysym.sym == SDLK_ESCAPE) { // press ESC to quit App
                    _isquit = true;
                }
            }
        }
    }

    void quitSystem() {
        SDL_Quit();
    }

    void step() {
        // TODO your code
    }

    SDL_Window* _window;
    SDL_Event _event;
    bool _isquit = false;
};

int main(int argc, char** argv) {
    App app;
    app.Run();
    return 0;
}
