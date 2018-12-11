#pragma once
#include <QOpenGLExtraFunctions>
#include <QUrl>
#include <glm/glm.hpp>
#include "../support/Pointer.hpp"

namespace Lattice {

class Shader final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(Shader)
public:
    Shader(const QByteArray& vertexShader, const QByteArray& geometryShader, const QByteArray& fragmentShader);
    Shader(const QUrl& vertexShaderUrl, const QUrl& geometryShaderUrl, const QUrl& fragmentShaderUrl);
    ~Shader();

    void bind();
    void unbind();

    void setUniform(const char* name, float value);
    void setUniform(const char* name, glm::vec2 value);
    void setUniform(const char* name, glm::vec3 value);
    void setUniform(const char* name, glm::vec4 value);
    void setUniform(const char* name, glm::mat2 value);
    void setUniform(const char* name, glm::mat3 value);
    void setUniform(const char* name, glm::mat4 value);

private:
    GLuint loadShader(GLenum type, const QByteArray& src);
    GLuint _program;
};

}
