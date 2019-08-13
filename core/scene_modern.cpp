#include <vvr/scene_modern.h>
#include <vvr/drawing.h>
#include <MathGeoLib.h>
#include <QtGui>
#include <qopenglext.h>

/*--------------------------------------------------------------------------------------*/
const char* vertex_shader =
        "#version 410\n"
        "in vec3 vp;"
        "void main() {"
        "  gl_Position = vec4(vp, 1.0);"
        "}";

const char* fragment_shader =
        "#version 410\n"
        "out vec4 frag_colour;"
        "void main() {"
        "  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
        "}";

/*--------------------------------------------------------------------------------------*/
float pts_data[] = {
    0.0f,  0.5f,  0.0f,
    0.5f, -0.5f,  0.0f,
   -0.5f, -0.5f,  0.0f,

    0.0f,  0.5f,  0.0f,
    0.5f, -0.8f,  0.0f,
   -0.5f, -0.5f,  0.0f,
};

GLuint vbo = 0;
GLuint vao = 0;
GLuint vs, fs;
GLuint shader_program;

namespace vvr {
    //! This class is used to avoid including QOpenGLExtraFunctions in the header
    //! as it would have been necessary if SceneModern inherited from it.
    struct SceneModern::Impl : QOpenGLExtraFunctions { };
}

const char * vvr::SceneModern::name = "Modern OpenGL scene";

/*--------------------------------------------------------------------------------------*/
vvr::SceneModern::SceneModern()
{
    m_bg_col = vvr::grey;
    m_show_log = true;
    gl = new Impl;
}

vvr::SceneModern::~SceneModern()
{
    delete gl;
}

void vvr::SceneModern::resize()
{
    if (m_first_resize) {
        setupGL();
    }
}

void vvr::SceneModern::setupGL()
{
    char infoLog[512];
    int  ok;

    gl->initializeOpenGLFunctions();

    //---[Buffers]---
    //! ArrayBuffer
    gl->glGenBuffers(1, &vbo);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(pts_data), pts_data, GL_STATIC_DRAW);
    //! VertexArray
    gl->glGenVertexArrays(1, &vao);
    gl->glBindVertexArray(vao);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //---[Vertex shader]---
    vs = gl->glCreateShader(GL_VERTEX_SHADER);
    gl->glShaderSource(vs, 1, &vertex_shader, NULL);
    gl->glCompileShader(vs);
    gl->glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) { gl->glGetShaderInfoLog(vs, 512, NULL, infoLog); vvr_msg(infoLog); }

    //---[Fragment shader]---
    fs = gl->glCreateShader(GL_FRAGMENT_SHADER);
    gl->glShaderSource(fs, 1, &fragment_shader, NULL);
    gl->glCompileShader(fs);
    gl->glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) { gl->glGetShaderInfoLog(fs, 512, NULL, infoLog); vvr_msg(infoLog); }

    //---[Compile / Link shader]---
    shader_program = gl->glCreateProgram();
    gl->glAttachShader(shader_program, fs);
    gl->glAttachShader(shader_program, vs);
    gl->glLinkProgram(shader_program);
}

void vvr::SceneModern::draw()
{
    gl->glUseProgram(shader_program);
    gl->glBindVertexArray(vao);
    gl->glDrawArrays(GL_TRIANGLES, 0, 3);
}
/*--------------------------------------------------------------------------------------*/

vvr_invoke_main_with_scene(vvr::SceneModern)
