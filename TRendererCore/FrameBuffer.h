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
	int w;
	int h;
    std::vector<float> depthBuffer;
    QImage colorBuffer;
};

#endif
