#ifndef TEXTURE_H
#define TEXTURE_H

#include <QString>
#include <QImage>
#include <QDebug>
#include "BasicDataStructure.hpp"

// 纹理过滤模式
enum class FilterMode{
    NearestNeighbor,    // 最近邻采样
    Bilinear,           // 双线性插值
    Trilinear           // 三线性插值
};


class Texture
{
    enum
    {
        DIFFUSE,
        SPECLUAR
    };
    int w;
    int h;
    QImage texture;
    FilterMode filterMode;
public:
    QString path;
    Texture() = default;

    bool LoadFromImage(QString path);
    Color Sample2D(Coord2D coord);

//    Color GetNormalisedColor(int x, int y); // 获取归一化的Color
//    void GenerateMipmaps();                 // 生成 Mipmap
//    void SetFilterMode(FilterMode mode);    // 设置纹理过滤模式

private:
//    QImage textureImage;
//    std::vector<QImage> mipmaps;            // 存储 Mipmap 级别的纹理
//    Color SampleNearestNeighbor(Coord2D coord); // 最近邻采样
//    Color SampleBilinear(Coord2D coord);        // 双线性插值采样
//    Color SampleTrilinear(Coord2D coord);       // 三线性插值采样
};

#endif // TEXTURE_H
