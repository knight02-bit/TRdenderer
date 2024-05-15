#include "BlinnPhongShader.h"

void BlinnPhongShader::VertexShader(Vertex &vertex)
{
    // 将顶点的空间坐标变换到模型空间
    vertex.worldSpacePos = Coord3D(Model * Coord4D(vertex.worldSpacePos, 1.f));
    // 将顶点的空间坐标变换到裁剪空间
    vertex.clipSpacePos = Projection * View * Coord4D(vertex.worldSpacePos, 1.f);
    // 计算法线在模型空间的变换(使用逆转置矩阵)
    vertex.normal = glm::mat3(glm::transpose(glm::inverse(Model))) * vertex.normal;
}

void BlinnPhongShader::FragmentShader(Fragment &fragment)
{
    Color diffuseColor = {0.6f,0.6f,0.6f};
    Color specularColor = {1.0f,1.0f,1.0f};

    // 若材质中有漫反射贴图
    if(material.diffuse != -1)
        diffuseColor = TRendererDevice::GetInstance().
                       textureList[material.diffuse].
                       Sample2D(fragment.texCoord);
    // 若材质中有镜面反射贴图
    if(material.specular != -1)
        specularColor = TRendererDevice::GetInstance().
                        textureList[material.specular].
                        Sample2D(fragment.texCoord);

    Vector3D normal = glm::normalize(fragment.normal);
    Vector3D viewDir = glm::normalize(eyePos - fragment.worldSpacePos);

    // 光源对当前片元的光照效果
    auto calculateLight = [&](Light light)->Color
    {
        Vector3D lightDir;
        if(light.pos.w != 0.f)      // 光源是点光源
            lightDir = glm::normalize(Coord3D(light.pos) - fragment.worldSpacePos);
        else                        // 光源是定向光
            lightDir = - Vector3D(light.dir);

        Color ambient = light.ambient * diffuseColor;
        Color diffuse = light.diffuse * std::max(glm::dot(normal,lightDir), 0.f) * diffuseColor;
        Color specular = light.specular * std::pow(std::max(glm::dot(normal, glm::normalize(viewDir + lightDir)), 0.0f), material.shininess) * specularColor;
        return (ambient + diffuse + specular);
    };

    // 累加所有光源的光照效果
    Color result(0.f, 0.f, 0.f);
    for(auto light : lightList)
    {
        result += calculateLight(light);
    }

    // 将颜色分量限制在[0,1]范围内
    if(result.x > 1.f) result.x = 1.f;
    if(result.y > 1.f) result.y = 1.f;
    if(result.z > 1.f) result.z = 1.f;

    fragment.fragmentColor = result;
}
