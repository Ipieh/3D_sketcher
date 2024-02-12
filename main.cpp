// ----------------------------------------------------------------------------
// main.cpp
//adapted from Kiwon UM's code provided from the first TP using openGL (solar system rendering)
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
#include "shape.hpp"
#include "camera.hpp"

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

bool g_mousePressed = false;
std::shared_ptr<Shape> g_current_shape = nullptr;

// Model transformation matrices
glm::mat4 g_line;


//mesh
std::shared_ptr<Mesh> g_line_test = nullptr;
std::vector<std::shared_ptr<Shape>> shapes = {};
std::vector<std::shared_ptr<Mesh>> shape_meshes ={};
std::vector<glm::vec3> test_line{glm::vec3(1.f,0.f,0.f), glm::vec3(2.f,0.f,0.f),glm::vec3(7.f,0.f,0.f), glm::vec3(9.f,3.f,0.f), glm::vec3(12.f,3.f,0.f)};
std::shared_ptr<Mesh> main_mesh= nullptr;
// Window parameters
GLFWwindow *g_window = nullptr;

// GPU objects
GLuint g_program = 0; // A GPU program contains at least a vertex shader and a fragment shader


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
  g_camera.setAspectRatio(width,height);
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
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS && key == GLFW_KEY_W) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if(action == GLFW_PRESS && key == GLFW_KEY_F) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else if(action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
    glfwSetWindowShouldClose(window, true); // Closes the application if the escape key is pressed
  }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        g_mousePressed = true;
        g_current_shape = std::make_shared<Shape>();
        std::cout<<"mouse clicked \n";
      }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        g_mousePressed=false;
        if ((g_current_shape->getPoints()).size()>1){
          g_current_shape -> close_shape();
          std::cout<<"mouse released \n";
          std::vector<glm::vec3> current_shape_points = g_current_shape->getPoints();
        
          //do not draw an empty line
          shapes.push_back(g_current_shape);
          std::shared_ptr<Mesh> line_Mesh = Mesh::genLine(current_shape_points,g_camera.getPosition());
          /*for(glm::vec3 i:current_shape_points){
            std::cout<<i.x<<" , "<<i.y<<" , "<<i.z<<"\n";
          }*/
          line_Mesh->init();
          shape_meshes.push_back(line_Mesh);
          }
          if (shape_meshes.size()==1){
            main_mesh = Mesh::genMeshConstraint(g_current_shape, g_camera);
            main_mesh->init();
          }
          else{
            //TODO
            //main_mesh->remesh(shape_meshes);
          }

      }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
  if (g_mousePressed){
    g_current_shape->addPoint(xpos,ypos,g_camera);

    if (shape_meshes.size()>1){
      //TODO
      //main_mesh->interpolate_new_constraints();
    }
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
  glfwSetMouseButtonCallback(g_window, mouse_button_callback);
  glfwSetCursorPosCallback(g_window, cursor_position_callback);

}

void initOpenGL() {
  // Load extensions for modern OpenGL
  if(!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  //glCullFace(GL_BACK); // Specifies the faces to cull (here the ones pointing away from the camera)
  //glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
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


  g_line_test->render(g_program);

  for (int i =0 ; i<shape_meshes.size(); i++){
    shape_meshes[i]->render(g_program);
  };
  if (shape_meshes.size()>0){
    main_mesh->render(g_program);
  }
}

void initCamera() {
  int width, height;
  glfwGetWindowSize(g_window, &width, &height);
  g_camera.setAspectRatio(width,height);

  g_camera.setPosition(glm::vec3(0.0, 0.0, 40.0));
  g_camera.setNear(0.1);
  g_camera.setFar(80.1);
}

void init() {

  

  initGLFW();
  initOpenGL();
  initGPUprogram();
  initCamera();

  g_line_test = Mesh::genLine(test_line,g_camera.getPosition());
  g_line_test ->setModelMatrix(glm::mat4(1.0f));

  g_line_test->init();
}

void clear() {
  glDeleteProgram(g_program);

  glfwDestroyWindow(g_window);
  glfwTerminate();
}


// Update any accessible variable based on the current time
void update(const float currentTimeInSec) {


} 

int main(int argc, char ** argv) {
  init();
  std::cout<<"init ok main main \n";
  while(!glfwWindowShouldClose(g_window)) {
    update(static_cast<float>(glfwGetTime()));
    render();
    glfwSwapBuffers(g_window);
    glfwPollEvents();

  }
  clear();
  return EXIT_SUCCESS;
}
