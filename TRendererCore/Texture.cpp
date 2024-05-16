#include "Texture.h"


bool Texture::LoadFromImage(QString path)
{
    this->path = path;
    filterMode = FilterMode::NearestNeighbor;
    if (texture.load(path))
    {
        w = texture.width();
        h = texture.height();
//        GenerateMipmaps();  // 加载纹理后生成 Mipmap
        return true;
    }
    return false;
}

//****************************************
// 此处默认使用NearestNeighbor(最近邻采样)
Color Texture::Sample2D(Coord2D coord)
{
    int x = static_cast<int>(coord.x * w - 0.5f) % w;
    // 翻转纹理坐标的 y 分量
    int y = static_cast<int>((1 - coord.y) * h - 0.5f) % h;
    x = x < 0 ? w + x : x;
    y = y < 0 ? h + y : y;
    return Color(texture.pixelColor(x, y).red() / 255.f, texture.pixelColor(x, y).green() / 255.f, texture.pixelColor(x, y).blue() / 255.f);
}

//Color Texture::Sample2D(Coord2D coord)
//{
//    switch (filterMode)
//    {
//    case FilterMode::NearestNeighbor:
//        return SampleNearestNeighbor(coord);
//    case FilterMode::Bilinear:
//        return SampleBilinear(coord);
//    case FilterMode::Trilinear:
//        return SampleTrilinear(coord);
//    default:
//        return Color(122.0f, 122.0f, 122.0f);
//    }
//}

//void Texture::GenerateMipmaps()
//{
//    mipmaps.clear();                    // 清空之前的 Mipmap
//    mipmaps.push_back(texture);    // 原始纹理作为第 0 级 Mipmap

//    int level = 1;
//    while (w > 1 || h > 1)
//    {
//        w = std::max(1, w / 2); // 宽度减半
//        h = std::max(1, h / 2); // 高度减半
//        mipmaps.push_back(texture.scaled(w, h));
//        ++ level;
//    }
//}

//Color Texture::GetNormalisedColor(int x, int y)
//{
//    return Color(texture.pixelColor(x, y).red() / 255.f, texture.pixelColor(x, y).green() / 255.f, texture.pixelColor(x, y).blue() / 255.f);
//}


//Color Texture::SampleNearestNeighbor(Coord2D coord)
//{
//    int x = static_cast<int>(coord.x * w - 0.5f) % w;
//    int y = static_cast<int>(coord.y * h - 0.5f) % h;
//    x = x < 0 ? w + x : x;
//    y = y < 0 ? h + y : y;
//    return GetNormalisedColor(x, y);
//}

//Color Texture::SampleBilinear(Coord2D coord)
//{
//    float u = coord.x * (w - 1);
//    float v = coord.y * (h - 1);
//    int x0 = static_cast<int>(std::floor(u));
//    int x1 = static_cast<int>(std::ceil(u));
//    int y0 = static_cast<int>(std::floor(v));
//    int y1 = static_cast<int>(std::ceil(v));
//    float tx = u - x0;
//    float ty = v - y0;

//    Color c00 = GetNormalisedColor(x0, y0);
//    Color c01 = GetNormalisedColor(x0, y1);
//    Color c10 = GetNormalisedColor(x1, y0);
//    Color c11 = GetNormalisedColor(x1, y1);

//    Color top = c00 * (1 - tx) + c10 * tx;
//    Color bottom = c01 * (1 - tx) + c11 * tx;
//    return top * (1 - ty) + bottom * ty;
//}

//Color Texture::SampleTrilinear(Coord2D coord)
//{
//    float level = mipmaps.size() - 1;
//    float u = coord.x * (w - 1);
//    float v = coord.y * (h - 1);
//    int x0 = static_cast<int>(std::floor(u));
//    int x1 = static_cast<int>(std::ceil(u));
//    int y0 = static_cast<int>(std::floor(v));
//    int y1 = static_cast<int>(std::ceil(v));
//    float tx = u - x0;
//    float ty = v - y0;

//    Color c00 = GetNormalisedColor(x0, y0);
//    Color c01 = GetNormalisedColor(x0, y1);
//    Color c10 = GetNormalisedColor(x1, y0);
//    Color c11 = GetNormalisedColor(x1, y1);

//    Color top = c00 * (1 - tx) + c10 * tx;
//    Color bottom = c01 * (1 - tx) + c11 * tx;
//    Color finalColor = top * (1 - ty) + bottom * ty;

//    // 使用 Mipmap 选择算法，比如 LOD bias 或者使用相机距离计算
//    // 这里使用简单的 LOD bias
//    float bias = 0.5f;
//    level = std::max(0.0f, level - bias);

//    // 通过双线性插值对相邻 Mipmap 级别进行插值
//    Color nextLevelColor = SampleBilinear(coord * 0.5f);
//    return finalColor * (1 - level) + nextLevelColor * level;
//}
