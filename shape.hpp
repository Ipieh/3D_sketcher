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
            screen_points_coords.push_back(glm::vec2(x,y));
            glm::vec4 screenCoords(2*x/camera.m_width-1.f,1.f-2*y/camera.m_height,0.99f,1.f);
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
            if (glm::length(last_added_point-world_coords)>min_distance){
            points.push_back(world_coords);
            last_added_point=world_coords;
            }
        };

        void close_shape(){
            points.push_back(glm::vec3(points[0].x,points[0].y,points[0].z));
        }
        std::vector<glm::vec3> getPoints(){return points;}
        std::vector<glm::vec2> get_screen_points_coords(){return screen_points_coords;};
        std::vector<glm::vec3> compute_bounding_circle(){
            glm::vec3 center(0.0);
            glm::vec3 radius(0.0);
            for (glm::vec3 p : points){
                center+=p;
            }
            center = (1.0f/points.size())*center; 
            for (glm::vec3 point : points){
                if (glm::length(point-center)>glm::length(radius)){
                    radius = point-center;
                }
            }
            return std::vector<glm::vec3>{center,radius};
        }
    private:
        float min_distance = 0.5f;
        glm::vec3 last_added_point = glm::vec3(0.f,0.f,0.f);
        std::vector<glm::vec3> points{};
        std::vector<glm::vec2> screen_points_coords{};
       // std::vector<glm::vec3> get_curvature(int index_of_point);
};

#endif