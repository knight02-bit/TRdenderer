#include "TRendererWidget.h"
#include "./ui_TRendererWidget.h"
#include <QDebug>
#include <chrono>
#include <TRendererCore/TRendererDevice.h>

int lastFrameTime;  // 上一帧的时间戳
int deltaTime;      // 两帧之间的时间差
QPoint lastPos;     // 两帧之间的时间差
QPoint currentPos;  // 当前鼠标位置
int ratio;          // 滚轮缩放比例
Qt::MouseButtons currentBtns;   // 当前鼠标按键状态
glm::mat4 ModelMatrix;          // 模型变换矩阵

// 初始化相机参数和渲染设备
TRendererWidget::TRendererWidget(QWidget *parent) :
    QWidget(parent),camera((float)DEFAULT_WIDTH/DEFAULT_HEIGHT, FIXED_CAMERA_FAR),
    w(DEFAULT_WIDTH),h(DEFAULT_HEIGHT),
    ui(new Ui::TRendererWidget),model(nullptr)
{
    ui->setupUi(this);
    setFixedSize(w, h);

    ui->FPSLabel->setStyleSheet("background:transparent");
    ui->FPSLabel->setVisible(false);
    InitDevice();

    // 渲染频率
    connect(&timer, &QTimer::timeout, this,&TRendererWidget::Render);
    timer.start(1);
}

TRendererWidget::~TRendererWidget()
{
    delete ui;
    delete model;
}

void TRendererWidget::ResetCamera()
{
    ui->FPSLabel->setVisible(true);
    camera.SetModel(model->centre,model->GetYRange());

    // 初始为单位矩阵
    ModelMatrix = glm::mat4(1.0f);
}

void TRendererWidget::SetRenderColor(Color color, RenderColorType type)
{
    switch(type)
    {
    case BACKGROUND:
        TRendererDevice::GetInstance().clearColor = color;
        break;
    case LINE:
        TRendererDevice::GetInstance().lineColor = color;
        break;
    case POINT:
        TRendererDevice::GetInstance().pointColor = color;
    }
}

void TRendererWidget::SetLightColor(Color color, LightColorType type)
{
    switch(type)
    {
    case DIFFUSE:
        TRendererDevice::GetInstance().shader->lightList[0].diffuse = color;
        break;
    case SPECULAR:
        TRendererDevice::GetInstance().shader->lightList[0].specular = color;
        break;
    case AMBIENT:
        TRendererDevice::GetInstance().shader->lightList[0].ambient = color;
        break;
    }
}

void TRendererWidget::LoadModel(QString path)
{
    Model *newModel = new Model(path);
    if(!newModel->loadSuccess)
    {
        QMessageBox::critical(this,"Error","Model loading error!");
        delete newModel;
        return;
    }
    if(model != nullptr)
        delete model;
    model = newModel;
    meshNum = model->meshNum;

    emit SendModelData(model->triangleCount, model->vertexCount);
    ResetCamera();
}

void TRendererWidget::InitDevice()
{
    TRendererDevice::Init(w,h);
    TRendererDevice::GetInstance().shader = std::make_unique<BlinnPhongShader>();
    TRendererDevice::GetInstance().shader->lightList.push_back(Light());
}

void TRendererWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, TRendererDevice::GetInstance().GetBuffer());
}

void TRendererWidget::mousePressEvent(QMouseEvent *event)
{
    currentBtns = event->buttons();
    currentPos = event->pos();
    lastPos = {0,0};
}

void TRendererWidget::mouseReleaseEvent(QMouseEvent *event)
{
    currentBtns = event->buttons();
}

void TRendererWidget::mouseMoveEvent(QMouseEvent *event)
{
    currentPos = event->pos();
}

void TRendererWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    QPoint res;
    if (!numPixels.isNull()) {
        res = numPixels;
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        res = numSteps;
    }
    ratio += res.y();
}


void TRendererWidget::ProcessInput()
{
    if((currentBtns & Qt::LeftButton) || (currentBtns & Qt::RightButton))
    {
        if(!lastPos.isNull())
        {
            Vector2D motion = {(float)(currentPos - lastPos).x(),(float)(currentPos - lastPos).y()};
            // 将鼠标移动距离转换为相对窗口宽度和高度的比例
            motion.x = (motion.x / w);
            motion.y = (motion.y / h);

            // 左键被按下, 相机围绕目标点旋转
            if(currentBtns & Qt::LeftButton)
            {
                camera.RotateAroundTarget(motion);
            }
            // 右键被按下, 相机移动目标点
            if(currentBtns & Qt::RightButton)
            {
                camera.MoveTarget(motion);
            }
        }
        lastPos = currentPos;
    }
    if(ratio != 0)
    {
        // 调整相机的近/远平面距离
        camera.CloseToTarget(ratio);
        ratio = 0;
    }
}

void TRendererWidget::Render()
{
    TRendererDevice::GetInstance().ClearBuffer();
    if(model == nullptr) return;

    int nowTime = QTime::currentTime().msecsSinceStartOfDay();
    if(lastFrameTime != 0)
    {
        deltaTime = nowTime - lastFrameTime;
        ui->FPSLabel->setText(QStringLiteral("FPS : ")+QString::number(1000.0 / deltaTime, 'f', 0));
    }
    lastFrameTime = nowTime;
    ProcessInput();

    TRendererDevice::GetInstance().shader->Model = ModelMatrix;
    TRendererDevice::GetInstance().shader->View = camera.GetViewMatrix();
    TRendererDevice::GetInstance().shader->Projection = camera.GetProjectionMatrix();
    TRendererDevice::GetInstance().shader->eyePos = camera.position;
    TRendererDevice::GetInstance().shader->material.shininess = 150.f;

    model->Draw();
    update();
}
