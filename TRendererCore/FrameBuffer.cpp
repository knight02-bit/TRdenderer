#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int _w, int _h):w(_w), h(_h),depthBuffer(w * h),
    colorBuffer(w, h, QImage::Format_RGB888)
{
    colorBuffer.fill(QColor(0.f,0.f,0.f));

    // 初始化深度缓冲区为最大深度值1.0
    std::fill(depthBuffer.begin(),depthBuffer.end(),1.f);
}

// 判断像素的深度值是否需要更新
bool FrameBuffer::JudgeDepth(int x, int y, float z)
{
    if(z < depthBuffer[y * w + x])
    {
        depthBuffer[y * w + x] = z;
        return true;
    }
    return false;
}

void FrameBuffer::SetPixel(int x, int y, Color color)
{
    colorBuffer.setPixelColor(x, h - 1 - y
                ,QColor(color.r * 255.f, color.g * 255.f, color.b * 255.f));
}

void FrameBuffer::ClearBuffer(Color color)
{
    std::fill(depthBuffer.begin(),depthBuffer.end(),1.f);
    colorBuffer.fill(QColor(color.x * 255.f, color.y * 255.f, color.z * 255.f));
}

bool FrameBuffer::SaveImage(QString filePath)
{
    return colorBuffer.save(filePath);
}
