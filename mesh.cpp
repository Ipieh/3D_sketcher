#include "mesh.hpp"
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <cmath>

//TODO delete stuff that had to do with the solar system
void Mesh::initCPUGeometry() {
  //done in the generate certain shapes
}


void Mesh::initGPUGeometry() {
  // Create a single handle, vertex array object that contains attributes,
  // vertex buffer objects (e.g., vertex's position, normal, and color)

  glGenVertexArrays(1, &m_vao); // If your system doesn't support OpenGL 4.5, you should use this instead of glCreateVertexArrays.

  glBindVertexArray(m_vao);

  // Generate a GPU buffer to store the positions of the vertices
  size_t vertexBufferSize = sizeof(float)*m_vertexPositions->size(); // Gather the size of the buffer from the CPU-side vector

  glGenBuffers(1, & mposVbo);
  glBindBuffer(GL_ARRAY_BUFFER, mposVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertexPositions->data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);


//Generate a GPU buffer to store the color values of the vertices

  glGenBuffers(1, &m_colVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_colVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertexColors->data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(1);

//Generate a GPU buffer to store the normal values of the surfaces
  glGenBuffers(1, &m_normalVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_normalVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertexNormals->data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(2);

  vertexBufferSize = sizeof(float)*m_vertexTexCoords->size();
  //Generate a GPU buffer to store the texture vertices
  glGenBuffers(1, &m_texCoordVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertexTexCoords->data(), GL_DYNAMIC_READ);
  
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(3);

  // Same for an index buffer object that stores the list of indices of the
  // triangles forming the mesh
  size_t indexBufferSize = sizeof(unsigned int)*(m_triangleIndices->size());
  

  glGenBuffers(1, &m_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, m_triangleIndices->data(), GL_DYNAMIC_READ);


  glBindVertexArray(0); // deactivate the VAO for now, will be activated again when rendering
}   

void Mesh::init(){
  initCPUGeometry();
  initGPUGeometry();
}

void Mesh::render(GLuint g_program){
  // The main rendering call

  
  glUniform1f(glGetUniformLocation(g_program, "shininess"), m_shininess);
  glUniform3f(glGetUniformLocation(g_program, "objColor"), (*m_vertexColors)[0], (*m_vertexColors)[1], (*m_vertexColors)[2]);
  glUniformMatrix4fv(glGetUniformLocation(g_program, "model_matrix"),1, GL_FALSE, glm::value_ptr(model_matrix));
  glUniform1i(glGetUniformLocation(g_program, "isSun"),isSourceOfLight);
  
  glBindVertexArray(m_vao);     // activate the VAO storing geometry data
  glDrawElements(GL_TRIANGLES, m_triangleIndices->size(), GL_UNSIGNED_INT, 0);
   // Call for rendering: stream the current GPU geometry through the current GPU program
}

void Mesh::setVertexPositions(std::vector<float> vertexPositions){
  m_vertexPositions=std::make_shared<std::vector<float>>(vertexPositions.begin(), vertexPositions.end());
}

void Mesh::setTriangleIndices(std::vector<unsigned int> triangleIndices){
  m_triangleIndices = std::make_shared<std::vector<unsigned int>>(triangleIndices.begin(),triangleIndices.end());
}

void Mesh::setVertexTexCoords(std::vector<float> vertexTexCoords)
{
  m_vertexTexCoords = std::make_shared<std::vector<float>>(vertexTexCoords.begin(),vertexTexCoords.end());
}

void Mesh::isSun(){
  isSourceOfLight = true;
} //TODO remove this

void Mesh::interpolate_new_constraints(Shape current_shape){
  //TODO
}

void Mesh::setShininess(float shininess){
  m_shininess= shininess;
}

void Mesh::setColors(std::vector<float> sphereColors)
{
  m_vertexColors = std::make_shared<std::vector<float>>(sphereColors.begin(),sphereColors.end());
}

void Mesh::setNormals(std::vector<float> sphereNormals)
{
  m_vertexNormals = std::make_shared<std::vector<float>>(sphereNormals.begin(),sphereNormals.end());
}

void Mesh::setModelMatrix(glm::mat4 new_model_matrix)
{
  model_matrix = new_model_matrix;
}

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

  float thickness = 0.1f;

  for(int i = 1; i < line.size(); i++){
    //plane defined by points in line, and the normal vector
    glm::vec3 lineDirection = glm::vec3(line[i-1].x-line[i].x,line[i-1].y-line[i].y,line[i-1].z-line[i].z);
    glm::vec3 thicknessDirection = glm::normalize(glm::cross(lineDirection,normalVec));
    if (i==1){
      glm::vec3 new_vertex = line[0]-thickness*thicknessDirection;
      lineVertices.push_back(new_vertex.x);
      lineVertices.push_back(new_vertex.y);
      lineVertices.push_back(new_vertex.z);
      //vertex on the left of drawn point

      glm::vec3 new_vertex_plus = line[0]+thickness*thicknessDirection;
      lineVertices.push_back(new_vertex_plus.x);
      lineVertices.push_back(new_vertex_plus.y);
      lineVertices.push_back(new_vertex_plus.z);
      //vertex on the right of drawn point

      glm::vec3 new_vertex_normal = line[0]+thickness*normal;
      lineVertices.push_back(new_vertex_normal.x);
      lineVertices.push_back(new_vertex_normal.y);
      lineVertices.push_back(new_vertex_normal.z);
      //vertex above drawn point

      //calculate the normals
      glm::vec3 normal(new_vertex.x, new_vertex.y, new_vertex.z);
      normal = glm::normalize(normal);

      glm::vec3 normal_plus(new_vertex_plus.x, new_vertex_plus.y, new_vertex_plus.z);
      normal_plus = glm::normalize(normal);

      glm::vec3 normal_normal(new_vertex_normal.x, new_vertex_normal.y, new_vertex_normal.z);
      normal_plus = glm::normalize(normal);
 
      lineNormals.push_back(normal.x);
      lineNormals.push_back(normal.y);
      lineNormals.push_back(normal.z);

      lineNormals.push_back(normal_plus.x);
      lineNormals.push_back(normal_plus.y);
      lineNormals.push_back(normal_plus.z);

      lineNormals.push_back(normal_normal.x);
      lineNormals.push_back(normal_normal.y);
      lineNormals.push_back(normal_normal.z);

      //set the colors for each vertex
      lineColors.push_back(0.0f);  // Red component
      lineColors.push_back(0.0f);  // Green component
      lineColors.push_back(1.0f); // Blue component

      lineColors.push_back(0.0f);
      lineColors.push_back(0.0f);  
      lineColors.push_back(1.0f);

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
  
  lineMesh->setVertexPositions(lineVertices);
  lineMesh->setTriangleIndices(lineIndices);
  lineMesh->setColors(lineColors);
  lineMesh ->setNormals(lineNormals);
  lineMesh-> setVertexTexCoords(vertexTexCoords);
  GLuint m_texCoordVbo = 0;
  return lineMesh;
}

std::shared_ptr<Mesh> Mesh::genMeshConstraint(std::shared_ptr<Shape> constraint, Camera camera){
  
  //TODO maybe should generate only half at first to see if it works
  std::vector<glm::vec3>constraint_points = constraint->getPoints(); 

  //Assign random triangles
  std::shared_ptr<Mesh> mainMesh = std::make_shared<Mesh>();
  std::vector<float> meshVertices;
  //contains the coordinates of the vertices
  std::vector<unsigned int> triangleIndices;
  //indices of triangles
  std::vector<float> meshColors;
  std::vector<float> meshNormals;
  std::vector<float> vertexTexCoords;

  for(int i = 1; i < constraint_points.size(); i++){
    glm::vec3 constraint_point_i = constraint_points[i];
    //put every point in twice, once for each side of the plane, with slight offset
    glm::vec3 constraint_point_top = constraint_points[i];
    meshVertices.push_back(constraint_point_top.x);
    meshVertices.push_back(constraint_point_top.y);
    meshVertices.push_back(constraint_point_top.z);
    //even points on top
    meshVertices.push_back(constraint_point_i.x);
    meshVertices.push_back(constraint_point_i.y);
    meshVertices.push_back(constraint_point_i.z);
    //odd point on bottom
    
  }
  // Generate indices for triangles
  for (int i = 1; i < constraint_points.size()-2 ; i++){
    int v0 = 0;
    int v2 = 2*i;
    int v4 = 2*(i+1);
    int v1 = 1;
    int v3 = 2*i+1;
    int v5 = 2*(i+1)+1;
    //top mesh
    triangleIndices.push_back(v0);
    triangleIndices.push_back(v4);
    triangleIndices.push_back(v2);
    //bottom mesh
    triangleIndices.push_back(v1);
    triangleIndices.push_back(v3);
    triangleIndices.push_back(v5);
    
  }
  

  mainMesh->setVertexPositions(meshVertices);
  mainMesh->setTriangleIndices(triangleIndices);
  mainMesh->setColors(meshColors);
  mainMesh ->setNormals(meshNormals);
  mainMesh->compute_colors(0.5f,0.2f,0.7f);
  mainMesh->compute_normals();
  mainMesh->compute_texCoords();
  
  
  //remesh so that the surface is made of equilateral triangles
  mainMesh->remesh_isotropic();

  std::cout<<"number of points in  colors"<<(mainMesh->m_vertexColors->size()/3)<<"\n";
  std::cout<<"number of points in positions"<<(mainMesh->m_vertexPositions->size()/3)<<"\n";
  std::cout<<"number of points in normals"<<(mainMesh->m_vertexNormals->size()/3)<<"\n";
  std::cout<<"number of points in texturecoords"<<(mainMesh->m_vertexTexCoords->size()/2)<<"\n";
  
  return mainMesh;

} 

void Mesh::remesh_isotropic(float L){
 
  float maxLength = 5/3*L;
  float minLength = 4/5*L;

  for (int i =0 ; i<3; i++){
    split_long_edges(maxLength);
    std::cout<<"split long edges ok \n";
    std::cout<<"m_vertexPositions size "<<m_vertexPositions->size()<<"\n";
  }
  //collapse_short_edges(minLength);
  //std::cout<<"collapse short edges ok \n";
  compute_normals();
  compute_colors(0.45f,0.59f,0.70f);
  compute_texCoords();
  std::cout<<"isometric compute other buffers ok \n";
  }
void Mesh::compute_normals(){

  
  std::vector<float> newNormals{};
  for (std::size_t i = 0; i < m_vertexPositions->size(); ++i){
    newNormals.push_back(0.f);
  }
   // Change the following code to compute a proper per-vertex normal

  for (unsigned int tIt = 0; tIt < m_triangleIndices->size(); tIt+=3) {
    //get the corners of a face
    glm::uvec3 t = glm::uvec3((*m_triangleIndices)[tIt  ],
                              (*m_triangleIndices)[tIt+1],
                              (*m_triangleIndices)[tIt+2]);

    glm::vec3 x0 = glm::vec3((*m_vertexPositions)[t[0]*3  ],
                             (*m_vertexPositions)[t[0]*3+1],
                             (*m_vertexPositions)[t[0]*3+2]);

    glm::vec3 x1 = glm::vec3((*m_vertexPositions)[t[1]*3  ],
                             (*m_vertexPositions)[t[1]*3+1],
                             (*m_vertexPositions)[t[1]*3+2]);

    glm::vec3 x2 = glm::vec3((*m_vertexPositions)[t[2]*3  ],
                             (*m_vertexPositions)[t[2]*3+1],
                             (*m_vertexPositions)[t[2]*3+2]);

    glm::vec3 n_t = glm::normalize(glm::cross(x1 - x0 , x2 - x0));

      newNormals[t[0]*3  ] += n_t.x;
      newNormals[t[0]*3+1] += n_t.y;
      newNormals[t[0]*3+2] += n_t.z;

      newNormals[t[1]*3  ] += n_t.x;
      newNormals[t[1]*3+1] += n_t.y;
      newNormals[t[1]*3+2] += n_t.z;

      newNormals[t[2]*3  ] += n_t.x;
      newNormals[t[2]*3+1] += n_t.y;
      newNormals[t[2]*3+2] += n_t.z;
    }
    std::cout<<" out of normal assignment \n";
    for (unsigned int nIt = 0; nIt < newNormals.size(); nIt+=3) {
      glm::vec3 n_t = glm::vec3(newNormals[nIt  ],
                                newNormals[nIt+1],
                                newNormals[nIt+2]);
      
      glm::normalize(n_t);
      newNormals[nIt] = n_t.x;
      newNormals[nIt] = n_t.y;
      newNormals[nIt] = n_t.z;
    }
    std::cout<<" out of normal normalization \n";
    
    setNormals(newNormals);
}
void Mesh::compute_colors(float r, float g, float b){
  std::cout<<"hello good sir \n";

  std::vector<float> colorsNew{};
  for (int i = 0 ; i<m_vertexPositions->size(); i+=3){
    colorsNew.push_back(r);
    colorsNew.push_back(g);
    colorsNew.push_back(b);
  }

  setColors(colorsNew);
  std::cout<<"colors assigned \n";
}
void Mesh::compute_texCoords(){

  std::vector<float> vertexTexCoords{};
  std::cout<<"entered tex Coords \n";
  //Generate texture coordinates
  for(int i = 0 ; i<m_vertexPositions->size(); i+=3) {

    float x = (*m_vertexPositions)[i];
    float y = (*m_vertexPositions)[i+1];
    float z = (*m_vertexPositions)[i+2];

    float u = (x + 1.0f) / 2.0f;
    float v = (y + 1.0f) / 2.0f;

    vertexTexCoords.push_back(u);
    vertexTexCoords.push_back(v);
  }
   setVertexTexCoords(vertexTexCoords);

   std::cout<<"m_vertexTexCoords assigned \n";
}

void Mesh::collapse_short_edges(float minLength){
  //will first run through all the edges (triangle per triangle)
  //if the edges are too long they will be split in half and this will be memorized
  //once all the edges have been visited, we will retrace the triangles
  //if an edge has been split, its triangles will now make two more triangles each

  struct Edge {
    unsigned int a , b;
    Edge( unsigned int c , unsigned int d ) : a( std::min<unsigned int>(c,d) ) , b( std::max<unsigned int>(c,d) ) {}
    bool operator < ( Edge const & o ) const {   return a < o.a  ||  (a == o.a && b < o.b);  }
    bool operator == ( Edge const & o ) const {   return a == o.a  &&  b == o.b;  }
    };

  std::set<unsigned int> verticesToDelete;
  std::set<unsigned int> trianglesToDelete;
  std::vector<unsigned int> newTriangles(m_triangleIndices->begin(),m_triangleIndices->end());
  std::set<Edge> edgesSet;
  //contains all the edges
  
  std::map< Edge , std::set< unsigned int > > trianglesOnEdge; 
  // finds the triangles on the old edges
  std::map<unsigned int , std::set<unsigned int> > trianglesWithCorner;
  //to find triangles linked to a certain corner

  //initialize the edges
  for (unsigned int i = 0; 3*i < m_triangleIndices->size(); i ++) {
    unsigned int v0 = (*m_triangleIndices)[3*i    ];
    unsigned int v1 = (*m_triangleIndices)[3*i + 1];
    unsigned int v2 = (*m_triangleIndices)[3*i + 2];

    Edge edge01(v0,v1); 
    Edge edge02(v0,v2); 
    Edge edge12(v2,v1); 
    //memorize edges
    edgesSet.insert(edge01);
    edgesSet.insert(edge02);
    edgesSet.insert(edge12);
    //memorize which triangles touch them
    trianglesOnEdge[edge01].insert(i);
    trianglesOnEdge[edge02].insert(i);
    trianglesOnEdge[edge12].insert(i);

    trianglesWithCorner[v0].insert(i);
    trianglesWithCorner[v1].insert(i);
    trianglesWithCorner[v2].insert(i);
    
    }
  std::cout<<"collapse number of edges : "<< edgesSet.size()<<"\n";
  //go through all the edges to collapse those that need it
  //delete the triangles on that edge
  //update those that used u so that they use v
  //remove u

  for (Edge e : edgesSet) {
    glm::vec3 a = glm::vec3((*m_vertexPositions)[3*e.a    ],
                            (*m_vertexPositions)[3*e.a + 1],
                            (*m_vertexPositions)[3*e.a + 2]);
    glm::vec3 b = glm::vec3((*m_vertexPositions)[3*e.b    ],
                            (*m_vertexPositions)[3*e.b + 1],
                            (*m_vertexPositions)[3*e.b + 2]);
    
    float length = glm::length(a-b);


    if (length < minLength) {
      // Collapse Edge
      a = 0.5f * (a+b);
      //make the points overlap in the middle
      (*m_vertexPositions)[3*e.a  ]=a.x;
      (*m_vertexPositions)[3*e.a+1]=a.y;
      (*m_vertexPositions)[3*e.a+2]=a.z;

      (*m_vertexPositions)[3*e.b  ]=a.x;
      (*m_vertexPositions)[3*e.b+1]=a.y;
      (*m_vertexPositions)[3*e.b+2]=a.z;
      //mark the vertex to delete
      verticesToDelete.insert(e.b);
      //mark the triangles to be deleted:
      std::set<unsigned int> triangles_on_e = trianglesOnEdge[e];
      for (unsigned int t : triangles_on_e){
          trianglesToDelete.insert(t);
      }
      //update the corners of triangles attached to b
      std::set<unsigned int> triangles_to_update = trianglesWithCorner[e.b];
      for (unsigned int t : triangles_to_update){
          if(newTriangles[3*t]==e.b){
            newTriangles[3*t]=e.a;
          }
          else if(newTriangles[3*t+1]==e.b){
            newTriangles[3*t+1]=e.a;
          }
          else if(newTriangles[3*t+2]==e.b){
            newTriangles[3*t+2]=e.a;
          }
      }
    }
  }
  std::vector<unsigned int> finalTriangles;
  //delete the obsolete triangles
  std::cout<<" collapse trianglesToDelete before loop size : " <<trianglesToDelete.size()<<" \n";
  for (unsigned int i = 0 ; i<newTriangles.size(); i+=3){
    if (trianglesToDelete.find(i)==trianglesToDelete.end()){
      //check if the triangles has to be deleted or not
      finalTriangles.push_back(newTriangles[i]);
      finalTriangles.push_back(newTriangles[i+1]);
      finalTriangles.push_back(newTriangles[i+2]);
    }
  }

  std::vector<float> finalVertices{};
  std::cout<<" collapse verticesToDelete before loop size : " <<verticesToDelete.size()<<" \n";
  std::cout<<" collapse newTriangles before loop size : " << newTriangles.size()<<" \n";
  std::cout<<" collapse m_vertexPositions before loop size : " << m_vertexPositions->size()<<" \n";
  //delete the obsolete triangles
  for (unsigned int i = 0 ; i<m_vertexPositions->size(); i+=3){

    if (verticesToDelete.find(i)==verticesToDelete.end()){
      //check if the triangles has to be deleted or not
      finalVertices.push_back(newTriangles[i  ]);
      finalVertices.push_back(newTriangles[i+1]);
      finalVertices.push_back(newTriangles[i+2]);
    }
  }
  //update m_vertexPositions and m_triangleIndices
  std::cout<<" collapse final Vertices size : " <<finalVertices.size()<<" \n";

  setVertexPositions(finalVertices);
  setTriangleIndices(finalTriangles);
}
void Mesh::split_long_edges(float maxLength){
  //will first run through all the edges (triangle per triangle)
  //if the edges are too long they will be split in half and this will be memorized
  //once all the edges have been visited, we will retrace the triangles
  //if an edge has been split, its triangles will now make two more triangles each

  struct Edge {
    unsigned int a , b;
    Edge( unsigned int c , unsigned int d ) : a( std::min<unsigned int>(c,d) ) , b( std::max<unsigned int>(c,d) ) {}
    bool operator < ( Edge const & o ) const {   return a < o.a  ||  (a == o.a && b < o.b);  }
    bool operator == ( Edge const & o ) const {   return a == o.a  &&  b == o.b;  }
    };

  std::vector<float> newVertexPositions{};
  std::vector<unsigned int> newTriangles{};
  std::set<Edge> edgesSet{};
  //contains all the edges
  
  std::map< Edge , std::set< unsigned int > > trianglesOnEdge; 
  // finds the triangles on the old edges
  std::map< Edge , unsigned int > newVertexOnEdge; 
  // this will be useful to find out whether we already split the edge

  //initialize the edges
  for (unsigned int i = 0; 3*i < m_triangleIndices->size(); i ++) {
    unsigned int v0 = (*m_triangleIndices)[3*i    ];
    unsigned int v1 = (*m_triangleIndices)[3*i + 1];
    unsigned int v2 = (*m_triangleIndices)[3*i + 2];
  
    Edge edge01(v0,v1); 
    Edge edge02(v0,v2); 
    Edge edge12(v2,v1); 
    //memorize edges
    edgesSet.insert(edge01);
    edgesSet.insert(edge02);
    edgesSet.insert(edge12);
    //memorize which triangles touch them
    trianglesOnEdge[edge01].insert(i);
    trianglesOnEdge[edge02].insert(i);
    trianglesOnEdge[edge12].insert(i);
  
    }

  //go through all the edges to divide those that need it
  int number_of_edges_added = 0;
  
  for (Edge e : edgesSet) {
    glm::vec3 a = glm::vec3((*m_vertexPositions)[e.a*3    ],
                            (*m_vertexPositions)[e.a*3 + 1],
                            (*m_vertexPositions)[e.a*3 + 2]);
    glm::vec3 b = glm::vec3((*m_vertexPositions)[e.b*3    ],
                            (*m_vertexPositions)[e.b*3 + 1],
                            (*m_vertexPositions)[e.b*3 + 2]);
    
    float length = glm::length(a-b);
    
    if (length > maxLength) {
      // Split edge
      glm::vec3 newPoint = 0.5f * (a+b);
      //update the points
      //debugging

      newVertexPositions.push_back(newPoint.x);
      newVertexPositions.push_back(newPoint.y);
      newVertexPositions.push_back(newPoint.z);
      //we're going to add the vertex to the end of the list
      newVertexOnEdge[e]=(number_of_edges_added+m_vertexPositions->size()/3);
      //update the counter of added edges
      number_of_edges_added++;
    }
  }

  //go through all triangles and subdivide them accordingly
  //going through triangles rather than edges allows to avoid putting the same triangle twice in the list
  for (int t = 0 ; t < m_triangleIndices->size() ; t+=3){
    //look at all three edges
    int t0 = (*m_triangleIndices)[t  ];
    int t1 = (*m_triangleIndices)[t+1];
    int t2 = (*m_triangleIndices)[t+2];
    
    Edge e1 = Edge(t0,t1);
    Edge e2 = Edge(t2,t1);
    Edge e3 = Edge(t0,t2);

    if (newVertexOnEdge.find(e1)!=newVertexOnEdge.end()){
      
      int new_vertex = newVertexOnEdge[e1];
      std::cout<<new_vertex<<std::endl;
      if (newVertexOnEdge.find(e2)!=newVertexOnEdge.end()){
            //if the other edge was subdivided
            int new_vertex_other = newVertexOnEdge[e2];

            newTriangles.push_back(t2);
            newTriangles.push_back(new_vertex);
            newTriangles.push_back(new_vertex_other);

            newTriangles.push_back(t1);
            newTriangles.push_back(new_vertex_other);
            newTriangles.push_back(new_vertex);

          }
          else{
            newTriangles.push_back(t2);
            newTriangles.push_back(new_vertex);
            newTriangles.push_back(t1);
          }

      if (newVertexOnEdge.find(e3)!=newVertexOnEdge.end()){
            //if the other edge was subdivided
            int new_vertex_other = newVertexOnEdge[e3];

            newTriangles.push_back(t0);
            newTriangles.push_back(new_vertex);
            newTriangles.push_back(new_vertex_other);

            newTriangles.push_back(t2);
            newTriangles.push_back(new_vertex_other);
            newTriangles.push_back(new_vertex);
          }
          else{
            newTriangles.push_back(t2);
            newTriangles.push_back(t0);
            newTriangles.push_back(new_vertex);
          }
    }

    else{
      if (newVertexOnEdge.find(e2)!=newVertexOnEdge.end()){
        //if the other edge was subdivided
        int new_vertex= newVertexOnEdge[e2];

        if (newVertexOnEdge.find(e3)!=newVertexOnEdge.end()){
          int new_vertex_other = newVertexOnEdge[e3];

          newTriangles.push_back(t0);
          newTriangles.push_back(new_vertex);
          newTriangles.push_back(new_vertex_other);

          newTriangles.push_back(t2);
          newTriangles.push_back(new_vertex_other);
          newTriangles.push_back(new_vertex);

        }
        else{
          newTriangles.push_back(t2);
          newTriangles.push_back(t0);
          newTriangles.push_back(new_vertex);
        }
        newTriangles.push_back(t0);
        newTriangles.push_back(new_vertex);
        newTriangles.push_back(t1);

      }
      else{
        newTriangles.push_back(t0);
        newTriangles.push_back(t1);
        newTriangles.push_back(t2);
      }
    }
  }
  
  //update m_vertex positions and triangle indices
  for (float i : newVertexPositions){
    m_vertexPositions->push_back(i);
    //TODO check if it works better if you make an entirely new vector and then assign
  }

  setTriangleIndices(newTriangles);
}

Mesh::~Mesh(){
  glDeleteBuffers(1,&mposVbo);
  glDeleteBuffers(1,&m_colVbo);
  glDeleteBuffers(1,&m_normalVbo);
  glDeleteBuffers(1,&m_texCoordVbo);
  
}
/*float calculate_gaussian_curvature(std::vector<unsigned int> triangle_index_list, unsigned int point){
  float kg=2*3.1415;
  float mixed_barycentric_area =0;
  sum_angles = 0;
  for (unsigned int t : triangle_index_list){
    mixed_barycentric_area+= (1/3)*area(t);
    sum_angles+=angle(t, point);
   }
   kg-=sum_angles;
   kg = kg / mixed_barycentric_area;
   return kg;
}*/
float Mesh::area(unsigned int triangle){
  unsigned int v0 = (*m_triangleIndices)[triangle*3  ];
  unsigned int v1 = (*m_triangleIndices)[triangle*3+1];
  unsigned int v2 = (*m_triangleIndices)[triangle*3+2];

  glm::vec3 v0_vec((*m_vertexPositions)[3*v0  ],
                   (*m_vertexPositions)[3*v0+1],
                   (*m_vertexPositions)[3*v0+2]);
  glm::vec3 v1_vec((*m_vertexPositions)[3*v1  ],
                   (*m_vertexPositions)[3*v1+1],
                   (*m_vertexPositions)[3*v1+2]);
  glm::vec3 v2_vec((*m_vertexPositions)[3*v2  ],
                   (*m_vertexPositions)[3*v2+1],
                   (*m_vertexPositions)[3*v2+2]);
  
  float a = glm::length(v0_vec -v1_vec);
  float b = glm::length (v1_vec - v2_vec);
  float c = glm::length(v2_vec-v0_vec);
  float s = (a+b+c)/2.f;
  float area = std::sqrt(s*(s-a)*(s-b)*(s-c));
  return area;  
}
float Mesh::angle(unsigned int triangle, unsigned int point){
  unsigned int v0 = (*m_triangleIndices)[triangle*3  ];
  unsigned int v1 = (*m_triangleIndices)[triangle*3+1];
  unsigned int v2 = (*m_triangleIndices)[triangle*3+2];
  unsigned int a;
  unsigned int b;
  unsigned int c;
  if (v0 == point){
    a=v0;
    b=v1;
    c=v2;
  }
  else if (v1==point){
    a=v1;
    b=v2;
    c=v0;
  }
  else if (v2 == point){
    a=v2;
    b=v1;
    c=v0;
  }
  glm::vec3 x((*m_vertexPositions)[3*a  ],
              (*m_vertexPositions)[3*a+1],
              (*m_vertexPositions)[3*a+2]);
  glm::vec3 y((*m_vertexPositions)[3*b  ],
              (*m_vertexPositions)[3*b+1],
              (*m_vertexPositions)[3*b+2]);
  glm::vec3 z((*m_vertexPositions)[3*c  ],
              (*m_vertexPositions)[3*c+1],
              (*m_vertexPositions)[3*c+2]);
  float final_angle= std::acos(glm::dot(glm::normalize(y-x),glm::normalize(z-x)));
  return final_angle;
}
glm::vec3 Mesh::calculate_laplacian_curve(std::vector<unsigned int> triangle_index_list, unsigned int point){
  struct Edge {
    unsigned int a , b;
    Edge( unsigned int c , unsigned int d ) : a( std::min<unsigned int>(c,d) ) , b( std::max<unsigned int>(c,d) ) {}
    bool operator < ( Edge const & o ) const {   return a < o.a  ||  (a == o.a && b < o.b);  }
    bool operator == ( Edge const & o ) const {   return a == o.a  &&  b == o.b;  }
    };

  glm::vec3 temp_vec(0.f);
  float mixed_barycentric_area = 0.f;
  std::set<Edge> edgesSet{};
  //contains all the edges
  std::map< Edge , std::set< unsigned int > > trianglesOnEdge; 
  // finds the triangles on the edges
  glm::vec3 point_laplacian(0.f);
  //initialize the edges
  for (unsigned int triangle_index : triangle_index_list) {
    unsigned int v0 = (*m_triangleIndices)[3*triangle_index    ];
    unsigned int v1 = (*m_triangleIndices)[3*triangle_index + 1];
    unsigned int v2 = (*m_triangleIndices)[3*triangle_index + 2];
  
    Edge edge01(v0,v1); 
    Edge edge02(v0,v2); 
    Edge edge12(v2,v1); 
    //memorize edges
    edgesSet.insert(edge01);
    edgesSet.insert(edge02);
    edgesSet.insert(edge12);
    //memorize which triangles touch them
    trianglesOnEdge[edge01].insert(triangle_index);
    trianglesOnEdge[edge02].insert(triangle_index);
    trianglesOnEdge[edge12].insert(triangle_index);
    //calculate barycentric area
    mixed_barycentric_area+=(1/3)*area(triangle_index);
    }
  for (Edge e : edgesSet){
    std::set<unsigned int> triangles = trianglesOnEdge[e];
    glm::vec3 p1;
    glm::vec3 p2;
    unsigned int a; //center vertex
    unsigned int b;
    unsigned int c;

    if(point == e.a){
      unsigned int a = e.a;
      unsigned int b = e.b;
    }
    else{
      unsigned int a = e.b;
      unsigned int b =e.a;
    }
    p1 = glm::vec3 ((*m_vertexPositions)[3*a  ],
                    (*m_vertexPositions)[3*a+1],
                    (*m_vertexPositions)[3*a+2]);
    p2 = glm::vec3 ((*m_vertexPositions)[3*b  ],
                    (*m_vertexPositions)[3*b+1],
                    (*m_vertexPositions)[3*b+2]);

    for (unsigned int triangle : triangles){
      //find opposite angle
      unsigned int alpha = (*m_triangleIndices)[3*triangle  ];
      unsigned int beta = (*m_triangleIndices)[3*triangle+1];
      unsigned int gamma = (*m_triangleIndices)[3*triangle+2];

      if ((alpha == a && beta ==b) || (beta == a && alpha == b)){
        c = gamma;
      }
      if ((alpha == a && gamma ==b)||(gamma == a && alpha == b)){
        c = beta;
      }

      if ((beta == a && gamma == b)||(gamma == a && beta ==b)){
        c= alpha;
      }

      float angle_opposite = angle(triangle, c);
      temp_vec+=(p2-p1)/(std::tan(angle_opposite));
    }
  }
  temp_vec = temp_vec/(2*mixed_barycentric_area);
  return temp_vec;
}
//glm::vec3 Mesh::compute_mean_curvature_normal()