#include "../include/glad/glad/glad.h"
#include "../include/shader.h"
#include "glm-1.0.1-light/glm/ext/matrix_float4x4.hpp"
#include "glm-1.0.1-light/glm/ext/matrix_transform.hpp"
#include "glm-1.0.1-light/glm/ext/vector_float4.hpp"
#include "glm-1.0.1-light/glm/trigonometric.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm-1.0.1-light/glm/glm.hpp>
#include <glm-1.0.1-light/glm/gtc/matrix_transform.hpp>
#include <glm-1.0.1-light/glm/gtc/type_ptr.hpp>

// Function definition
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

float mixValue = 0.5f;
void updateMixValue(bool up) {
  if (up)
    mixValue += 0.01f;
  else
    mixValue -= 0.01f;
  mixValue = std::max(0.0f, std::min(1.0f, mixValue));
}
// Process input called every frame, use GLFW to determine if there are any
// input from the user when the new frame loads
void processInput(GLFWwindow *window) {
  // If escape key is pressed, set windows should be closed to true
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    updateMixValue(true);
  else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    updateMixValue(false);
}

// Z-axis are set to 0.0 to simulate 2D coordinate
float vertices[] = {
    // positions // colors // texture coords
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left

};
unsigned int indices[] = {0, 1, 3, 1, 2, 3};

int main(int argc, char *argv[]) {
  glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 trans = glm::mat4(1.0f);
  // Initialize GLFW
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << std::endl;
    return -1;
  }
  // Configurate GLFW, set the openGL version to be 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // Set OpenGL to only use core profile, avoids any legacy features
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a GLFW window
  GLFWwindow *window =
      glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create OpenGL window" << std::endl;
    glfwTerminate();
    return -1;
  }
  // Make the newly created window the current context
  glfwMakeContextCurrent(window);

  // Initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // Set the viewport
  glViewport(0, 0, 800, 600);

  // Set the framebuffer size callback
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Create a buffer object with ID
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  // Creae a shader program to combine all the shaders together

  // Create a EBO with ID
  unsigned int EBO;
  glGenBuffers(1, &EBO);

  // Create a texture with id
  unsigned int texture;
  glGenTextures(1, &texture);
  // Bound the texture to type of a 2D texture
  glBindTexture(GL_TEXTURE_2D, texture);
  // Set texture wrapping
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // Set texture filtering method
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Use stb_image library to load the texture from image
  int width, height, nrChannel;
  unsigned char *data = stbi_load("../resource/img/container.jpg", &width,
                                  &height, &nrChannel, 0);
  if (data) {
    // Generate texture using the loaded texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  unsigned int texture2;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);

  stbi_set_flip_vertically_on_load(true);
  data = stbi_load("../resource/img/awesomeface.png", &width, &height,
                   &nrChannel, 0);
  if (data) {
    // Generate texture using the loaded texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  // Create a vertex array object
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  // Bind vertex array object
  glBindVertexArray(VAO);
  // Specify the types of the buffer object to be an array buffer object
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // copy the vertices into the array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Read vertex position data and pass it to the shader through location 0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Read vertex color data + pass it to the shader through location 1
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Read vertex texture data + pass it to the shader through location 2
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  // Draw int wireframe mode
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Use the shader constructor to import and compile vertex + fragment
  // vertexShader
  Shader ourShader("../shader/shader.vs", "../shader/shader.fs");

  ourShader.use();
  ourShader.SetInt("texture1", 0);
  ourShader.SetInt("texture2", 1);
  // This is the render loop, keep running until window closed is called
  while (!glfwWindowShouldClose(window)) {
    // Swap the color buffer + show output
    processInput(window);
    // Specify the color used for clear
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // Actually clear the color buffer, fill the buffer with the color set
    // earlier
    glClear(GL_COLOR_BUFFER_BIT);
    // Use the shader program to draw
    float timeValue = glfwGetTime();
    float scaleShift = sin(timeValue);
    ourShader.SetFloat("mixValue", mixValue);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans =
        glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    unsigned int transformLoc = glGetUniformLocation(ourShader.Id, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glBindVertexArray(VAO);
    // First parameter determine what primitive shape to draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-0.5, 0.5, 0.0f));
    trans = glm::scale(trans, glm::vec3(scaleShift, scaleShift, scaleShift));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Check if any events are triggered, call functions through callbacks i.e.

    glfwSwapBuffers(window);
    // the framebuffer_size_callback
    glfwPollEvents();
  }

  // Terminate GLFW
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
