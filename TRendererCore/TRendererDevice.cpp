#include "TRendererDevice.h"
//#define MIN(a, b) ((a) < (b) ? (a) : (b))
//#define MAX(a, b) ((a) > (b) ? (a) : (b))


/********************************************************************************/
// helper functions

static inline bool JudgeOnTopLeftEdge(CoordI2D v0, CoordI2D v1)
{
    return (v0.y > v1.y) || (v0.x > v1.x && v1.y == v0.y);
}

static inline bool JudgeInsideTriangle(EdgeEquation& triEdge,VectorI3D res)
{
    bool flag = true;
    if(res.x == 0) flag &= triEdge.topLeftFlag[0];
    if(res.y == 0) flag &= triEdge.topLeftFlag[1];
    if(res.z == 0) flag &= triEdge.topLeftFlag[2];
    return flag && ((res.x >= 0 && res.y >= 0 && res.z >=0) || (res.x <= 0 && res.y <= 0 && res.z <= 0));
}

template<class T>
static inline T CorrectPerspective(float viewDepth, std::vector<T> attribute, Triangle& tri, glm::vec3& barycentric)
{
    return viewDepth * (barycentric.x * attribute[0] / tri[0].clipSpacePos.w + barycentric.y * attribute[1] / tri[1].clipSpacePos.w + barycentric.z * attribute[2] / tri[2].clipSpacePos.w);
}

template<class T>
static inline T CalculateInterpolation(T a, T b, T c, Vector3D &barycentric)
{
    return a * barycentric.x + b * barycentric.y + c * barycentric.z;
}

template<class T>
static inline T CalculateInterpolation(T a, T b, float alpha)
{
    return a * (1 - alpha) + b * alpha;
}

static inline CoordI2D CalculateInterpolation(CoordI2D a, CoordI2D b, float alpha)
{
    CoordI2D res;
    res.x = static_cast<int>(a.x * (1 - alpha) + b.x * alpha + 0.5f);
    res.y = static_cast<int>(a.y * (1 - alpha) + b.y * alpha + 0.5f);
    return res;
}

static inline Vertex CalculateInterpolation(Vertex a, Vertex b, float alpha)
{
    Vertex res;
    res.clipSpacePos = CalculateInterpolation(a.clipSpacePos, b.clipSpacePos, alpha);
    res.worldSpacePos = CalculateInterpolation(a.worldSpacePos, b.worldSpacePos, alpha);
    res.normal = CalculateInterpolation(a.normal, b.normal, alpha);
    res.texCoord = CalculateInterpolation(a.texCoord, b.texCoord, alpha);
    return res;
}

template<class T>
static inline float CalculateDistance(T point, T border)
{
    return glm::dot(point, border);
}

template<class T,size_t N>
static inline std::bitset<N> GetClipCode(T point, std::array<T, N>& clip)
{
    std::bitset<N> res;
    for(int i = 0; i < N; ++i)
        if(CalculateDistance(point, clip[i]) < 0) res.set(i, 1);
    return res;
}

void TRendererDevice::ExecutePerspectiveDivision(Triangle &tri)
{
    for (int i = 0; i < 3; ++i)
    {
        tri[i].ndcSpacePos.x /= tri[i].clipSpacePos.w;
        tri[i].ndcSpacePos.y /= tri[i].clipSpacePos.w;
        tri[i].ndcSpacePos.z /= tri[i].clipSpacePos.w;
    }
}

void TRendererDevice::ConvertToScreen(Triangle &tri)
{
    for (int i = 0; i < 3; ++i)
    {
        tri[i].screenPos.x = static_cast<int>(0.5f * w * (tri[i].ndcSpacePos.x + 1.0f) + 0.5f);
        tri[i].screenPos.y = static_cast<int>(0.5f * h * (tri[i].ndcSpacePos.y + 1.0f) + 0.5f);
        tri[i].screenDepth = tri[i].ndcSpacePos.z;
    }
}

