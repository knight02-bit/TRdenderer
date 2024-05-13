#ifndef SHADER1_H
#define SHADER1_H

#include <cmath>
#include <string>
#include <functional>
#include "TRendererDevice.h"
#include "BasicDataStructure.hpp"

class TRendererDevice;

class Shader
{
public:
    ~Shader(){};
    glm::mat4 Model;
    glm::mat4 View;
    glm::mat4 Projection;
    std::vector<Light> lightList;
    Material material;
    Coord3D eyePos;
    virtual void VertexShader(Vertex &vertex) = 0;
    virtual void FragmentShader(Fragment &fragment) = 0;

};

#endif
