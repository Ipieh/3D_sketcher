#ifndef MESH_H
#define MESH_H

#include <memory>
#include <vector>
#include <glad/gl.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include "shape.hpp"

class Mesh {
    public:
        void init(); //inits gpu and cpu
        void render(GLuint g_program); // should be called in the main rendering loop
        static std::shared_ptr<Mesh> genSphere(const size_t resolution=16); // should generate a unit sphere // ...
        static std::shared_ptr<Mesh> genLine(std::vector<glm::vec3> line,glm::vec3 normalVec);
        static std::shared_ptr<Mesh> genMeshConstraint(std::shared_ptr<Shape> first_shape, Camera camera);
        //static float thicknessLine;
        void setVertexPositions(std::vector<float> vertexPositions);
        void setTriangleIndices(std::vector<unsigned int> triangleIndices);
        void setVertexTexCoords(std::vector<float> vertexTexCoords);
        void setColors(std::vector<float> sphereColors);
        void setNormals(std::vector <float> sphereNormals);
        void setModelMatrix(glm::mat4 model_matrix);
        void setShininess(float shininess);
        void isSun();
        //TODO rename isSun or delete it...
    
        void interpolate_new_constraints(Shape current_shape);
        //takes care of new constraints
        ~Mesh();
    private:
        float m_shininess=50.0;
        unsigned int m_offset = 2;
        std::shared_ptr<std::vector<float>> m_vertexPositions=nullptr;
        //contains world positions of the vertices
        std::shared_ptr<std::vector<float>> m_vertexColors=nullptr; 
        //contains hard coded colors of every vertex
        std::shared_ptr<std::vector<unsigned int>> m_triangleIndices=nullptr;
        //every 3 elements forms a triangle
        //the int sends back to its position in m_vertexPositions
       std::shared_ptr< std::vector<float>> m_vertexNormals=nullptr;
        //not implemented - not useful in this context
        std::shared_ptr<std::vector<float>> m_vertexTexCoords=nullptr;
        //not useful
        glm::mat4 model_matrix = glm::mat4(1.0f);
        
        bool isSourceOfLight = 0;

        void initGPUGeometry(); // sets up the geometry buffer
        void initCPUGeometry();
        void remesh_isotropic(float L=2.f);
        void split_long_edges(float maxL);
        void collapse_short_edges(float minL);
        void compute_normals();
        void compute_colors(float r, float g, float b);
        void compute_texCoords();

        float area(unsigned int triangle); //calculates the area of a given triangle
        float angle(unsigned int triangle, unsigned int point); //calculate the angle of a triangle's corner
        glm::vec3 calculate_laplacian_curve(std::vector<unsigned int> triangle_index_list, 
                                  unsigned int point); // calculated the main cuurvature vertex
        
        GLuint m_texCoordVbo = 0;
        GLuint m_vao = 0;
        GLuint mposVbo =0;
        GLuint m_normalVbo = 0;
        GLuint m_colVbo = 0;
        GLuint m_ibo = 0;
        
        
};

#endif