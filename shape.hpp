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
        void addPoint(double x, double y, Camera camera, bool setToZeroPlane = true){
            //calculate world position of vertex

            glm::vec4 screenCoords(x/512-1.f,1.f-y/384,0.99f,1.f);
            glm::mat4 proj = camera.computeProjectionMatrix();
            glm::mat4 view = camera.computeViewMatrix();
            glm::mat4 proj_inv=glm::inverse(proj);
            glm::mat4 view_inv=glm::inverse(view);
            glm::vec4 world_coords_homogenous = proj_inv*screenCoords;
            
            world_coords_homogenous = view_inv* world_coords_homogenous;
            world_coords_homogenous/= world_coords_homogenous.w;
            glm::vec3 world_coords = glm::vec3(world_coords_homogenous);
            
            
            if (setToZeroPlane) {
            // Project the world coordinates onto the plane containing the origin
                glm::vec3 camera_position = camera.getPosition();
                glm::vec3 normal_to_plane = glm::normalize(camera_position);  // Assuming camera_position is not (0, 0, 0)
                float distance_to_plane = glm::dot(world_coords, normal_to_plane);
                world_coords -= distance_to_plane * normal_to_plane;
                world_coords*=(40.f/(40.f-23.98))*0.99f;
                //these values were found empirically : 40 is the norm of distance camera
                // 23.98 is the z coord when the camera isn't moved so what happens to a click after the transformation
                //0.99 is the z depth value in the screen coords first vector.
                //i did this using thales theorem to scale but have yet to find a direct method to compensate for the projection.
                }
            
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