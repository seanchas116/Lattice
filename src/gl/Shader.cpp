#include "Shader.hpp"
#include <QtDebug>

namespace Lattice {
namespace GL {

Shader::Shader(const std::string &vertexShader, const std::string &geometryShader, const std::string &fragmentShader) {
    initializeOpenGLFunctions();

    GLuint program = glCreateProgram();
    glAttachShader(program, loadShader(GL_VERTEX_SHADER, vertexShader));
    if (!geometryShader.empty()) {
        glAttachShader(program, loadShader(GL_GEOMETRY_SHADER, geometryShader));
    }
    glAttachShader(program, loadShader(GL_FRAGMENT_SHADER, fragmentShader));
    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint logLen;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);

        std::string log(logLen, 0);
        glGetProgramInfoLog(program, logLen, nullptr, log.data());

        glDeleteProgram(_program);

        qWarning() << log.data();
    }
    _program = program;
}

Shader::~Shader() {
    glDeleteProgram(_program);
}

void Shader::bind() {
    glUseProgram(_program);
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::setUniform(const char *name, bool value) {
    glUseProgram(_program);
    glUniform1i(glGetUniformLocation(_program, name), value);
}

void Shader::setUniform(const char *name, int value) {
    glUseProgram(_program);
    glUniform1i(glGetUniformLocation(_program, name), value);
}

void Shader::setUniform(const char *name, glm::ivec2 value) {
    glUseProgram(_program);
    glUniform2i(glGetUniformLocation(_program, name), value.x, value.y);
}

void Shader::setUniform(const char *name, glm::ivec3 value) {
    glUseProgram(_program);
    glUniform3i(glGetUniformLocation(_program, name), value.x, value.y, value.z);
}

void Shader::setUniform(const char *name, glm::ivec4 value) {
    glUseProgram(_program);
    glUniform4i(glGetUniformLocation(_program, name), value.x, value.y, value.z, value.w);
}

void Shader::setUniform(const char *name, float value) {
    glUseProgram(_program);
    glUniform1f(glGetUniformLocation(_program, name), value);
}

void Shader::setUniform(const char *name, glm::vec2 value) {
    glUseProgram(_program);
    glUniform2f(glGetUniformLocation(_program, name), value.x, value.y);
}

void Shader::setUniform(const char *name, glm::vec3 value) {
    glUseProgram(_program);
    glUniform3f(glGetUniformLocation(_program, name), value.x, value.y, value.z);
}

void Shader::setUniform(const char *name, glm::vec4 value) {
    glUseProgram(_program);
    glUniform4f(glGetUniformLocation(_program, name), value.x, value.y, value.z, value.w);
}

void Shader::setUniform(const char *name, glm::mat2 value) {
    glUseProgram(_program);
    glUniformMatrix2fv(glGetUniformLocation(_program, name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setUniform(const char *name, glm::mat3 value) {
    glUseProgram(_program);
    glUniformMatrix3fv(glGetUniformLocation(_program, name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setUniform(const char *name, glm::mat4 value) {
    glUseProgram(_program);
    glUniformMatrix4fv(glGetUniformLocation(_program, name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setUniform(const char *name, double value) {
    setUniform(name, float(value));
}

void Shader::setUniform(const char *name, glm::dvec2 value) {
    setUniform(name, glm::vec2(value));
}

void Shader::setUniform(const char *name, glm::dvec3 value) {
    setUniform(name, glm::vec3(value));
}

void Shader::setUniform(const char *name, glm::dvec4 value) {
    setUniform(name, glm::vec4(value));
}

void Shader::setUniform(const char *name, glm::dmat2 value) {
    setUniform(name, glm::mat2(value));
}

void Shader::setUniform(const char *name, glm::dmat3 value) {
    setUniform(name, glm::mat3(value));
}

void Shader::setUniform(const char *name, glm::dmat4 value) {
    setUniform(name, glm::mat4(value));
}

GLuint Shader::loadShader(GLenum type, const std::string &src) {
    GLuint shader = glCreateShader(type);
    const char *srcData = src.data();

    glShaderSource(shader, 1, &srcData, nullptr);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint logLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

        std::string log(logLen, 0);
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());

        glDeleteShader(shader);

        qWarning() << log.data();
    }

    return shader;
}

} // namespace GL
} // namespace Lattice
