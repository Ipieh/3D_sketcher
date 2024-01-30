#ifndef MESH_H
#define MESH_H

#include <memory>
#include <vector>
#include <glad/gl.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

class Mesh {
    public:
        void init(); //inits gpu and cpu
        void render(GLuint g_program); // should be called in the main rendering loop
        static std::shared_ptr<Mesh> genSphere(const size_t resolution=16); // should generate a unit sphere // ...
        static std::shared_ptr<Mesh> genLine(std::vector<glm::vec3> line,glm::vec3 normalVec);
        //static float thicknessLine;
        void setVertexPositions(std::vector<float> vertexPositions);
        void setTriangleIndices(std::vector<unsigned int> triangleIndices);
        void setVertexTexCoords(std::vector<float> vertexTexCoords);
        void setColors(std::vector<float> sphereColors);
        void setNormals(std::vector <float> sphereNormals);
        void setModelMatrix(glm::mat4 model_matrix);
        void setShininess(float shininess);
        void isSun();
        ~Mesh();
    private:
        float m_shininess=50.0;
        std::vector<float> m_vertexPositions; 
        std::vector<float> m_vertexColors; 
        std::vector<unsigned int> m_triangleIndices;
        std::vector<float> m_vertexNormals;
        std::vector<float> m_vertexTexCoords;
        glm::mat4 model_matrix = glm::mat4(1.0f);
        
        bool isSourceOfLight = 0;

        void initGPUGeometry(); // should properly set up the geometry buffer
        void initCPUGeometry();
        
        GLuint m_texCoordVbo = 0;
        GLuint m_vao = 0;
        GLuint mposVbo =0;
        GLuint m_normalVbo = 0;
        GLuint m_colVbo = 0;
        GLuint m_ibo = 0;
        
        
};

#endif