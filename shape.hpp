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
            std::cout<<"screen coords: "<<x<< " , "<<y<<"\n";
            glm::vec4 screenCoords(x/512-1.f,1.f-y/384,0.99f,1.f);
            glm::mat4 proj = camera.computeProjectionMatrix();
            glm::mat4 view = camera.computeViewMatrix();
            glm::mat4 proj_inv=glm::inverse(proj);
            glm::mat4 view_inv=glm::inverse(view);
            glm::vec4 world_coords_homogenous = proj_inv*screenCoords;
            
            world_coords_homogenous = view_inv* world_coords_homogenous;
            world_coords_homogenous/= world_coords_homogenous.w;
            glm::vec3 world_coords = glm::vec3(world_coords_homogenous);

            points.push_back(world_coords);
        };

        void close_shape(){
            points.push_back(glm::vec3(points[0].x,points[0].y,points[0].z));
        }
        std::vector<glm::vec3> getPoints(){return points;}

    private:
        std::vector<glm::vec3> points{};
};

#endif