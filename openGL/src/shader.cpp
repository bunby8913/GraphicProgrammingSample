#include "../include/shader.h"
#include "glm-1.0.1-light/glm/ext/matrix_float4x4.hpp"
#include "glm-1.0.1-light/glm/gtc/type_ptr.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

// Shader class constructor, read + parse the source file + compile the shader +
// create the shader program
Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  // Start by reading the shader source file into strings
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // Start by opening the file + read content to a string stream
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vshaderStream, fShaderStream;
    vshaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // Close the file
    vShaderFile.close();
    fShaderFile.close();
    // Assign the stream to the code string
    vertexCode = vshaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  // Compile shaders
  // First create variable to store shader Id + store error codes
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
  // fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, nullptr);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION::FAILED\n"
              << infoLog << std::endl;
  }
  // Now create the shader program with the vertex + fragment shader created
  // earlier
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

// This function is used in the render loop to set the shader program to be used
void Shader::use() { glUseProgram(Id); }

// Supporting functions to set uniform with value
void Shader::SetBool(const std::string &name, bool value) const {
  glUniform1d(glGetUniformLocation(Id, name.c_str()), (int)value);
}
void Shader::SetInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(Id, name.c_str()), value);
}
void Shader::SetFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(Id, name.c_str()), value);
}
void Shader::setMat4(const std::string &name, glm::mat4 value) const {
  glUniformMatrix4fv(glGetUniformLocation(Id, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(value));
}
