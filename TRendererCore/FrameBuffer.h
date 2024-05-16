#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "BasicDataStructure.hpp"
#include <iostream>
#include <string>
#include <cfloat>
#include <QImage>
#include <QColor>
#include <QString>
#include <QDebug>

class FrameBuffer
{
public:
    FrameBuffer(int _w, int _h);
    bool JudgeDepth(int x, int y, float z);
    void SetPixel(int x, int y, Color color);
    bool SaveImage(QString filePath);
    void ClearBuffer(Color color);
    QImage& GetImage(){return colorBuffer;}

private:
    int w;                          // 帧缓冲区宽度
    int h;                          // 帧缓冲区高度
    std::vector<float> depthBuffer; // 深度缓冲区
    QImage colorBuffer;             // 颜色缓冲区
};

#endif
