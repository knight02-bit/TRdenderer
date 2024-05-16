#ifndef TRENDEREDEVICE_H
#define TRENDEREDEVICE_H

#include <QDebug>
#include <QTime>
#include <vector>
#include <memory>
#include <climits>
#include <bitset>
#include <immintrin.h>

#include <thread>
#include <omp.h>

#include "BasicDataStructure.hpp"
#include "FrameBuffer.h"
#include "Texture.h"
#include "Shader.hpp"


class Shader;


struct EdgeEquation
{
    VectorI3D I,J,K;
    bool topLeftFlag[3];    // 三个边界的左上标志
    int twoArea;            // 三角形面积的2倍
    float delta;            // 三角形面积的倒数

    EdgeEquation(const Triangle &tri);
    VectorI3D GetResult(int x, int y);
    void UpX(VectorI3D& res);
    void UpY(VectorI3D& res);
    Vector3D GetBarycentric(VectorI3D val);
};


class TRendererDevice
{
public:
    RenderMode renderMode{VERTEX};          // 渲染模式,默认为VERTEX
    bool faceCulling{true};                 // 面剔除开关
    bool multiThread{true};                 // 多线程开关
    std::vector<Vertex> vertexList;         // 顶点列表
    std::vector<unsigned> indices;          // 顶点索引列表
    std::vector<Texture> textureList;       // 纹理列表
    std::unique_ptr<Shader> shader;         // 着色器
    Color clearColor;                       // 清屏颜色
    Color pointColor;                       // 点渲染颜色
    Color lineColor;                        // 线框颜色

    void ClearBuffer(){frameBuffer.ClearBuffer(clearColor);}
    QImage& GetBuffer(){return frameBuffer.GetImage();}
    bool SaveImage(QString path){return frameBuffer.SaveImage(path);}
    void Render();                          // 渲染函数
    static void Init(int w,int h)           // 初始化渲染器设备
    {
        GetInstance(w, h);
    }
    static TRendererDevice& GetInstance(int w = 0 ,int h = 0)
    {
        static TRendererDevice Instance(w, h);
        return Instance;
    }

    TRendererDevice(int w, int h);
    TRendererDevice(const TRendererDevice&) = delete;   // 禁止拷贝构造函数
    TRendererDevice(TRendererDevice &&) = delete;       // 禁止移动构造函数
    TRendererDevice& operator=(const TRendererDevice&) = delete;// 禁止拷贝赋值运算符
    TRendererDevice& operator=(TRendererDevice&&) = delete;     // 禁止移动赋值运算符

private:
    int w;
    int h;
    std::array<BorderPlane, 6> viewPlanes;  // 视口平面数组
    std::array<BorderLine, 4> screenLines;  // 屏幕边界线数组
    FrameBuffer frameBuffer;                // 帧缓冲区

    void ProcessTriangle(Triangle& tri);            // 处理三角形渲染
    void RasterizationTriangle(Triangle& tri);      // 光栅化三角形
    void WireframedTriangle(Triangle& tri);         // 绘制线框三角形
    void PointedTriangle(Triangle &tri);            // 绘制点渲染三角形
    void DrawLine(Line& line);                      // 绘制线段
    void ConvertToScreen(Triangle &tri);            // 将三角形坐标转换到屏幕坐标
    void ExecutePerspectiveDivision(Triangle& tri); // 透视除法
    CoordI4D GetBoundingBox(Triangle & tri);        // 获取三角形的包围盒
    std::vector<Triangle> ClipTriangle(Triangle& tri);// 裁剪线段
    std::optional<Line> ClipLine(Line& line);
};

#endif
