#ifndef CAMERA_H
#define CAMERA_H

#include "../TRendererCore/BasicDataStructure.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum CameraPara {FOV,NEAR};

class Camera
{
public:
    float aspect;           // 视口的宽高比
    Vector3D position;      // 摄像机位置
    Vector3D target;        // 目标位置
    float zNear;            // 近裁剪面距离
    float zFar;             // 远裁剪面距离
    float fov;              // 摄像机视野角度

    Camera(float _aspect, float far):aspect(_aspect),zFar(far){}
    void RotateAroundTarget(Vector2D motion);
    void MoveTarget(Vector2D motion);
    void CloseToTarget(int ratio);
    void SetModel(Coord3D modelCentre, float yRange);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
};

#endif // CAMERA_H
