# TRenderer - 一个软光栅渲染器



## 开发环境

+ Qt 5.14.2
+ assimp-5.2.5
+ glm-1.0.1



## 重点

+ 最近邻采样纹理过滤
+ Bresenham 画线算法
+ Cohen-Sutherland算法(线段裁剪)
+ 面剔除：Sutherland-Hodgman 算法(齐次空间裁剪)
+ Blinn-Phong着色
+ Half-Space 三角形光栅化算法
+ openmp多线程优化



## 后续待优化

+ 使用MipMap优化着色部分(可通过双线性插值对相邻 Mipmap 级别进行插值)