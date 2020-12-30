#define GLEW_STATIC
#include <fstream>
#include <string>
#include <streambuf>
#include <cassert>
#include <iostream>
#include "GL/glew.h"
#include "SDL.h"
using std::fstream;
using std::ifstream;
using std::string;
using std::istreambuf_iterator;
using std::cout;
using std::endl;

class App {
 public:
    App() {
        SDL_Log("Start App");
        initSystem();
        SDL_Log("Inited System");
        prepDatas();
        SDL_Log("Prepared Datas");
        prepProgram();
        SDL_Log("Prepared Program");
        glUseProgram(_program);
        SDL_Log("Used Program");
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

        // OpenGL 4.1 core
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        _window = SDL_CreateWindow("OpenGL App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
        if (!_window) {
            SDL_Log("window create failed");
            assert(_window != nullptr);
        }
        SDL_GL_CreateContext(_window);

        // must call it after SDL_GL_CreateContext
        glewInit();
        glViewport(0, 0, 800, 600);
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

    void prepDatas() {
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ebo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

        GLfloat vertices[] = {  // vertices of triangle
            -0.5, -0.5,
            -0.5,  0.5,
             0.5, -0.5,
             0.5, 0.5
        };

        // indices must be GLuint
        GLuint indices[] = {
            0, 1, 2,    // first rectangle indices
            2, 1, 3     // second rectangle indices
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), static_cast<void*>(0));
        glEnableVertexAttribArray(0);
    }

    void prepProgram() {
        createShaders();
        SDL_Log("Created Shaders");
        loadShader(_vshader, GL_VERTEX_SHADER, "./shaders/pass_shader.vs");
        SDL_Log("Loaded Vertex Shader");
        loadShader(_fshader, GL_FRAGMENT_SHADER, "./shaders/pass_shader.fs");
        SDL_Log("Loaded Fragment Shader");
        createProgram();
        compileLinkProgram();
        SDL_Log("Linked Program");
    }

    void createShaders() {
        _vshader = glCreateShader(GL_VERTEX_SHADER);
        _fshader = glCreateShader(GL_FRAGMENT_SHADER);
    }

    void loadShader(GLuint shader_id, GLenum shader_type, string filepath) {
        ifstream file(filepath);
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

        const char* content_ptr = content.c_str();
        const GLchar* sources[] = {content_ptr, nullptr};
        glShaderSource(shader_id, 1, sources, nullptr);
        glCompileShader(shader_id);
        GLint status;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
        if (!status) {
            char buffer[1024] = {0};
            glGetShaderInfoLog(shader_id, sizeof(buffer), nullptr, buffer);
            SDL_Log("shader compile error:\n vertic shader:\n\n%s", buffer);
        }
    }

    void createProgram() {
        _program = glCreateProgram();
    }

    void compileLinkProgram() {
        glAttachShader(_program, _vshader);
        glAttachShader(_program, _fshader);
        glLinkProgram(_program);
        GLint status;
        glGetProgramiv(_program, GL_LINK_STATUS, &status);
        if (!status) {
            char buffer[1024] = {0};
            glGetProgramInfoLog(_program, sizeof(buffer), nullptr, buffer);
            SDL_Log("program link error:\n\n%s", buffer);
        }
    }

    void quitSystem() {
        SDL_Quit();
    }

    void step() {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    GLuint _vao;
    GLuint _vbo;
    GLuint _ebo;
    GLuint _vshader;
    GLuint _fshader;
    GLuint _program;

    SDL_Window* _window;
    SDL_Event _event;
    bool _isquit = false;
};

int main(int argc, char** argv) {
    App app;
    app.Run();
    return 0;
}
