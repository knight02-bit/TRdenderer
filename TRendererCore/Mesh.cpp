#include "Mesh.h"

// 绘制网格, 根据索引和顶点数据进行渲染
void Mesh::Draw()
{
    TRendererDevice::GetInstance().vertexList = vertices;
    TRendererDevice::GetInstance().indices = indices;
    TRendererDevice::GetInstance().shader->material.diffuse = diffuseTextureIndex;
    TRendererDevice::GetInstance().shader->material.specular = specularTextureIndex;
    TRendererDevice::GetInstance().Render();
}
