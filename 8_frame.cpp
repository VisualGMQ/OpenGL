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
        prepTexture();
        SDL_Log("Prepared Texture");
        prepFrameBuffer();
        prepRenderBuffer();
        CheckFramebufferValid();
        SDL_Log("Prepared frame, render buffers");

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
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

        // set bit length of r, b, g, a
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);        // open double buffer
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);  // open GPU accelerate

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
        loadShader(_vshader, GL_VERTEX_SHADER, "./shaders/4_3d.vs");
        SDL_Log("Loaded Vertex Shader");
        loadShader(_fshader, GL_FRAGMENT_SHADER, "./shaders/4_3d.fs");
        SDL_Log("Loaded Fragment Shader");
        _program_frame = createProgram();
        compileLinkProgram(_program_frame);
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
            SDL_Log("shader compile error:\n\n%s", buffer);
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

    void prepFrameBuffer() {
        glGenFramebuffers(1, &_frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);

        glGenTextures(1, &_frame_tex);
        glBindTexture(GL_TEXTURE_2D, _frame_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, GL_FALSE, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _frame_tex, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void prepRenderBuffer() {
        glGenRenderbuffers(1, &_render_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, _render_buffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _render_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void CheckFramebufferValid() {
        glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            SDL_Log("Frame Buffer invalid");
            assert(false);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void prepTexture() {
        glActiveTexture(0);
        glGenTextures(1, &_tex);
        glBindTexture(GL_TEXTURE_2D, _tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        SDL_Surface* surface = IMG_Load("resources/cube_man.png");
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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glGenBuffers(1, &_texvbo);
        glBindBuffer(GL_ARRAY_BUFFER, _texvbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(coord), coord, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), static_cast<void*>(0));
        glEnableVertexAttribArray(1);

        SDL_FreeSurface(surface);

        GLuint loc = glGetUniformLocation(_program_frame, "tex");
        glUniform1i(loc, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void quitSystem() {
        SDL_Quit();
    }

    void step() {
        glUseProgram(_program_frame);
        glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, _tex);
        DrawStencilImage();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, _frame_tex);
        DrawSimpleImage();
    }

    void DrawStencilImage() {
        setMatrixes(_program_frame, 0, 0, 0.6, 0.6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        setMatrixes(_program_frame, -0.6, -0.6, 0.6, 0.6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        setMatrixes(_program_frame, 0.6, -0.6, 0.6, 0.6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        setMatrixes(_program_frame, 0.6, 0.6, 0.6, 0.6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        setMatrixes(_program_frame, -0.6, 0.6, 0.6, 0.6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void DrawSimpleImage() {
        setMatrixes(_program_frame, 0, 0, 1.5, 1.5);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void setMatrixes(GLuint program, float deltax, float deltay, float scalex, float scaley) {
        static GLuint model_loc = glGetUniformLocation(program, "model"),
               view_loc = glGetUniformLocation(program, "view"),
               project_loc = glGetUniformLocation(program, "project");

        glm::mat4 model(1.0), view(1.0),
                  project = glm::perspective(glm::radians(45.0f), WindowWidth/WindowHeight, 0.1f, 100.0f);
        model = glm::scale(model, glm::vec3(scalex, scaley, 1));
        model = glm::translate(model, glm::vec3(deltax, deltay, 0));
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
    GLuint _program_frame;
    GLuint _tex;
    GLuint _texvbo;
    GLuint _frame_buffer;
    GLuint _render_buffer;
    GLuint _frame_tex;

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