std::vector<Triangle> ConstructTriangle(std::vector<Vertex> vertexList)
{
    std::vector<Triangle> res;
    for(int i = 0; i < vertexList.size() - 2; ++i)
    {
        int k = (i + 1) % vertexList.size();
        int m = (i + 2) % vertexList.size();
        Triangle tri{vertexList[0], vertexList[k], vertexList[m]};
        res.push_back(tri);
    }
    return res;
}

Fragment ConstructFragment(int x, int y, float z, float viewDepth, Triangle& tri, Vector3D& barycentric)
{
    Fragment frag;
    frag.screenPos.x = x;
    frag.screenPos.y = y;
    frag.screenDepth = z;
    frag.worldSpacePos = CorrectPerspective(viewDepth, std::vector<Color>{tri[0].worldSpacePos, tri[1].worldSpacePos, tri[2].worldSpacePos}, tri, barycentric);
    frag.normal = CorrectPerspective(viewDepth, std::vector<Vector3D>{tri[0].normal, tri[1].normal, tri[2].normal}, tri, barycentric);
    frag.texCoord = CorrectPerspective(viewDepth, std::vector<Coord2D>{tri[0].texCoord, tri[1].texCoord, tri[2].texCoord}, tri, barycentric);
    return frag;
}

// 获取三角形的边界框坐标
CoordI4D TRendererDevice::GetBoundingBox(Triangle & tri)
{
    int xMin = w - 1;
    int yMin = h - 1;
    int xMax = 0;
    int yMax = 0;
    for(int i = 0; i < 3; ++i)
    {
        xMin = std::min(xMin, tri[i].screenPos.x);
        yMin = std::min(yMin, tri[i].screenPos.y);
        xMax = std::max(xMax, tri[i].screenPos.x);
        yMax = std::max(yMax, tri[i].screenPos.y);
    }
    return {
        xMin > 0 ? xMin : 0,
        yMin > 0 ? yMin : 0,
        xMax < w - 1 ? xMax : w - 1,
        yMax < h - 1 ? yMax : h - 1};
}

/********************************************************************************/
// https://zhuanlan.zhihu.com/p/140926917
// 通过三角形的屏幕空间坐标计算边缘方程的参数
EdgeEquation::EdgeEquation(const Triangle &tri)
{
    I = {
        tri[0].screenPos.y - tri[1].screenPos.y,
        tri[1].screenPos.y - tri[2].screenPos.y,
        tri[2].screenPos.y - tri[0].screenPos.y };
    J = {
        tri[1].screenPos.x - tri[0].screenPos.x,
        tri[2].screenPos.x - tri[1].screenPos.x,
        tri[0].screenPos.x - tri[2].screenPos.x };
    K = {
        tri[0].screenPos.x * tri[1].screenPos.y - tri[0].screenPos.y * tri[1].screenPos.x,
        tri[1].screenPos.x * tri[2].screenPos.y - tri[1].screenPos.y * tri[2].screenPos.x,
        tri[2].screenPos.x * tri[0].screenPos.y - tri[2].screenPos.y * tri[0].screenPos.x   };

    topLeftFlag[0] = JudgeOnTopLeftEdge(tri[0].screenPos,tri[1].screenPos);
    topLeftFlag[1] = JudgeOnTopLeftEdge(tri[1].screenPos,tri[2].screenPos);
    topLeftFlag[2] = JudgeOnTopLeftEdge(tri[2].screenPos,tri[0].screenPos);
    twoArea = K[0] + K[1] + K[2];
    delta = 1.f / twoArea;
}

VectorI3D EdgeEquation::GetResult(int x, int y)
{
    VectorI3D res = I * x + J * y + K;
    return res;
}

void EdgeEquation::UpX(VectorI3D& res)
{
    res += I;
}

void EdgeEquation::UpY(VectorI3D& res)
{
    res += J;
}

Vector3D EdgeEquation::GetBarycentric(VectorI3D val)
{
    return {val.y * delta, val.z * delta, val.x * delta};
}

