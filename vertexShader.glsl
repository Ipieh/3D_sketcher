#version 330 core            // Minimal GL version support expected from the GPU

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vColor; 
layout (location=2) in vec3 vNormal;
layout (location=3) in vec2 vTexCoord;

uniform mat4 viewMat, projMat, model_matrix;


out vec3 fNormal;
out vec3 fPosition;
out vec2 fTexCoord;

void main() {
  

        gl_Position = projMat*viewMat*model_matrix*vec4(vPosition, 1.0); 
        //mandatory to rasterize properly
       
        fNormal = (transpose(inverse(model_matrix))*vec4(vNormal,1.0f)).xyz;
        fPosition = vec3(model_matrix*vec4(vPosition,1.0f));
        fTexCoord = vTexCoord;
         // will be passed to the next stage

}
