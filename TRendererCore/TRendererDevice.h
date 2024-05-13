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
    bool topLeftFlag[3];
    int twoArea;
    float delta;
    EdgeEquation(const Triangle &tri);
    VectorI3D GetResult(int x, int y);
    void UpX(VectorI3D& res);
    void UpY(VectorI3D& res);
    Vector3D GetBarycentric(VectorI3D val);
};


class TRendererDevice
{
public:
    RenderMode renderMode{FACE};
    bool faceCulling{true};
    bool multiThread{false};
    std::vector<Vertex> vertexList;
    std::vector<unsigned> indices;
    std::vector<Texture> textureList;
    std::unique_ptr<Shader> shader;
    Color clearColor;
    Color pointColor;
    Color lineColor;

    void ClearBuffer(){frameBuffer.ClearBuffer(clearColor);}
    QImage& GetBuffer(){return frameBuffer.GetImage();}
    bool SaveImage(QString path){return frameBuffer.SaveImage(path);}
    void Render();
    static void Init(int w,int h)
    {
        GetInstance(w, h);
    }
    static TRendererDevice& GetInstance(int w = 0 ,int h = 0)
    {
        static TRendererDevice Instance(w, h);
        return Instance;
    }

    TRendererDevice(int w, int h);
    TRendererDevice(const TRendererDevice&) = delete;
    TRendererDevice(TRendererDevice &&) = delete;
    TRendererDevice& operator=(const TRendererDevice&) = delete;
    TRendererDevice& operator=(TRendererDevice&&) = delete;

private:
    int w;
    int h;
    std::array<BorderPlane, 6> viewPlanes;
    std::array<BorderLine, 4> screenLines;
    FrameBuffer frameBuffer;
    void ProcessTriangle(Triangle& tri);
    void RasterizationTriangle(Triangle& tri);
    void WireframedTriangle(Triangle& tri);
    void PointedTriangle(Triangle &tri);
    void DrawLine(Line& line);
    void ConvertToScreen(Triangle &tri);
    void ExecutePerspectiveDivision(Triangle& tri);
    CoordI4D GetBoundingBox(Triangle & tri);
    std::vector<Triangle> ClipTriangle(Triangle& tri);
    std::optional<Line> ClipLine(Line& line);
};

#endif
