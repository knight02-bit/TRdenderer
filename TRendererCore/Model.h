#ifndef MODEL_H
#define MODEL_H

#include <QDebug>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cfloat>
#include "BasicDataStructure.hpp"
#include "Mesh.h"

class Model
{
public:
    Coord3D centre;         // 模型中心点坐标
    int meshNum{0};
    int triangleCount{0};
    int vertexCount{0};

    Model(QString path);
    void Draw();
    float GetYRange(){return maxY - minY;}
    bool loadSuccess{true};

private:
    float minX{FLT_MAX};
    float minY{FLT_MAX};
    float minZ{FLT_MAX};
    float maxX{FLT_MIN};
    float maxY{FLT_MIN};
    float maxZ{FLT_MIN};
    std::vector<Mesh> meshes;           // 存储模型网格
    std::vector<Texture> textureList;   // 存储纹理
    QString directory;                  // 模型所在目录路径

    void loadModel(QString path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    int loadMaterialTextures(Mesh & mesh,aiMaterial *mat, aiTextureType type);
};

#endif // MODEL_H
