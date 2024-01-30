#ifndef DRAWTOOL_H
#define DRAWTOOL_H

#include <memory>
#include <vector>
#include <glad/gl.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

class DrawTool{
    private:
        int smooth_mode = 1 ; // 0 when drawing sharp edges, 1 otherwise
        //thinking of putting all the meshes in a list for later
        //do i distinguish the lines from the rest
        //yes so i can edit them, hide them, erase them...
        //let the drawing tool keep the lines

        //called from the interface with every click, probably main, or create a window
        //highlight line that is hovered
        //drag line

        //second part of draw tool is computing the filling mesh
        //some kind of dictionnary that links both, integer for example.
        //handling deletions?
        //applying a second constraint on the same mesh object
        //creating irregular meshes
        //yeesssir

};

#endif