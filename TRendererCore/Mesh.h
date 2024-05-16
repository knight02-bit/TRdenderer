#ifndef MESH_H
#define MESH_H

#include <QString>
#include <QDebug>
#include "BasicDataStructure.hpp"
#include "TRendererDevice.h"


struct Mesh
{
    std::vector<Vertex> vertices;       // 存储顶点数据
    std::vector<unsigned> indices;      // 存储顶点索引
    int diffuseTextureIndex{-1};        // 漫反射贴图索引
    int specularTextureIndex{-1};       // 镜面反射贴图索引

    Mesh() = default;
    void Draw();
};

#endif // MESH_H
