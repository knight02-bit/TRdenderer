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
    glm::mat4 Model;                // 模型矩阵
    glm::mat4 View;                 // 视图矩阵
    glm::mat4 Projection;           // 投影矩阵
    std::vector<Light> lightList;   // 光源列表
    Material material;              // 材质
    Coord3D eyePos;                 // 观察者位置
    virtual void VertexShader(Vertex &vertex) = 0;
    virtual void FragmentShader(Fragment &fragment) = 0;
};

#endif
