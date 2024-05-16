#include "Camera.h"
#include <QDebug>

// 绕目标点旋转摄像机
void Camera::RotateAroundTarget(Vector2D motion)
{
    Vector3D formTarget = position-target;

    float radius = glm::length(formTarget);

    // 俯仰角(垂直方向旋转角度)
    float pitch = (float)std::asin(formTarget.y / radius);
    // 偏航角(水平方向旋转角度)
    float yaw = (float)std::atan2(formTarget.x, formTarget.z);

    // 转换因子为2π
    float factor = (float)M_PI * 2.f;

    Vector3D offset;
    yaw -= motion.x * factor;
    pitch += motion.y * factor;

    // 限制俯仰角的范围避免翻转
    if(pitch + M_PI_2 < FLT_EPSILON) pitch = - glm::radians(89.9f);
    if(pitch - M_PI_2 > FLT_EPSILON) pitch = glm::radians(89.9f);

    // 根据偏航角和俯仰角更新相机位置
    offset.x = (radius * (float)std::cos(pitch) * (float)std::sin(yaw));
    offset.y = (radius * (float)std::sin(pitch));
    offset.z = (radius * (float)std::cos(pitch) * (float)std::cos(yaw));
    position = target + offset;
}

// 移动摄像机至目标点
void Camera::MoveTarget(Vector2D motion)
{
    Vector3D fromPosition = target-position;
    Vector3D forward = glm::normalize(fromPosition);
    Vector3D left = glm::normalize(glm::cross({0.0f,1.0f,0.0f},forward));
    Vector3D up = glm::normalize(glm::cross(forward,left));

    float distance = glm::length(fromPosition);
    float factor = distance * (float)tan(glm::radians(fov) / 2) * 2;

    // 计算水平和垂直移动的增量
    // 更新目标点和相机位置
    Vector3D deltaX = factor * aspect * motion.x * left;
    Vector3D deltaY = factor * motion.y * up;
    target += (deltaX+deltaY);
    position += (deltaX+deltaY);
}

// 靠近目标点
void Camera::CloseToTarget(int ratio)
{
    Vector3D formTarget = position-target;
    float radius = glm::length(formTarget);

    float pitch = (float)std::asin(formTarget.y / radius);
    float yaw = (float)std::atan2(formTarget.x, formTarget.z);

    Vector3D offset;
    radius *= (float)std::pow(0.95, ratio); // 缩放半径

    // 根据偏航角和俯仰角更新相机位置
    offset.x = (radius * (float)std::cos(pitch) * (float)std::sin(yaw));
    offset.y = (radius * (float)std::sin(pitch));
    offset.z = (radius * (float)std::cos(pitch) * (float)std::cos(yaw));
    position = target + offset;
}

// 设置模型中心和视角范围, 调整摄像机位置和目标点位置
void Camera::SetModel(Coord3D modelCentre, float yRange)
{
    target = modelCentre;
    position = modelCentre;
    // 计算相机位置的高度，确保在视野范围内
    position.z += (yRange / std::tan(glm::radians(fov) / 2));
}

// 创建观察矩阵(根据相机的位置、目标点、上方向向量)
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(position,target,{0.0f,1.0f,0.0f});
}

// 创建透视投影矩阵
glm::mat4 Camera::GetProjectionMatrix()
{
    return glm::perspective(glm::radians(fov), aspect, zNear, zFar);
}
