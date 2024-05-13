#include "Mesh.h"


void Mesh::Draw()
{
    TRendererDevice::GetInstance().vertexList = vertices;
    TRendererDevice::GetInstance().indices = indices;
    TRendererDevice::GetInstance().shader->material.diffuse = diffuseTextureIndex;
    TRendererDevice::GetInstance().shader->material.specular = specularTextureIndex;
    TRendererDevice::GetInstance().Render();
}
