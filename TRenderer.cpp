#include "TRenderer.h"
#include "ui_TRenderer.h"

#include <QTextStream>

static inline QString GenerateStyleSheet(QColor color)
{
    return "background-color: rgb("
           + QString::number(static_cast<int>(color.red())) + ','
           + QString::number(static_cast<int>(color.green())) + ','
           + QString::number(static_cast<int>(color.blue())) + ");";
}

TRenderer::TRenderer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TRenderer)
{
    ui->setupUi(this);
    InitUI();
    InitSignalAndSlot();
}

TRenderer::~TRenderer()
{
    delete ui;
}

// 设置界面的样式, 包括背景颜色、线条颜色、点颜色等
void TRenderer::SetStyle(Style style)
{
    QFile f;
    if(style == LIGHT)
    {
        f.setFileName(":/qdarkstyle/light/style.qss");
        ui->RenderWidget->SetRenderColor({0.98f, 0.98f, 0.98f}, BACKGROUND);
        ui->RenderWidget->SetRenderColor({0.098f, 0.137f, 0.176f}, LINE);
        ui->RenderWidget->SetRenderColor({0.098f, 0.137f, 0.176f}, POINT);
        ui->actionLight->setChecked(true);
        ui->actionDark->setChecked(false);
    }
    else
    {
        f.setFileName(":/qdarkstyle/dark/style.qss");
        ui->RenderWidget->SetRenderColor({0.098f, 0.137f, 0.176f}, BACKGROUND);
        ui->RenderWidget->SetRenderColor({0.98f, 0.98f, 0.98f}, LINE);
        ui->RenderWidget->SetRenderColor({0.98f, 0.98f, 0.98f}, POINT);
        ui->actionDark->setChecked(true);
        ui->actionLight->setChecked(false);
    }
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    qApp->setStyleSheet(ts.readAll());
}

// 设置渲染选项, 多线程渲染和面剔除
void TRenderer::SetOption(Option option, bool val)
{
    if(option == MUTITHREAD)
    {
        ui->actionMultiThread->setChecked(val);
        ui->RenderWidget->SetMultiThread(val);
    }
    else
    {
        ui->actionFaceCulling->setChecked(val);
        ui->RenderWidget->SetFaceCulling(val);
    }
}

// 设置光照的颜色, 包括环境光、漫反射光和镜面光
void TRenderer::SetLightColor(LightColorType type, QColor color)
{
    switch(type)
    {
    case SPECULAR:
        ui->SpecularColor->setStyleSheet(GenerateStyleSheet(color));
        specularColor = color;
        break;
    case DIFFUSE:
        ui->DiffuseColor->setStyleSheet(GenerateStyleSheet(color));
        diffuseColor = color;
        break;
    case AMBIENT:
        ui->AmbientColor->setStyleSheet(GenerateStyleSheet(color));
        ambientColor = color;
        break;
    }

    // 更新对应的UI控件样式
    ui->RenderWidget->SetLightColor({color.red() / 255.f,
                                     color.green() / 255.f,
                                     color.blue() / 255.f}, type);
}

// 设置相机的参数, 包括视角FOV和近裁剪平面距离
void TRenderer::SetCameraPara(CameraPara para, float val)
{
    if(para == FOV)
    {
        ui->FovLabel->setText(QString::number(static_cast<int>(val)));
        ui->RenderWidget->camera.fov = val;
    }
    else
    {
        ui->NearLabel->setText(QString::number(val, 'f', 1));
        ui->RenderWidget->camera.zNear = val;
    }
}

// 计算光照的方向
void TRenderer::SetLightDir()
{
    Vector3D lightDir;

    // 俯仰角
    float pitch = glm::radians(glm::clamp(static_cast<float>(ui->PitchSlider->value()), -89.9f, 89.9f));

    // 偏航角
    float yaw = -glm::radians(static_cast<float>(ui->YawDial->value()));

    // 俯仰角和偏航角
    lightDir.x = (1.f * (float)std::cos(pitch) * (float)std::sin(yaw));
    lightDir.y = (1.f * (float)std::sin(pitch));
    lightDir.z = (1.f * (float)std::cos(pitch) * (float)std::cos(yaw));

    // 设置w分量为0来确保在进行平移变换时, 方向向量不会发生偏移, 仅影响旋转和缩放
    ui->RenderWidget->SetLightDir(Vector4D(-lightDir, 0.f));
}

