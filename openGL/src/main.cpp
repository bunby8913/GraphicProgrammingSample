#include "../include/glad/glad/glad.h"
#include "../include/shader.h"
#include "glm-1.0.1-light/glm/ext/matrix_clip_space.hpp"
#include "glm-1.0.1-light/glm/ext/matrix_float4x4.hpp"
#include "glm-1.0.1-light/glm/ext/matrix_transform.hpp"
#include "glm-1.0.1-light/glm/ext/vector_float3.hpp"
#include "glm-1.0.1-light/glm/fwd.hpp"
#include "glm-1.0.1-light/glm/geometric.hpp"
#include "glm-1.0.1-light/glm/trigonometric.hpp"
#include <GLFW/glfw3.h>
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

// Store the camera position + front vector + up vector as global value
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
// Store the camera direction as global value, allow camera rotation with mouse
glm::vec3 camerDirection;
// Cursor related global variables
float lastX = 400, lastY = 300;
float pitch = 0.0f, yaw = -90.0f;
bool firstMouse = true;
float zoom = 45.0f;

void mouse_callback(GLFWwindow *window, double posX, double posY) {
  if (firstMouse) {
    lastX = posX;
    lastY = posY;
    firstMouse = false;
  }
  float xOffset = posX - lastX;
  float yOffset = lastY - posY;
  lastX = posX;
  lastY = posY;
  const float sensitivity = 0.05f;
  xOffset *= sensitivity;
  yOffset *= sensitivity;
  yaw += xOffset;
  pitch += yOffset;

  // Camera pitch needs to be constraints between -90 to 90 degree
  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;
  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset) {
  zoom -= (float)yOffset;
  if (zoom < 1.0f)
    zoom = 1.0f;
  if (zoom > 45.0f)
    zoom = 45.0f;
}
// Process input called every frame, use GLFW to determine if there are any
// input from the user when the new frame loads
void processInput(GLFWwindow *window) {
  const float cameraSpeed = 2.5f * deltaTime;
  // If escape key is pressed, set windows should be closed to true
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
  else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
  else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos +=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// Trying out making custom lookat functions
glm::mat4 customLookAt(glm::vec3 cameraPosition, glm::vec3 cameraLookLocation,
                       glm::vec3 worldUp) {
  glm::mat4 lookAtMatrix = glm::mat4(1.0f);
  glm::vec3 cameraDirection =
      glm::normalize(cameraPosition - cameraLookLocation);
  glm::vec3 rightAxis = glm::normalize(glm::cross(worldUp, cameraDirection));
  glm::vec3 upAxis = glm::normalize(glm::cross(cameraDirection, rightAxis));
  lookAtMatrix =
      glm::mat4(glm::vec4(rightAxis.x, upAxis.x, cameraDirection.x, 0.0f),
                glm::vec4(rightAxis.y, upAxis.y, cameraDirection.y, 0.0f),
                glm::vec4(rightAxis.z, upAxis.z, cameraDirection.z, 0.0f),
                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) *
      glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                glm::vec4(-(cameraPosition.x), -(cameraPosition.y),
                          -(cameraPosition.z), 1.0f));
  return lookAtMatrix;
}

float vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

    -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

glm::vec3 cubePositions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

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
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // Create a model matrix
  glm::mat4 model = glm::mat4(1.0f);
  // This will help with transforming vertex to world coordinates
  // Create a view matrix, move the camera backward to moving the scene forward
  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  // Last, create the projection matrix
  glm::mat4 projection;

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

  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  //             GL_STATIC_DRAW);

  // Read vertex position data and pass it to the shader through
  // location 0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Read vertex texture data + pass it to the shader through
  // location 2
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // Draw int wireframe mode
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // enable depth test using the Z-buffer
  glEnable(GL_DEPTH_TEST);
  // Set up the input mode to capture mouse, and disable the mouse cursor to
  // show
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Use the shader constructor to import and compile vertex +
  // fragment vertexShader
  Shader ourShader("../shader/shader.vs", "../shader/shader.fs");

  ourShader.use();
  ourShader.SetInt("texture1", 0);
  ourShader.SetInt("texture2", 1);
  // This is the render loop, keep running until window closed is
  // called
  while (!glfwWindowShouldClose(window)) {
    // Swap the color buffer + show output
    processInput(window);
    // Specify the color used for clear
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // Actually clear the color buffer, fill the buffer with the
    // color set earlier
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Use the shader program to draw
    int modelLoc = glGetUniformLocation(ourShader.Id, "model");

    const float radius = 10.0f;
    // Change the camera location base on Sin and Cos
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;
    cameraPos.y = 0.0f;
    view = customLookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    modelLoc = glGetUniformLocation(ourShader.Id, "view");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(view));

    projection =
        glm::perspective(glm::radians(zoom), 800.f / 600.f, 0.1f, 100.f);
    modelLoc = glGetUniformLocation(ourShader.Id, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // Update deltatime and lastFrame
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glBindVertexArray(VAO);
    // First parameter determine what primitive shape to draw
    for (unsigned int i = 0; i < 10; ++i) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      if (i % 3 == 0) {
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(45.f),
                            glm::vec3(0.5f, 1.0f, 0.0f));
      } else {
        float angle = 20.f * i;
        model = glm::rotate(model, glm::radians(angle),
                            glm::vec3(1.0f, 0.3f, 0.5f));
      }
      ourShader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Check if any events are triggered, call functions through
    // callbacks i.e.

    glfwSwapBuffers(window);
    // the framebuffer_size_callback
    glfwPollEvents();
  }

  // Terminate GLFW
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