/********************************************************************************/
// 初始化裁剪平面和屏幕边界
TRendererDevice::TRendererDevice(int w, int h):shader(nullptr), w(w), h(h), frameBuffer(w, h)
{
    {// set view planes
        // near
        viewPlanes[0] = {0, 0, 1.f, 1.f};
        // far
        viewPlanes[1] = {0, 0, -1.f, 1.f};
        // left
        viewPlanes[2] = {1.f, 0, 0, 1.f};
        // right
        viewPlanes[3] = {-1.f, 0, 0, 1.f};
        // top
        viewPlanes[4] = {0, 1.f, 0, 1.f};
        // bottom
        viewPlanes[5] = {0, -1.f, 0, 1.f};
    }
    {// set screen border
        // left
        screenLines[0] = {1.f, 0, 0};
        // right
        screenLines[1] = {-1.f, 0, (float)w};
        // bottom
        screenLines[2] = {0, 1.f, 0};
        // top
        screenLines[3] = {0, -1.f, (float)h};
    }
}


/********************************************************************************/
// Half-Space 三角形光栅化算法
void TRendererDevice::RasterizationTriangle(Triangle &tri)
{
    CoordI4D boundingBox = GetBoundingBox(tri);
    int xMin = boundingBox[0];
    int yMin = boundingBox[1];
    int xMax = boundingBox[2];
    int yMax = boundingBox[3];
    EdgeEquation triEdge(tri);
    if(faceCulling && triEdge.twoArea <= 0) return;
    else if(triEdge.twoArea == 0) return;
    Fragment frag;
    VectorI3D cy = triEdge.GetResult(xMin, yMin);
    for(int y = yMin; y <= yMax; y++)
    {
        VectorI3D cx = cy;
        for(int x = xMin; x <= xMax; x++)
        {
            if(JudgeInsideTriangle(triEdge, cx))
            {
                Vector3D barycentric = triEdge.GetBarycentric(cx);
                float screenDepth = CalculateInterpolation(tri[0].screenDepth, tri[1].screenDepth, tri[2].screenDepth, barycentric);
                if (frameBuffer.JudgeDepth(x, y, screenDepth))
                {
                    float viewDepth = 1.0f / (barycentric.x / tri[0].ndcSpacePos.w + barycentric.y / tri[1].ndcSpacePos.w + barycentric.z / tri[2].ndcSpacePos.w);
                    frag = ConstructFragment(x, y, screenDepth, viewDepth, tri, barycentric);
                    shader->FragmentShader(frag);
                    frameBuffer.SetPixel(frag.screenPos.x,frag.screenPos.y,frag.fragmentColor);
                }
            }
            triEdge.UpX(cx);
        }
        triEdge.UpY(cy);
    }
}