void TRenderer::on_actionLight_triggered()
{
    SetStyle(LIGHT);
}


void TRenderer::on_actionDark_triggered()
{
    SetStyle(DARK);
}

void TRenderer::on_actionopen_file_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Model File", "", "OBJ(*.obj)");
    if(!filePath.isEmpty())
        ui->RenderWidget->LoadModel(filePath);
}

void TRenderer::on_actionsave_image_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "PNG(*.png)");
    if(!fileName.isEmpty())
        ui->RenderWidget->SaveImage(fileName);
}

// 初始化渲染器参数
void TRenderer::InitUI()
{
    setFixedSize(1120,672);
    ui->Setting->setTabText(0, "model");
    ui->Setting->setTabText(1, "light");

    SetStyle(LIGHT);
    SetOption(MUTITHREAD,true);
    SetOption(FACECULLING,true);
    SetCameraPara(FOV, 60.f);
    SetCameraPara(NEAR, 1.0f);

    SetLightColor(AMBIENT, QColor(102, 102, 102));
    SetLightColor(DIFFUSE, QColor(153, 153, 153));
    SetLightColor(SPECULAR, QColor(255, 255, 255));

    SetLightDir();
    QColorDialog::setCustomColor(0,QColor(255, 255, 255));
    QColorDialog::setCustomColor(2,QColor(153, 153, 153));
    QColorDialog::setCustomColor(4,QColor(102, 102, 102));

    ui->VertexCheckBox->setChecked(true);
}

void TRenderer::InitSignalAndSlot()
{
    connect(ui->RenderWidget, &TRendererWidget::SendModelData,this,
            [this](int triangleCount, int vertexCount)
            {
                ui->TriangleNumberLabel->setText(QString::number(triangleCount));
                ui->VertexNumberLabel->setText(QString::number(vertexCount));
            });
}

void TRenderer::on_LineCheckBox_clicked()
{
    if(ui->LineCheckBox->isChecked())
    {
        if(ui->VertexCheckBox->isChecked())
            ui->VertexCheckBox->setChecked(false);
        ui->RenderWidget->SetRenderMode(EDGE);
    }
    else
    {
        ui->RenderWidget->SetRenderMode(FACE);
    }
}

void TRenderer::on_VertexCheckBox_clicked()
{
    if(ui->VertexCheckBox->isChecked())
    {
        if(ui->LineCheckBox->isChecked())
            ui->LineCheckBox->setChecked(false);
        ui->RenderWidget->SetRenderMode(VERTEX);
    }
    else
    {
        ui->RenderWidget->SetRenderMode(FACE);
    }
}

void TRenderer::on_actionMultiThread_triggered()
{
    SetOption(MUTITHREAD, ui->actionMultiThread->isChecked());
}


void TRenderer::on_actionFaceCulling_triggered()
{
    SetOption(FACECULLING, ui->actionFaceCulling->isChecked());
}


void TRenderer::on_FovSilder_valueChanged(int value)
{
    SetCameraPara(FOV, static_cast<float>(value));
}

void TRenderer::on_NearSilder_valueChanged(int value)
{
    SetCameraPara(NEAR, value / 10.f);
}

void TRenderer::on_SpecularColorSet_clicked()
{
    QColor color = QColorDialog::getColor(specularColor,this,"Select Specular Color");
    if(color.isValid())
        SetLightColor(SPECULAR, color);
}

void TRenderer::on_DiffuseColorSet_clicked()
{
    QColor color = QColorDialog::getColor(diffuseColor,this,"Select Diffuse Color");
    if(color.isValid())
        SetLightColor(DIFFUSE, color);
}

void TRenderer::on_AmbientColorSet_clicked()
{
    QColor color = QColorDialog::getColor(ambientColor,this,"Select Ambient Color");
    if(color.isValid())
        SetLightColor(AMBIENT, color);
}

void TRenderer::on_PitchSlider_valueChanged(int value)
{
    SetLightDir();
}

void TRenderer::on_YawDial_valueChanged(int value)
{
    SetLightDir();
}
