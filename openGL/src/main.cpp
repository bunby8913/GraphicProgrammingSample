#include "../include/glad/glad/glad.h"
#include "../include/stb_image.h"
#include "glm-1.0.1-light/glm/ext/matrix_float4x4.hpp"
#include "glm-1.0.1-light/glm/ext/matrix_transform.hpp"
#include "glm-1.0.1-light/glm/ext/vector_float3.hpp"
#include "glm-1.0.1-light/glm/trigonometric.hpp"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../include/glm-1.0.1-light/glm/glm.hpp"
#include "../include/glm-1.0.1-light/glm/gtc/matrix_transform.hpp"
#include "../include/glm-1.0.1-light/glm/gtc/type_ptr.hpp"

#include "../include/camera.h"
#include "../include/shader.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile our shader zprogram
  // ------------------------------------
  Shader objectShader("../shader/camera.vs", "../shader/camera.fs");
  Shader lightSourceShader("../shader/light.vs", "../shader/light.fs");

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      // positions          // normals           // texture coords
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};
  // world space positions of our cubes
  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};
  // Define the location of all the point lights
  glm::vec3 pointLightPositions[] = {
      glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
      glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

  unsigned int VBO, VAO, lightVAO;
  glGenVertexArrays(1, &VAO);
  glGenVertexArrays(1, &lightVAO);
  glGenBuffers(1, &VBO);

  // Bind configuration to VAO
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Normal vector attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Bind configuration to lightVAO
  glBindVertexArray(lightVAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // load and create a texture
  // -------------------------
  unsigned int diffuseMap, specularMap, emissionMap;
  // texture 1
  // ---------
  glGenTextures(1, &diffuseMap);
  glBindTexture(GL_TEXTURE_2D, diffuseMap);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image, create texture and generate mipmaps
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(
      true); // tell stb_image.h to flip loaded texture's on the y-axis.
  unsigned char *data = stbi_load("../resource/img/container2.png", &width,
                                  &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  // texture 2
  // ---------
  glGenTextures(1, &specularMap);
  glBindTexture(GL_TEXTURE_2D, specularMap);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image, create texture and generate mipmaps
  data = stbi_load("../resource/img/container2_specular.png", &width, &height,
                   &nrChannels, 0);
  if (data) {
    // note that the awesomeface.png has transparency and thus an alpha channel,
    // so make sure to tell OpenGL the data type is of GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  glGenTextures(1, &emissionMap);
  glBindTexture(GL_TEXTURE_2D, emissionMap);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  data =
      stbi_load("../resource/img/matrix.jpg", &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  // tell opengl for each sampler to which texture unit it belongs to (only has
  // to be done once)
  // -------------------------------------------------------------------------------------------
  objectShader.use();
  objectShader.SetVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
  objectShader.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

  // Set material values for the fragment shader
  objectShader.SetVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
  objectShader.SetInt("material.diffuse", 0);
  objectShader.SetInt("material.specular", 1);
  objectShader.SetInt("material.emission", 2);
  objectShader.SetVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
  objectShader.SetFloat("material.shininess", 32.0f);

  objectShader.SetVec3("directLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
  objectShader.SetVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
  objectShader.SetVec3("directLight.direction", glm::vec3(-0.23, -1.0f, -0.3f));
  // Set up the point light drop of formula, setting up the point light distance
  // to be 50

  objectShader.SetVec3("pointLights[0].position", pointLightPositions[0]);
  objectShader.SetVec3("pointLights[0].ambient",
                       glm::vec3(0.05f, 0.05f, 0.05f));
  objectShader.SetVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
  objectShader.SetVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
  objectShader.SetFloat("pointLights[0].constant", 1.0f);
  objectShader.SetFloat("pointLights[0].linear", 0.09f);
  objectShader.SetFloat("pointLights[0].quadratic", 0.032f);
  // point light 2
  objectShader.SetVec3("pointLights[1].position", pointLightPositions[1]);
  objectShader.SetVec3("pointLights[1].ambient",
                       glm::vec3(0.05f, 0.05f, 0.05f));
  objectShader.SetVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
  objectShader.SetVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
  objectShader.SetFloat("pointLights[1].constant", 1.0f);
  objectShader.SetFloat("pointLights[1].linear", 0.09f);
  objectShader.SetFloat("pointLights[1].quadratic", 0.032f);
  // point light 3
  objectShader.SetVec3("pointLights[2].position", pointLightPositions[2]);
  objectShader.SetVec3("pointLights[2].ambient",
                       glm::vec3(0.05f, 0.05f, 0.05f));
  objectShader.SetVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
  objectShader.SetVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
  objectShader.SetFloat("pointLights[2].constant", 1.0f);
  objectShader.SetFloat("pointLights[2].linear", 0.09f);
  objectShader.SetFloat("pointLights[2].quadratic", 0.032f);
  // point light 4
  objectShader.SetVec3("pointLights[3].position", pointLightPositions[3]);
  objectShader.SetVec3("pointLights[3].ambient",
                       glm::vec3(0.05f, 0.05f, 0.05f));
  objectShader.SetVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
  objectShader.SetVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
  objectShader.SetFloat("pointLights[3].constant", 1.0f);
  objectShader.SetFloat("pointLights[3].linear", 0.09f);
  objectShader.SetFloat("pointLights[3].quadratic", 0.032f);
  // Change light color over time
  glm::vec3 diffuseColor = glm::vec3(0.5f);
  glm::vec3 ambientColor = glm::vec3(0.2f);
  objectShader.SetVec3("directLight.ambient", ambientColor);
  objectShader.SetVec3("pointLight.ambient", ambientColor);
  objectShader.SetVec3("spotLight.ambient", ambientColor);
  objectShader.SetVec3("directLight.diffuse", diffuseColor);
  objectShader.SetVec3("pointLight.diffuse", diffuseColor);
  objectShader.SetVec3("spotLight.diffuse", diffuseColor);
  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissionMap);

    // activate shader
    objectShader.use();
    objectShader.SetVec3("cameraPos", camera.Position);

    objectShader.SetVec3("spotLight.position", camera.Position);
    objectShader.SetVec3("spotLight.direction", camera.Front);
    objectShader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5)));
    objectShader.SetFloat("spotLight.outerCutOff",
                          glm::cos(glm::radians(15.0)));
    // pass projection matrix to shader (note that in this case it could change
    // every frame)
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    objectShader.setMat4("projection", projection);

    // camera/view transformation
    glm::mat4 view = camera.GetViewMatrix();
    objectShader.setMat4("view", view);
    // Set the camera position for fragment shader
    // render boxes
    for (unsigned int i = 0; i < 10; i++) {
      // calculate the model matrix for each object and pass it to shader before
      // drawing
      glm::mat4 model = glm::mat4(
          1.0f); // make sure to initialize matrix to identity matrix first
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      // float angle = 0.0f;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      objectShader.setMat4("model", model);
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    lightSourceShader.use();
    for (int i = 0; i < 4; ++i) {

      lightSourceShader.setMat4("projection", projection);
      lightSourceShader.setMat4("view", view);
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, pointLightPositions[i]);
      model = glm::scale(model, glm::vec3(0.2f));

      lightSourceShader.setMat4("model", model);

      glBindVertexArray(lightVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &VAO);
  glDeleteVertexArrays(1, &lightVAO);
  glDeleteBuffers(1, &VBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
