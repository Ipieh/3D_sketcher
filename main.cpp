// ----------------------------------------------------------------------------
// main.cpp
//
//  Created on: 24 Jul 2020
//      Author: Kiwon Um
//        Mail: kiwon.um@telecom-paris.fr
//
// Description: IGR201 Practical; OpenGL and Shaders (DO NOT distribute!)
//
// Copyright 2020-2023 Kiwon Um
//
// The copyright to the computer program(s) herein is the property of Kiwon Um,
// Telecom Paris, France. The program(s) may be used and/or copied only with
// the written permission of Kiwon Um or in accordance with the terms and
// conditions stipulated in the agreement/contract under which the program(s)
// have been supplied.
// ----------------------------------------------------------------------------
#ifndef _MY_OPENGL_IS_33_
#define _MY_OPENGL_IS_33_
#endif

#define _USE_MATH_DEFINES

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "mesh.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// constants
const static float kSizeSun = 2;
const static float kSizeEarth = 0.5;
const static float kSizeMoon = 0.25;
const static float kRadOrbitEarth = 10;
const static float kRadOrbitMoon = 2;

// Model transformation matrices
glm::mat4 g_sun, g_earth, g_moon, g_line;


//mesh
std::shared_ptr<Mesh> g_mesh_sun = nullptr;
std::shared_ptr<Mesh> g_mesh_earth = nullptr;
std::shared_ptr<Mesh> g_mesh_moon = nullptr;
std::shared_ptr<Mesh> g_line_test = nullptr;
std::vector<glm::vec3> test_line{glm::vec3(1.f,0.f,0.f), glm::vec3(2.f,0.f,0.f),glm::vec3(7.f,0.f,0.f), glm::vec3(9.f,3.f,0.f), glm::vec3(12.f,3.f,0.f)};

// Window parameters
GLFWwindow *g_window = nullptr;

// GPU objects
GLuint g_program = 0; // A GPU program contains at least a vertex shader and a fragment shader

//texture ids
GLuint g_moonTexID ;
GLuint g_sunTexID;
GLuint g_earthTexID;

// Basic camera model
class Camera {
public:
  inline float getFov() const { return m_fov; }
  inline void setFoV(const float f) { m_fov = f; }
  inline float getAspectRatio() const { return m_aspectRatio; }
  inline void setAspectRatio(const float a) { m_aspectRatio = a; }
  inline float getNear() const { return m_near; }
  inline void setNear(const float n) { m_near = n; }
  inline float getFar() const { return m_far; }
  inline void setFar(const float n) { m_far = n; }
  inline void setPosition(const glm::vec3 &p) {m_pos = p;}
  float yaw = 0.0f; //angle with around y axis
  float pitch = 0.0f; //angle around x axis
  inline glm::vec3 getPosition() { return m_pos; }

  inline glm::mat4 computeViewMatrix() const {
    return glm::lookAt(m_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  }

  // Returns the projection matrix stemming from the camera intrinsic parameter.
  inline glm::mat4 computeProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
  }

private:
  glm::vec3 m_pos = glm::vec3(0, 0, 0);
  float m_fov = 45.f;        // Field of view, in degrees
  float m_aspectRatio = 1.f; // Ratio between the width and the height of the image
  float m_near = 0.1f; // Distance before which geometry is excluded from the rasterization process
  float m_far = 10.f; // Distance after which the geometry is excluded from the rasterization process

};
Camera g_camera;

GLuint loadTextureFromFileToGPU(const std::string &filename) {
  int width, height, numComponents;
  unsigned char *data = stbi_load(
    filename.c_str(),
    &width, &height,
    &numComponents, // 1 for a 8 bit grey-scale image, 3 for 24bits RGB image, 4 for 32bits RGBA image
    0);
  GLuint texID; //OpenGL texture identifier
  
  glGenTextures(1, &texID); 
  glBindTexture(GL_TEXTURE_2D, texID);  
  

  // Setup the texture filtering option and repeat mode; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // Fill the GPU texture with the data stored in the CPU image
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  // Free useless CPU memory
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
  

  return texID; 

  
}

