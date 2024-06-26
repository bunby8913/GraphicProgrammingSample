#include "../include//shader.h"
#include "../include/glad/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

// Function definition
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

// Process input called every frame, use GLFW to determine if there are any
// input from the user when the new frame loads
void processInput(GLFWwindow *window) {
  // If escape key is pressed, set windows should be closed to true
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// Z-axis are set to 0.0 to simulate 2D coordinate
float vertices[] = {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.0f,
                    0.0f,  1.0f,  0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,  1.0f};

int main(int argc, char *argv[]) {
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

  // Create a vertex array object
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  // Bind vertex array object
  glBindVertexArray(VAO);
  // Specify the types of the buffer object to be an array buffer object
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // copy the vertices into the array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // SPecify how OpenGL should interpret the vertex data
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  // Draw int wireframe mode
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnableVertexAttribArray(0);

  // Specify how openGL should interpret the color data and pass it to the
  // eshader
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Use the shader constructor to import and compile vertex + fragment
  // vertexShader
  Shader ourShader("../shader/shader.vs", "../shader/shader.fs");

  // This is the render loop, keep running until window closed is called
  while (!glfwWindowShouldClose(window)) {
    // Swap the color buffer + show output
    processInput(window);
    // Specify the color used for clear
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // Actually clear the color buffer, fill the buffer with the color set
    // earlier
    glClear(GL_COLOR_BUFFER_BIT);
    ourShader.use();
    float timeValue = glfwGetTime();
    float rightShift = sin(timeValue) / 2.0f;
    int vertexShiftLocation =
        glGetUniformLocation(ourShader.Id, "horizontalOffset");
    glUniform1f(vertexShiftLocation, rightShift);

    glBindVertexArray(VAO);
    // First parameter determine what primitive shape to draw
    glDrawArrays(GL_TRIANGLES, 0, 3);
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