/********************************************************************************/
// Bresenham 画线算法
void TRendererDevice::DrawLine(Line& line)
{
    int x0 = glm::clamp(static_cast<int>(line[0].x), 0, w - 1);
    int x1 = glm::clamp(static_cast<int>(line[1].x), 0, w - 1);
    int y0 = glm::clamp(static_cast<int>(line[0].y), 0, h - 1);
    int y1 = glm::clamp(static_cast<int>(line[1].y), 0, h - 1);
    bool steep = false;
    if (abs(x0 - x1) < abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int k = dy > 0 ? 1 : -1;
    if (dy < 0)dy = -dy;
    float e = -dx;
    int x = x0, y = y0;
    while (x != x1)
    {
        if (steep)frameBuffer.SetPixel(y, x, lineColor);
        else frameBuffer.SetPixel(x, y, lineColor);
        e += (2 * dy);
        if (e > 0)
        {
            y += k;
            e -= (2 * dx);
        }
        ++x;
    }
}


/********************************************************************************/
// Cohen-Sutherland算法(线段裁剪)
std::optional<Line> TRendererDevice::ClipLine(Line& line)
{
    // 计算线段端点的裁剪码
    std::bitset<4> code[2] =
    {
        GetClipCode(Coord3D(line[0], 1), screenLines),
        GetClipCode(Coord3D(line[1], 1), screenLines)
    };
    // 如果两个端点都在裁剪区域内，则直接返回
    if((code[0] | code[1]).none()) return line;
    // 如果两个端点都在裁剪区域外，则直接丢弃该直线
    if((code[0] & code[1]).any()) return std::nullopt;

    for(int i = 0; i < 4; ++i)
    {
        // 如果两个端点在不同的裁剪区域内部或外部
        // 裁剪码是通过位编码来表示点的位置关系的
        // 对两个裁剪码进行位异或运算,取索引为i的位,可检查两端点在第i位上是否不同
        if((code[0] ^ code[1])[i])
        {
            // 计算直线端点到裁剪平面的距离
            float da = CalculateDistance(Coord3D(line[0], 1), screenLines[i]);
            float db = CalculateDistance(Coord3D(line[1], 1), screenLines[i]);

            // 计算交点处的插值系数
            float alpha = da / (da - db);

            // 计算交点的屏幕坐标
            CoordI2D np = CalculateInterpolation(line[0], line[1], alpha);

            // 根据裁剪平面的不同，更新直线的端点和裁剪码
            if(da > 0){
                line[1] = np;
                code[1] = GetClipCode(Coord3D(np, 1), screenLines);
            }else{
                line[0] = np;
                code[0] = GetClipCode(Coord3D(np, 1), screenLines);
            }
        }
    }
    return line;
}


/********************************************************************************/
// 渲染三角形线框
void TRendererDevice::WireframedTriangle(Triangle &tri)
{
    Line triLine[3] =
    {
        {tri[0].screenPos, tri[1].screenPos},
        {tri[1].screenPos, tri[2].screenPos},
        {tri[2].screenPos, tri[0].screenPos},
    };
    for(auto &line : triLine)
    {
        auto res = ClipLine(line);
        if(res) DrawLine(*res);
    }
}


/********************************************************************************/
// 渲染三角形顶点
void TRendererDevice::PointedTriangle(Triangle &tri)
{
    for(int i = 0; i < 3; ++i)
    {
        if(tri[i].screenPos.x >= 0 && tri[i].screenPos.x <= w - 1 &&
                tri[i].screenPos.y >= 0 && tri[i].screenPos.y <= h - 1 &&
                tri[i].screenDepth <= 1.f)
        {
            frameBuffer.SetPixel(tri[i].screenPos.x, tri[i].screenPos.y,
                                 pointColor);
        }
    }
}


/********************************************************************************/
// Sutherland-Hodgman 算法(齐次空间裁剪)
std::vector<Triangle> TRendererDevice::ClipTriangle(Triangle &tri)
{
    // 计算三角形三个顶点的裁剪码
    std::bitset<6> code[3] =
    {
        GetClipCode(tri[0].clipSpacePos, viewPlanes),
        GetClipCode(tri[1].clipSpacePos, viewPlanes),
        GetClipCode(tri[2].clipSpacePos, viewPlanes)
    };

    // 如果三个顶点的裁剪码均为0,即三角形完全在视锥体内,无需裁剪,直接返回原始三角形
    // 如果三个顶点的裁剪码的与运算结果为非0,即三角形完全在视锥体外部,丢弃该三角形
    if((code[0] | code[1] | code[2]).none())
        return {tri};
    if((code[0] & code[1] & code[2]).any())
        return {};

    // 检查三角形是否与近裁剪平面相交
    if(((code[0] ^ code[1])[0]) || ((code[1] ^ code[2])[0]) || ((code[2] ^ code[0])[0]))
    {
        std::vector<Vertex> res;
        // 遍历三个边,对与近裁剪平面相交的边进行裁剪
        for(int i = 0; i < 3; ++i)
        {
            int k = (i + 1) % 3;

            if(!code[i][0] && !code[k][0])		// 边完全在内部
            {
                res.push_back(tri[k]);
            }
            else if(!code[i][0] && code[k][0])	// 边与近裁剪平面相交
            {
                float da = CalculateDistance(tri[i].clipSpacePos, viewPlanes[0]);
                float db = CalculateDistance(tri[k].clipSpacePos, viewPlanes[0]);
                float alpha = da / (da - db);
                Vertex np = CalculateInterpolation(tri[i], tri[k], alpha);
                res.push_back(np);
            }
            else if(code[i][0] && !code[k][0])	// 边与近裁剪平面相交
            {
                float da = CalculateDistance(tri[i].clipSpacePos, viewPlanes[0]);
                float db = CalculateDistance(tri[k].clipSpacePos, viewPlanes[0]);
                float alpha = da / (da - db);
                Vertex np = CalculateInterpolation(tri[i], tri[k], alpha);
                res.push_back(np);
                res.push_back(tri[k]);
            }
        }
        // 返回裁剪后的三角形
        return ConstructTriangle(res);
    }
    return std::vector<Triangle>{tri};
}


/********************************************************************************/
// 处理三角形，并根据渲染模式(TRIANGLE,LINE,POINT)选择相应的渲染方式
void TRendererDevice::ProcessTriangle(Triangle &tri)
{
    //对每个顶点应用顶点着色器
    for (int i = 0; i < 3; ++i)
    {
        shader->VertexShader(tri[i]);
    }

    //裁剪三角形
    std::vector<Triangle> completedTriangleList = ClipTriangle(tri);
    for (auto &ctri : completedTriangleList)
    {
        ExecutePerspectiveDivision(ctri);
        ConvertToScreen(ctri);
        if(renderMode == FACE) RasterizationTriangle(ctri);
        else if(renderMode == EDGE) WireframedTriangle(ctri);
        else if(renderMode == VERTEX) PointedTriangle(ctri);
    }
}


/********************************************************************************/
// 主渲染函数
// 对每个三角形进行顶点着色器处理、裁剪、透视除法、屏幕转换
// 然后根据渲染模式选择执行光栅化、线框化或顶点渲染
void TRendererDevice::Render()
{
    std::vector<Triangle> triangleList;
    for (int i = 0; i < indices.size(); i += 3)
    {
        assert(i + 1 < indices.size() && i + 2 < indices.size());
        triangleList.push_back({vertexList.at(indices.at(i)), vertexList.at(indices.at(i + 1)), vertexList.at(indices.at(i + 2))});
    }
    if(multiThread)
    {
        /********************************************************************************/
//        // tbb
//        tbb::parallel_for(tbb::blocked_range<size_t>(0, triangleList.size()),
//        [&](tbb::blocked_range<size_t> r)
//        {
//            for(size_t i = r.begin(); i < r.end(); ++i)
//                ProcessTriangle(triangleList[i]);
//        });


        /********************************************************************************/
//        // QtConcurrent
//        QtConcurrent::blockingMapped(triangleList, [](const Triangle& triangle) {
//            ProcessTriangle(triangle);
//        });

//        //c++ Thread
//        std::vector<std::thread> threads;
//        size_t numThreads = 12; // 要开启的线程数量

//        // 计算每个线程处理的元素数量
//        size_t elementsPerThread = triangleList.size() / numThreads;
//        size_t remainder = triangleList.size() % numThreads;

//        // 创建线程
//        size_t startIdx = 0;
//        for (size_t i = 0; i < numThreads; ++i) {
//            size_t threadElements = elementsPerThread + (i < remainder ? 1 : 0);
//            threads.emplace_back([&](size_t start, size_t end) {
//                for (size_t idx = start; idx < end; ++idx) {
//                    ProcessTriangle(triangleList[idx]);
//                }
//            }, startIdx, startIdx + threadElements);
//            startIdx += threadElements;
//        }

//        // 等待线程执行完毕
//        for (auto& thread : threads) {
//            thread.join();
//        }

        /********************************************************************************/

        #pragma omp parallel for
        for(size_t i = 0; i < triangleList.size(); ++i){
            ProcessTriangle(triangleList[i]);
        }
    }
    else
    {
        for(int i = 0; i < triangleList.size(); ++i)
            ProcessTriangle(triangleList[i]);
    }

}
