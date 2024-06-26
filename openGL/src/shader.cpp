#include "../include/shader.h"
#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vshaderStream, fShaderStream;
    vshaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vShaderFile.close();
    fShaderFile.close();
    vertexCode = vshaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  // Compile shaders
  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  // vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, nullptr);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION::FAILED\n"
              << infoLog << std::endl;
  }
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, nullptr);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION::FAILED\n"
              << infoLog << std::endl;
  }
  Id = glCreateProgram();
  glAttachShader(Id, vertex);
  glAttachShader(Id, fragment);
  glLinkProgram(Id);
  glGetProgramiv(Id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(Id, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::LINKER::COMPILATION::FAILED\n"
              << infoLog << std::endl;
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() { glUseProgram(Id); }

void Shader::SetBool(const std::string &name, bool value) const {
  glUniform1d(glGetUniformLocation(Id, name.c_str()), (int)value);
}
void Shader::SetInt(const std::string &name, int value) const {
  glUniform1d(glGetUniformLocation(Id, name.c_str()), value);
}
void Shader::SetFloat(const std::string &name, float value) const {
  glUniform1d(glGetUniformLocation(Id, name.c_str()), value);
}