#ifndef SHAPE_H
#define SHAPE_H

#include <memory>
#include <vector>
#include <glad/gl.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include "camera.hpp"
#include <iostream>


class Shape{
    public:
        void addPoint(double x, double y, Camera camera){
            //calculate world position of vertex
            
            glm::vec4 screenCoords(x/512-1,y/384-1,0.5f,1.f);
            glm::mat4 proj = camera.computeProjectionMatrix();
            glm::mat4 view = camera.computeViewMatrix();
            glm::mat4 proj_view_inv=glm::inverse(view*proj);
            glm::vec4 world_coords_homogenous = screenCoords*proj_view_inv;
            std::cout<<"homogenous coord" <<world_coords_homogenous.z<<"\n";
            glm::vec3 world_coords = glm::vec3(world_coords_homogenous.x/world_coords_homogenous.w ,
                                               world_coords_homogenous.y/world_coords_homogenous.w ,
                                               world_coords_homogenous.z/world_coords_homogenous.w );

            points.push_back(world_coords);
        };

        void close_shape(){
            points.push_back(points[0]);
        }
        std::vector<glm::vec3> getPoints(){return points;}

    private:
        std::vector<glm::vec3> points{};
};

#endif