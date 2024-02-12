//todo calculate the equilatera grid, given a plane
//calculate it's intersectioin with a shape and edit the corresponding mesh
#ifndef GRID_H
#define GRID_H

#include <memory>
#include <vector>
#include <glad/gl.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include "shape.hpp"
#include <cmath>
#include <iostream>
#include "vertex.hpp"

class IsometricGrid{
    private : 
        std::vector<std::shared_ptr<Vertex>> point_grid{};
        float triangle_side=1.0f;
        std::vector<Triangle> _triangles{};
        Shape _shape;
    
    public :
        IsometricGrid(Shape shape){
            _shape = shape;
            std::vector<glm::vec3> bounds = shape.compute_bounding_circle();
            //center of the circle and radius
            glm::vec3 p1=glm::normalize(bounds[0]);
            glm::vec3 p2 = glm::normalize(bounds[1]);
             float radius = glm::length(bounds[1]);

            glm::vec3 normal = glm::normalize(glm::cross(p1,p2));
            glm::vec3 x= glm::normalize(glm::cross(normal,(p2-p1)));
            glm::vec3 y = glm::normalize(p2-p1);
            //x and y are now two orthonormal vectors of the plane

            float triangle_height = std::sqrt(3)*triangle_side/2;
            unsigned int iterations_column = static_cast<unsigned int>(radius/triangle_side)+1;
            unsigned int iterations_line =static_cast<unsigned int>(radius/triangle_height)+1;

            for (int line_positive = 0; line_positive<2*iterations_line; line_positive++){
                int line = line_positive - iterations_line;
                //starting at negative values to go left first (loop breaks out for i<0 as initial value)
                //along x
                for (int col_positive = 0; col_positive<2*iterations_column;col_positive++){
                    int column = col_positive-iterations_column;

                //along y
                    glm::vec3 newPoint = p1+line*triangle_height*x+column*(triangle_side)*y;
                    if (line%2){
                        newPoint+=0.5f*triangle_side*y;
                    }
                    Vertex vertex = Vertex(newPoint);
                    vertex._index=point_grid.size();
                    point_grid.push_back(std::make_shared<Vertex>(vertex));
                }
                
            }

            for (unsigned int line = 0 ; line < 2*iterations_line ; line++){
                for(unsigned int column = 0 ; column< 2*iterations_column-1; column++){
                    if (line>0){
                        unsigned int v0 = line*2*iterations_column+ column;
                        unsigned int v1 = line*2*iterations_column+ column +1;
                        unsigned int v2 =0;
                        if (line%2){v2 = (line-1)*2*iterations_column+column+1;}
                        else{ v2= (line-1)*2*iterations_column+column;}
                        
                        Triangle t = Triangle(point_grid[v0],point_grid[v1],point_grid[v2]);
                        _triangles.push_back(t);
                    }
                    if(line<2*iterations_line-1){
                        unsigned int v0 = line*2*iterations_column+ column;
                        unsigned int v1 = line*2*iterations_column+ column +1;

                        unsigned int v2 =0;
                        if (line%2){v2 = (line+1)*2*iterations_column+column+1;}
                        else{ v2= (line+1)*2*iterations_column+column;}

                        Triangle t = Triangle(point_grid[v0],point_grid[v1],point_grid[v2]);
                        _triangles.push_back(t);
                    }

                }

            }
        
        }
        std::vector<unsigned int> getTriangles(){
            std::vector<unsigned int> triangle_indexes{};
            for (Triangle t: _triangles){
                triangle_indexes.push_back(t._a->_index);
                triangle_indexes.push_back(t._b->_index);
                triangle_indexes.push_back(t._c->_index);
            }
            return triangle_indexes;
        }
        std::vector<float> getPointPositions(){ 
            std::vector<float> positions_flattened{};
            for (std::shared_ptr<Vertex> v:point_grid ){
                glm::vec3 p= v->_pos;
                positions_flattened.push_back(p.x);
                positions_flattened.push_back(p.y);
                positions_flattened.push_back(p.z);
            }
            return positions_flattened;}

};
#endif