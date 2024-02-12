#ifndef VERTEX_H
#define VERTEX_H
#include <array>
#include <glad/gl.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <iostream>

class Vertex{
    public :
        unsigned int _index;
        glm::vec3 _pos;
        bool deleted=false;
        Vertex(glm::vec3 pos){
            _pos = pos;
        }
        void remove(){
            deleted=true;
        }
};

class Triangle{
    public :
        Triangle(std::shared_ptr<Vertex> a, std::shared_ptr<Vertex> b, std::shared_ptr<Vertex> c){
            _a=a;
            _b=b;
            _c=c;
        }
        std::shared_ptr<Vertex> _a;
        std::shared_ptr<Vertex> _b;
        std::shared_ptr<Vertex> _c;
};

struct Edge {
    unsigned int a , b;
    Edge( unsigned int c , unsigned int d ) : a( std::min<unsigned int>(c,d) ) , b( std::max<unsigned int>(c,d) ) {}
    bool operator < ( Edge const & o ) const {   return a < o.a  ||  (a == o.a && b < o.b);  }
    bool operator == ( Edge const & o ) const {   return a == o.a  &&  b == o.b;  }
    };
#endif