// Executed each time the window is resized. Adjust the aspect ratio and the rendering viewport to the current window.
void windowSizeCallback(GLFWwindow* window, int width, int height) {
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));
  glViewport(0, 0, (GLint)width, (GLint)height); // Dimension of the rendering region in the window
}
void scrollCallBack(GLFWwindow *window, double xoffset, double yoffset){
  glm::vec4 transform(0.f,0.f, 1.f,1.f);
  float distance = glm::length(g_camera.getPosition());
  g_camera.yaw+=0.1*xoffset;
  g_camera.pitch +=( 0.1*yoffset);
  g_camera.pitch = std::min(g_camera.pitch, 0.5f*3.14f);
  g_camera.pitch = std::max(g_camera.pitch, -0.5f*3.14f);
  transform=glm::rotate(glm::mat4(1.f), g_camera.pitch, glm::vec3(1.f,0.f,0.f))*transform;
  transform=glm::rotate(glm::mat4(1.f), g_camera.yaw, glm::vec3(0.f,1.f,0.f))*transform;

  g_camera.setPosition(glm::vec3(distance*transform));
}
// Executed each time a key is entered.
void mouseCallBack(GLFWwindow* window, int key, int scancode,)
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS && key == GLFW_KEY_W) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if(action == GLFW_PRESS && key == GLFW_KEY_F) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else if(action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
    glfwSetWindowShouldClose(window, true); // Closes the application if the escape key is pressed
  }
}

void errorCallback(int error, const char *desc) {
  std::cout <<  "Error " << error << ": " << desc << std::endl;
}

void initGLFW() {
  glfwSetErrorCallback(errorCallback);

  // Initialize GLFW, the library responsible for window management
  if(!glfwInit()) {
    std::cerr << "ERROR: Failed to init GLFW" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Before creating the window, set some option flags
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  // Create the window
  g_window = glfwCreateWindow(
    1024, 768,
    "Interactive 3D Applications (OpenGL) - Simple Solar System",
    nullptr, nullptr);
  if(!g_window) {
    std::cerr << "ERROR: Failed to open window" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  // Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
  glfwMakeContextCurrent(g_window);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetKeyCallback(g_window, keyCallback);
  glfwSetScrollCallback(g_window, scrollCallBack);
}

void initOpenGL() {
  // Load extensions for modern OpenGL
  if(!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glCullFace(GL_BACK); // Specifies the faces to cull (here the ones pointing away from the camera)
  glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
  glDepthFunc(GL_LESS);   // Specify the depth test for the z-buffer
  glEnable(GL_DEPTH_TEST);      // Enable the z-buffer test in the rasterization
  glClearColor(1.f, 1.f, 1.f, 1.f); // specify the background color, used any time the framebuffer is cleared
}

// Loads the content of an ASCII file in a standard C++ string
std::string file2String(const std::string &filename) {
  std::ifstream t(filename.c_str());
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

// Loads and compile a shader, before attaching it to a program
void loadShader(GLuint program, GLenum type, const std::string &shaderFilename) {
  GLuint shader = glCreateShader(type); // Create the shader, e.g., a vertex shader to be applied to every single vertex of a mesh
  std::string shaderSourceString = file2String(shaderFilename); // Loads the shader source from a file to a C++ string
  const GLchar *shaderSource = (const GLchar *)shaderSourceString.c_str(); // Interface the C++ string through a C pointer
  glShaderSource(shader, 1, &shaderSource, NULL); // load the vertex shader code
  glCompileShader(shader);
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR in compiling " << shaderFilename << "\n\t" << infoLog << std::endl;
  }
  glAttachShader(program, shader);
  glDeleteShader(shader);
}

void initGPUprogram() {
 g_program = glCreateProgram(); // Create a GPU program, i.e., two central shaders of the graphics pipeline
  loadShader(g_program, GL_VERTEX_SHADER, "vertexShader.glsl");
  loadShader(g_program, GL_FRAGMENT_SHADER, "fragmentShader.glsl");

  glLinkProgram(g_program); // The main GPU program is ready to be handle streams of polygons
  
  g_earthTexID = loadTextureFromFileToGPU("media/earth.jpg");

  g_moonTexID = loadTextureFromFileToGPU("media/moon.jpg");
  
  g_sunTexID = loadTextureFromFileToGPU("media/sun.jpg");

  glUseProgram(g_program);

}

void render(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.

  const glm::mat4 viewMatrix = g_camera.computeViewMatrix();
  const glm::mat4 projMatrix = g_camera.computeProjectionMatrix();
  const glm::vec3 camPosition = g_camera.getPosition();

  glUniform3f(glGetUniformLocation(g_program, "camPos"), camPosition[0], camPosition[1], camPosition[2]);

  glUniformMatrix4fv(glGetUniformLocation(g_program, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix)); // compute the view matrix of the camera and pass it to the GPU program
  glUniformMatrix4fv(glGetUniformLocation(g_program, "projMat"), 1, GL_FALSE, glm::value_ptr(projMatrix)); // compute the projection matrix of the camera and pass it to the GPU program
  
  // Bind the Earth's texture to texture unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_earthTexID);
  glUniform1i(glGetUniformLocation(g_program, "ourTexture"), 0);  // Use texture unit 0
  g_mesh_earth->render(g_program);

  // Bind the Moon's texture to texture unit 1
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_moonTexID);
  glUniform1i(glGetUniformLocation(g_program, "ourTexture"), 1);  // Use texture unit 1
  g_mesh_moon->render(g_program);

  // Bind the Sun's texture to texture unit 2
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, g_sunTexID);
  glUniform1i(glGetUniformLocation(g_program, "ourTexture"), 2);  // Use texture unit 2
  g_mesh_sun->render(g_program);

  
  g_line_test->render(g_program);

}

void initCamera() {
  int width, height;
  glfwGetWindowSize(g_window, &width, &height);
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));

  g_camera.setPosition(glm::vec3(0.0, 0.0, 40.0));
  g_camera.setNear(0.1);
  g_camera.setFar(80.1);
}

