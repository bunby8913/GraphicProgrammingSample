#ifndef SHADER_H
#define SHADER_H

#include "glad/glad/glad.h"
#include <fstream>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

class Shader {

public:
  unsigned int Id;

  // The constructor read + build the shader
  Shader(const char *vertexPath, const char *fragmentPath,
         const char *geometryPath = nullptr);

  void use();

  void SetBool(const std::string &name, bool value) const;
  void SetInt(const std::string &name, int value) const;
  void SetFloat(const std::string &name, float value) const;
  void setMat4(const std::string &name, glm::mat4 value) const;
  void SetVec3(const std::string &name, glm::vec3 value) const;
};

#endif // !SHADER_H

// Shader class constructor, read + parse the source file + compile the shader +
// create the shader program
Shader::Shader(const char *vertexPath, const char *fragmentPath,
               const char *geometryPath) {
  // Start by reading the shader source file into strings
  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;
  std::ifstream gShaderFile;
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
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
    if (geometryPath) {
      gShaderFile.open(geometryPath);
      std::stringstream gShaderStream;
      gShaderStream << gShaderFile.rdbuf();
      gShaderFile.close();
      geometryCode = gShaderStream.str();
    }
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }
  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();
  const char *gShaderCode = geometryCode.c_str();

  // Compile shaders
  // First create variable to store shader Id + store error codes
  unsigned int vertex, fragment, geometry;
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
  // Geometry shader
  if (geometryPath) {
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, nullptr);
    glCompileShader(geometry);
    glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(geometry, 512, nullptr, infoLog);
      std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION::FAILED\n"
                << infoLog << std::endl;
    }
  }
  // Now create the shader program with the vertex + fragment shader created
  // earlier
  Id = glCreateProgram();
  glAttachShader(Id, vertex);
  glAttachShader(Id, fragment);
  if (geometryPath)
    glAttachShader(Id, geometry);
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
void Shader::SetVec3(const std::string &name, glm::vec3 value) const {
  glUniform3fv(glGetUniformLocation(Id, name.c_str()), 1,
               glm::value_ptr(value));
}
