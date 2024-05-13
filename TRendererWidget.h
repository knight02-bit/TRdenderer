#ifndef TRendererWIDGET_H
#define TRendererWIDGET_H

#include <QWidget>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QFile>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMessageBox>
#include "TRendererCore/TRendererDevice.h"
#include "TRendererCore/BlinnPhongShader.h"
#include "TRendererCore/Model.h"
#include "Camera/Camera.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define FIXED_CAMERA_FAR 100.f

namespace Ui {
class TRendererWidget;
}

class TRendererWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TRendererWidget(QWidget *parent = nullptr);
    ~TRendererWidget();
    void SetRenderColor(Color color, RenderColorType type);
    void SetLightColor(Color color, LightColorType type);
    void SetLightDir(Vector4D dir){TRendererDevice::GetInstance().shader->lightList[0].dir = dir;}
    void SetRenderMode(RenderMode mode){TRendererDevice::GetInstance().renderMode = mode;}
    void SetFaceCulling(bool val){TRendererDevice::GetInstance().faceCulling = val;}
    void SetMultiThread(bool val){TRendererDevice::GetInstance().multiThread = val;}
    void SaveImage(QString path){TRendererDevice::GetInstance().SaveImage(path);}
    void LoadModel(QString path);
    void InitDevice();
    Camera camera;
protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
signals:
    void SendModelData(int triangleCount, int vertexCount);
public slots:
    void Render();

private:
    int w;
    int h;
    QTimer timer;
    void ProcessInput();
    void ResetCamera();
    void InitUI();
    Ui::TRendererWidget *ui;
    Model* model;
    int meshNum;
};

#endif // TRendererWIDGET_H

