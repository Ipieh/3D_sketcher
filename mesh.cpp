#include "mesh.hpp"
#include <cstdlib>
#include <iostream>


void Mesh::initCPUGeometry() {
  //done in the generate certain shapes
}


void Mesh::initGPUGeometry() {
  // Create a single handle, vertex array object that contains attributes,
  // vertex buffer objects (e.g., vertex's position, normal, and color)

  glGenVertexArrays(1, &m_vao); // If your system doesn't support OpenGL 4.5, you should use this instead of glCreateVertexArrays.

  glBindVertexArray(m_vao);

  // Generate a GPU buffer to store the positions of the vertices
  size_t vertexBufferSize = sizeof(float)*m_vertexPositions.size(); // Gather the size of the buffer from the CPU-side vector

  glGenBuffers(1, & mposVbo);
  glBindBuffer(GL_ARRAY_BUFFER, mposVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertexPositions.data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);


//Generate a GPU buffer to store the color values of the vertices

  glGenBuffers(1, &m_colVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_colVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertexColors.data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(1);

//Generate a GPU buffer to store the normal values of the surfaces
  glGenBuffers(1, &m_normalVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_normalVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertexNormals.data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(2);

  vertexBufferSize = sizeof(float)*m_vertexTexCoords.size();
  //Generate a GPU buffer to store the texture vertices
  glGenBuffers(1, &m_texCoordVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertexTexCoords.data(), GL_DYNAMIC_READ);
  
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(3);

  // Same for an index buffer object that stores the list of indices of the
  // triangles forming the mesh
  size_t indexBufferSize = sizeof(unsigned int)*m_triangleIndices.size();
  

  glGenBuffers(1, &m_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, m_triangleIndices.data(), GL_DYNAMIC_READ);


  glBindVertexArray(0); // deactivate the VAO for now, will be activated again when rendering
}   

void Mesh::init(){
  initCPUGeometry();
  initGPUGeometry();
}

void Mesh::render(GLuint g_program){
  // The main rendering call

  
  glUniform1f(glGetUniformLocation(g_program, "shininess"), m_shininess);
  glUniform3f(glGetUniformLocation(g_program, "objColor"), m_vertexColors[0], m_vertexColors[1], m_vertexColors[2]);
  glUniformMatrix4fv(glGetUniformLocation(g_program, "model_matrix"),1, GL_FALSE, glm::value_ptr(model_matrix));
  glUniform1i(glGetUniformLocation(g_program, "isSun"),isSourceOfLight);
  
  glBindVertexArray(m_vao);     // activate the VAO storing geometry data
  glDrawElements(GL_TRIANGLES, m_triangleIndices.size(), GL_UNSIGNED_INT, 0);
   // Call for rendering: stream the current GPU geometry through the current GPU program
}

void Mesh::setVertexPositions(std::vector<float> vertexPositions){
  m_vertexPositions=vertexPositions;
}

void Mesh::setTriangleIndices(std::vector<unsigned int> triangleIndices){
  m_triangleIndices = triangleIndices;
}

void Mesh::setVertexTexCoords(std::vector<float> vertexTexCoords)
{
  m_vertexTexCoords = vertexTexCoords;
}

void Mesh::isSun(){
  isSourceOfLight = true;
}

void Mesh::setShininess(float shininess){
  m_shininess= shininess;
}

void Mesh::setColors(std::vector<float> sphereColors)
{
  m_vertexColors = sphereColors;
}

void Mesh::setNormals(std::vector<float> sphereNormals)
{
  m_vertexNormals = sphereNormals;
}

void Mesh::setModelMatrix(glm::mat4 new_model_matrix)
{
  model_matrix = new_model_matrix;
}

std::shared_ptr<Mesh> Mesh::genSphere(const size_t resolution){
  std::shared_ptr<Mesh> sphereMesh = std::make_shared<Mesh>();

  std::vector<float> sphereVertices;
  std::vector<unsigned int> sphereIndices;
  std::vector<float> sphereColors;
  std::vector<float> sphereNormals;
  std::vector<float> vertexTexCoords;

  const float radius = 1.0f;
  const float pi = 3.14159265358979323846264338327950288;
  const float twoPi = 2.0f * pi;
  const float deltaPhi = twoPi / resolution;
  const float deltaTheta = twoPi / resolution;
  float s,t;

  for (size_t i = 0; i <= resolution; ++i) {
    float phi = -0.5f * pi + i * deltaPhi;
    for (size_t j = 0; j <= resolution; ++j) {
      float theta = j * deltaTheta;

      float x = radius * std::sin(phi) * std::cos(theta);
      float y = radius * std::sin(phi) * std::sin(theta);
      float z = radius * std::cos(phi);

      sphereVertices.push_back(x);
      sphereVertices.push_back(y);
      sphereVertices.push_back(z);

      //calculate the normals
      glm::vec3 normal(x, y, z);
      normal = glm::normalize(normal);

      sphereNormals.push_back(normal.x);
      sphereNormals.push_back(normal.y);
      sphereNormals.push_back(normal.z);

      //set the colors for each vertex
      sphereColors.push_back(0.25f);  // Red component
      sphereColors.push_back(0.75f);  // Green component
      sphereColors.push_back(1.0f); // Blue component
    }
  }

  // Generate indices for triangles
  for (size_t i = 0; i < resolution; ++i) {
    for (size_t j = 0; j < resolution; ++j) {
      // Compute indices for the four vertices of a quad
      unsigned int v0 = i * (resolution + 1) + j;
      unsigned int v1 = v0 + 1;
      unsigned int v2 = v0 + resolution + 1;
      unsigned int v3 = v2 + 1;
    
      // Create two triangles from the quad
      sphereIndices.push_back(v0);
      sphereIndices.push_back(v2);
      sphereIndices.push_back(v1);

      sphereIndices.push_back(v1);
      sphereIndices.push_back(v2);
      sphereIndices.push_back(v3);

    }
  }

  for(int i = 0 ; i<sphereVertices.size(); i+=3) {
    float x = sphereVertices[i];
    float y = sphereVertices[i+1];
    float z = sphereVertices[i+2];

    float u = 0.5f + atan2(z,x)/(2*M_PI);
    float v = 0.5f - asin(y)/M_PI;

    vertexTexCoords.push_back(u);
    vertexTexCoords.push_back(v);
  }

  sphereMesh->setVertexPositions(sphereVertices);
  sphereMesh->setTriangleIndices(sphereIndices);
  sphereMesh->setColors(sphereColors);
  sphereMesh ->setNormals(sphereNormals);
  sphereMesh-> setVertexTexCoords(vertexTexCoords);
  GLuint m_texCoordVbo = 0;

    return sphereMesh;
}; 

std::shared_ptr<Mesh> Mesh::genLine(std::vector<glm::vec3> line, glm::vec3 normalVec){
  std::shared_ptr<Mesh> lineMesh = std::make_shared<Mesh>();
  std::vector<float> lineVertices;
  //contains the coordinates of the vertices
  std::vector<unsigned int> lineIndices;
  //indices of triangles
  std::vector<float> lineColors;
  std::vector<float> lineNormals;
  std::vector<float> vertexTexCoords;
  glm::vec3 normal = glm::normalize(normalVec);

  float thickness = 0.5f;

  for(int i = 1; i < line.size(); i++){
    //plane defined by points in line, and the normal vector
    glm::vec3 lineDirection = glm::vec3(line[i-1].x-line[i].x,line[i-1].y-line[i].y,line[i-1].z-line[i].z);
    glm::vec3 thicknessDirection = glm::normalize(glm::cross(lineDirection,normalVec));
    if (i==1){
      glm::vec3 new_vertex = line[0]-thickness*thicknessDirection;
      lineVertices.push_back(new_vertex.x);
      lineVertices.push_back(new_vertex.y);
      lineVertices.push_back(new_vertex.z);

      glm::vec3 new_vertex_plus = line[0]+thickness*thicknessDirection;
      lineVertices.push_back(new_vertex_plus.x);
      lineVertices.push_back(new_vertex_plus.y);
      lineVertices.push_back(new_vertex_plus.z);

      glm::vec3 new_vertex_normal = line[0]+thickness*normal;
      lineVertices.push_back(new_vertex_plus.x);
      lineVertices.push_back(new_vertex_plus.y);
      lineVertices.push_back(new_vertex_plus.z);


      //calculate the normals
      glm::vec3 normal(new_vertex.x, new_vertex.y, new_vertex.z);
      normal = glm::normalize(normal);

      glm::vec3 normal_plus(new_vertex_plus.x, new_vertex_plus.y, new_vertex_plus.z);
      normal_plus = glm::normalize(normal);
 
      lineNormals.push_back(normal.x);
      lineNormals.push_back(normal.y);
      lineNormals.push_back(normal.z);

      lineNormals.push_back(normal_plus.x);
      lineNormals.push_back(normal_plus.y);
      lineNormals.push_back(normal_plus.z);

      //set the colors for each vertex
      lineColors.push_back(0.0f);  // Red component
      lineColors.push_back(0.0f);  // Green component
      lineColors.push_back(1.0f); // Blue component

      lineColors.push_back(0.0f);
      lineColors.push_back(0.0f);  
      lineColors.push_back(1.0f);
    }

    glm::vec3 top_vertex = line[i]-thickness*thicknessDirection;
    lineVertices.push_back(top_vertex.x);
    lineVertices.push_back(top_vertex.y);
    lineVertices.push_back(top_vertex.z);

    glm::vec3 bottom_vertex = line[i]+thickness*thicknessDirection;
    lineVertices.push_back(bottom_vertex.x);
    lineVertices.push_back(bottom_vertex.y);
    lineVertices.push_back(bottom_vertex.z);

    glm::vec3 normal_vertex = line[i]+thickness*normal;
      lineVertices.push_back(normal_vertex.x);
      lineVertices.push_back(normal_vertex.y);
      lineVertices.push_back(normal_vertex.z);

    //calculate the normals - for now this is only a placeholder
      glm::vec3 normal_top(top_vertex.x, top_vertex.y, top_vertex.z);
      normal_top = glm::normalize(normal);

      glm::vec3 normal_bottom(bottom_vertex.x, bottom_vertex.y, bottom_vertex.z);
      normal_bottom = glm::normalize(normal);

      glm::vec3 normal_normal(normal_vertex.x, normal_vertex.y, normal_vertex.z);
      normal_normal = glm::normalize(normal);
 
      lineNormals.push_back(normal_top.x);
      lineNormals.push_back(normal_top.y);
      lineNormals.push_back(normal_top.z);

      lineNormals.push_back(normal_bottom.x);
      lineNormals.push_back(normal_bottom.y);
      lineNormals.push_back(normal_bottom.z);

      lineNormals.push_back(normal_normal.x);
      lineNormals.push_back(normal_normal.y);
      lineNormals.push_back(normal_normal.z);

      //set the colors for each vertex
      lineColors.push_back(0.0f);
      lineColors.push_back(0.0f);
      lineColors.push_back(1.0f);

      lineColors.push_back(0.0f);
      lineColors.push_back(0.0f);  
      lineColors.push_back(1.0f);

      lineColors.push_back(0.0f);
      lineColors.push_back(0.0f);  
      lineColors.push_back(1.0f);
  }

  // Generate indices for triangles
  for (int i = 0; i < line.size()-1 ; i++){
    int v0 = 3*i;
    int v1 = 3*i+1;
    int v2 = 3*i+2;
    int v3 = 3*i+3;
    int v4 = 3*i+4;
    int v5 = 3*i+5;

    lineIndices.push_back(v0);
    lineIndices.push_back(v1);
    lineIndices.push_back(v4);

    lineIndices.push_back(v0);
    lineIndices.push_back(v4);
    lineIndices.push_back(v3);

    lineIndices.push_back(v0);
    lineIndices.push_back(v3);
    lineIndices.push_back(v5);

    lineIndices.push_back(v0);
    lineIndices.push_back(v5);
    lineIndices.push_back(v2);
    
    lineIndices.push_back(v1);
    lineIndices.push_back(v5);
    lineIndices.push_back(v4);

    lineIndices.push_back(v1);
    lineIndices.push_back(v2);
    lineIndices.push_back(v5);
  }
  //Generate texture coordinates
  for(int i = 0 ; i<lineVertices.size(); i+=3) {
    float x = lineVertices[i];
    float y = lineVertices[i+1];
    float z = lineVertices[i+2];

    float u = (x + 1.0f) / 2.0f;
    float v = (y + 1.0f) / 2.0f;
    

    vertexTexCoords.push_back(u);
    vertexTexCoords.push_back(v);
  }
  
  std::cout<<"\n";
  lineMesh->setVertexPositions(lineVertices);
  lineMesh->setTriangleIndices(lineIndices);
  lineMesh->setColors(lineColors);
  lineMesh ->setNormals(lineNormals);
  lineMesh-> setVertexTexCoords(vertexTexCoords);
  GLuint m_texCoordVbo = 0;

  return lineMesh;
}
         
Mesh::~Mesh(){
  glDeleteBuffers(1,&mposVbo);
  glDeleteBuffers(1,&m_colVbo);
  glDeleteBuffers(1,&m_normalVbo);
  glDeleteBuffers(1,&m_texCoordVbo);
  
}

