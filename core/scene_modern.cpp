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
  -0.5f, -0.5f,  0.0f
};

GLuint vbo = 0;
GLuint vao = 0;
GLuint vs, fs;
GLuint shader_program;

namespace vvr {
    struct SceneModern::Impl : QOpenGLExtraFunctions {
        //! This class is used to avoid including 
        //! QOpenGLExtraFunctions in the header
        //! as it would have been necessary if 
        // 'SceneModern' inherited from it.
    };
}

/*--------------------------------------------------------------------------------------*/
vvr::SceneModern::SceneModern()
{
    m_bg_col = vvr::grey;
    impl = new Impl;
}

vvr::SceneModern::~SceneModern()
{
    delete impl;
}

void vvr::SceneModern::resize()
{
    if (m_first_resize) setupGL();
}

void vvr::SceneModern::setupGL()
{
#if VVR_ENABLE_MODERN_GL
    int  ok;
    char infoLog[512];

    impl->initializeOpenGLFunctions();
    impl->glGenBuffers(1, &vbo);
    impl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    impl->glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), pts_data, GL_STATIC_DRAW);
    impl->glGenVertexArrays(1, &vao);
    impl->glBindVertexArray(vao);
    impl->glEnableVertexAttribArray(0);
    impl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    impl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    vs = impl->glCreateShader(GL_VERTEX_SHADER);
    impl->glShaderSource(vs, 1, &vertex_shader, NULL);
    impl->glCompileShader(vs);
    impl->glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) { impl->glGetShaderInfoLog(vs, 512, NULL, infoLog); vvr_msg(infoLog); }

    fs = impl->glCreateShader(GL_FRAGMENT_SHADER);
    impl->glShaderSource(fs, 1, &fragment_shader, NULL);
    impl->glCompileShader(fs);
    impl->glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) { impl->glGetShaderInfoLog(fs, 512, NULL, infoLog); vvr_msg(infoLog); }

    shader_program = impl->glCreateProgram();
    impl->glAttachShader(shader_program, fs);
    impl->glAttachShader(shader_program, vs);
    impl->glLinkProgram(shader_program);
#endif
}

void vvr::SceneModern::draw()
{
#if VVR_ENABLE_MODERN_GL
    enterPixelMode();
    impl->glUseProgram(shader_program);
    impl->glBindVertexArray(vao);
    impl->glDrawArrays(GL_TRIANGLES, 0, 3);
    exitPixelMode();
#endif
}
/*--------------------------------------------------------------------------------------*/

vvr_invoke_main_with_scene(vvr::SceneModern)