void init() {

  

  initGLFW();
  initOpenGL();
  initGPUprogram();
  initCamera();

  g_mesh_sun = Mesh::genSphere();
  g_mesh_earth = Mesh::genSphere();
  g_mesh_moon = Mesh::genSphere();
  g_line_test = Mesh::genLine(test_line,g_camera.getPosition());
  g_line_test ->setModelMatrix(glm::mat4(1.0f));

  g_mesh_sun->isSun();

  g_mesh_sun->init();
  g_mesh_moon -> init();
  g_mesh_earth -> init();
  g_line_test -> init();
  
}

void clear() {
  glDeleteProgram(g_program);

  glfwDestroyWindow(g_window);
  glfwTerminate();
}


// Update any accessible variable based on the current time
void update(const float currentTimeInSec) {
  glm::mat4 sunModelMatrix = glm::mat4(1.0f);
  sunModelMatrix = glm::translate(sunModelMatrix,glm::vec3(0.0f,0.0f,0.0f));
  //set the distance from center
  sunModelMatrix = glm::scale(sunModelMatrix, glm::vec3(kSizeSun,kSizeSun,kSizeSun));
  //scale the size of the sun
  sunModelMatrix=glm::rotate(sunModelMatrix,currentTimeInSec/10,glm::vec3(0.0,1.0,0.0));
  //sun rotates on itself
  g_mesh_sun->setModelMatrix(sunModelMatrix);


  glm::mat4 earthModelMatrix = glm::mat4(1.0f);
  earthModelMatrix=glm::rotate(earthModelMatrix,currentTimeInSec/8,glm::vec3(0.0,1.0,.0));
  //earth rotates around center axis
  earthModelMatrix = glm::translate(earthModelMatrix,glm::vec3(kRadOrbitEarth,0.0f,0.0f));
  //earth away from sun by its orbit
  earthModelMatrix = glm::scale(earthModelMatrix, glm::vec3(kSizeEarth,kSizeEarth,kSizeEarth));
  //earth size
  earthModelMatrix= glm::rotate(earthModelMatrix,currentTimeInSec*5,glm::vec3(0.235,1.0,0.0));
  //earth rotates on itself at a certain angle
  g_mesh_earth->setModelMatrix(earthModelMatrix);

  
  glm::mat4 moonModelMatrix = glm::mat4(1.0f);
  moonModelMatrix=glm::rotate(moonModelMatrix,currentTimeInSec/8,glm::vec3(0.0,1.0,.0));
  moonModelMatrix = glm::translate(moonModelMatrix,glm::vec3(kRadOrbitEarth,0.0f,0.0f));
  //moon at the same position as earth
  moonModelMatrix=glm::rotate(moonModelMatrix,currentTimeInSec*3/2,glm::vec3(0.0,1.0,0.0));
  //moon rotates around the central axis of earth
  moonModelMatrix = glm::translate(moonModelMatrix,glm::vec3(kRadOrbitMoon,0.0f,0.0f));
  //moon at orbit distance from earth
  moonModelMatrix = glm::scale(moonModelMatrix, glm::vec3(kSizeMoon,kSizeMoon,kSizeMoon));
  //set moon size
  g_mesh_moon->setModelMatrix(moonModelMatrix); 

  

} 

int main(int argc, char ** argv) {
  init();
  while(!glfwWindowShouldClose(g_window)) {
    update(static_cast<float>(glfwGetTime()));
    render();
    glfwSwapBuffers(g_window);
    glfwPollEvents();

  }
  clear();
  return EXIT_SUCCESS;
}
