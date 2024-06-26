#ifndef SHADER_H
#define SHADER_H

#include "../include/glad/glad/glad.h"

#include <string>

class Shader {

public:
  unsigned int Id;

  // The constructor read + build the shader
  Shader(const char *vertexPath, const char *fragmentPath);

  void use();

  void SetBool(const std::string &name, bool value) const;
  void SetInt(const std::string &name, int value) const;
  void SetFloat(const std::string &name, float value) const;
};

#endif // !SHADER_H
