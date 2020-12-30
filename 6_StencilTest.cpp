#define GLEW_STATIC
#include <fstream>
#include <string>
#include <streambuf>
#include <cassert>
#include <iostream>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "SDL.h"
#include "SDL_image.h"
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
        SDL_Log("Used Program");
        prepTexture();
        SDL_Log("Prepared Texture");

        glEnable(GL_STENCIL_TEST);
        /* can open only one character every time
           glEnable(GL_DEPTH_TEST|GL_STENCIL_TEST) will not work */
        glEnable(GL_DEPTH_TEST);
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
        IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG);

        // OpenGL 4.1 core
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);    // NOTE must open stencil, this default to 0
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);     // NOTE you can also change depth, this default to 16
        _window = SDL_CreateWindow("OpenGL App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), static_cast<void*>(0));
        glEnableVertexAttribArray(0);
    }

    void prepProgram() {
        createShaders();
        SDL_Log("Created Shaders");
        loadShader(_vshader, GL_VERTEX_SHADER, "./shaders/6_stencil.vs");
        SDL_Log("Loaded Vertex Shader");
        loadShader(_fshader, GL_FRAGMENT_SHADER, "./shaders/6_stencil.fs");
        SDL_Log("Loaded Fragment Shader");
        _program_stencil = createProgram();
        compileLinkProgram(_program_stencil);
        SDL_Log("Linked Stencil Program");

        loadShader(_vshader, GL_VERTEX_SHADER, "./shaders/6_purecolor.vs");
        loadShader(_fshader, GL_FRAGMENT_SHADER, "./shaders/6_purecolor.fs");
        _program_purecolor = createProgram();
        compileLinkProgram(_program_purecolor);
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

    GLuint createProgram() {
        return glCreateProgram();
    }

    void compileLinkProgram(GLuint program) {
        glAttachShader(program, _vshader);
        glAttachShader(program, _fshader);
        glLinkProgram(program);
        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (!status) {
            char buffer[1024] = {0};
            glGetProgramInfoLog(program, sizeof(buffer), nullptr, buffer);
            SDL_Log("program link error:\n\n%s", buffer);
        }
    }

    void prepTexture() {
        glActiveTexture(0);
        glGenTextures(1, &_tex);
        glBindTexture(GL_TEXTURE_2D, _tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        SDL_Surface* surface = IMG_Load("resources/wall.jpg");
        if (!surface) {
            SDL_Log("resources/wall.jpg can't find");
            assert(false);
        }

        GLfloat coord[] = {
            0, 0,
            0, 1,
            1, 0,
            1, 1
        };

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glGenBuffers(1, &_texvbo);
        glBindBuffer(GL_ARRAY_BUFFER, _texvbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(coord), coord, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), static_cast<void*>(0));
        glEnableVertexAttribArray(1);

        SDL_FreeSurface(surface);

        GLuint loc = glGetUniformLocation(_program_stencil, "tex");
        glUniform1i(loc, 0);
    }

    void quitSystem() {
        SDL_Quit();
    }

    void step() {
        glStencilMask(0xFF);    // must open stencil after clear it
        glClear(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

        // first draw origin texture
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        drawOriginTexture();

        // second draw pure color rectangle
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        drawPureColor();
    }

    void drawOriginTexture() {
        glUseProgram(_program_stencil);
        setMatrixesStencil();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void setMatrixesStencil() {
        static GLuint model_loc = glGetUniformLocation(_program_stencil, "model"),
               view_loc = glGetUniformLocation(_program_stencil, "view"),
               project_loc = glGetUniformLocation(_program_stencil, "project");

        glm::mat4 model(1.0), view(1.0),
                  project = glm::perspective(glm::radians(45.0f), WindowWidth/WindowHeight, 0.1f, 100.0f);
        view = glm::translate(view, glm::vec3(0, 0, -2));

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(project_loc, 1, GL_FALSE, glm::value_ptr(project));
    }

    void drawPureColor() {
        glUseProgram(_program_purecolor);
        setMatrixesPureColor();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void setMatrixesPureColor() {
        static GLuint model_loc = glGetUniformLocation(_program_purecolor, "model"),
               view_loc = glGetUniformLocation(_program_purecolor, "view"),
               project_loc = glGetUniformLocation(_program_purecolor, "project");

        glm::mat4 model(1.0), view(1.0),
                  project = glm::perspective(glm::radians(45.0f), WindowWidth/WindowHeight, 0.1f, 100.0f);
        model = glm::scale(model, glm::vec3(1.2, 1.2, 0));
        view = glm::translate(view, glm::vec3(0, 0, -2));

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(project_loc, 1, GL_FALSE, glm::value_ptr(project));
    }



    GLuint _vao;
    GLuint _vbo;
    GLuint _ebo;
    GLuint _vshader;
    GLuint _fshader;
    GLuint _program_stencil;
    GLuint _program_purecolor;
    GLuint _tex;
    GLuint _texvbo;

    static constexpr float WindowWidth = 800;
    static constexpr float WindowHeight = 600;

    SDL_Window* _window;
    SDL_Event _event;
    bool _isquit = false;
};

int main(int argc, char** argv) {
    App app;
    app.Run();
    return 0;
}
