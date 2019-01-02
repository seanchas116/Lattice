#pragma once
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include "../support/Pointer.hpp"

namespace Lattice {

class Shader final : protected QOpenGLExtraFunctions {
    Q_DISABLE_COPY(Shader)
public:
    Shader(const std::string &vertexShader, const std::string &geometryShader, const std::string &fragmentShader);
    ~Shader();

    void bind();
    void unbind();

    void setUniform(const char* name, int value);
    void setUniform(const char* name, bool value);

    void setUniform(const char* name, float value);
    void setUniform(const char* name, glm::vec2 value);
    void setUniform(const char* name, glm::vec3 value);
    void setUniform(const char* name, glm::vec4 value);
    void setUniform(const char* name, glm::mat2 value);
    void setUniform(const char* name, glm::mat3 value);
    void setUniform(const char* name, glm::mat4 value);

    void setUniform(const char* name, double value);
    void setUniform(const char* name, glm::dvec2 value);
    void setUniform(const char* name, glm::dvec3 value);
    void setUniform(const char* name, glm::dvec4 value);
    void setUniform(const char* name, glm::dmat2 value);
    void setUniform(const char* name, glm::dmat3 value);
    void setUniform(const char* name, glm::dmat4 value);

private:
    GLuint loadShader(GLenum type, const std::string &src);
    GLuint _program;
};

}
