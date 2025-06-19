#include <QRandomGenerator>
#include "datameasurementpage.h"
#include "ui_datameasurementpage.h"

DataMeasurementPage::DataMeasurementPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataMeasurementPage)
{
    ui->setupUi(this);

    m_scatter = new Q3DScatter();

    // 2. 创建容器并嵌入到 horizontalLayout
    m_scatterContainer = QWidget::createWindowContainer(m_scatter);
    //ui->verticalLayout_2->addWidget(QTextLayout("Test"));
    ui->verticalLayout_2->addWidget(m_scatterContainer);  // 添加到水平布局

    // 3. 设置 3D 散点图基本属性
    m_scatter->setAxisX(new QValue3DAxis);
    m_scatter->setAxisY(new QValue3DAxis);
    m_scatter->setAxisZ(new QValue3DAxis);
    m_scatter->axisX()->setRange(-10, 10);
    m_scatter->axisY()->setRange(-5, 5);
    m_scatter->axisZ()->setRange(-10, 10);

    m_scatter->setOrthoProjection(false); // 确保是透视投影（默认就是false）
    m_scatter->setShadowQuality(QAbstract3DGraph::ShadowQualityNone); // 阴影质量（可选）

    // 旋转、缩放、平移的全局开关
    m_scatter->setSelectionMode(QAbstract3DGraph::SelectionNone); // 选择模式
    //m_scatter->setCameraPreset(Q3DCamera::CameraPresetFront); // 初始视角

    // 启用各种交互（这些是默认启用的）
    m_scatter->scene()->activeCamera()->setCameraPosition(0, 0, 30);
    // 4. 生成随机数据
    generateRandomData();
}

void ::DataMeasurementPage::generateRandomData()
{
    QScatter3DSeries *series = new QScatter3DSeries;
    QScatterDataArray data;

    // 生成 100 个随机点
    for (int i = 0; i < 100; ++i) {
        float x = QRandomGenerator::global()->generateDouble() * 10 - 5;
        float y = QRandomGenerator::global()->generateDouble() * 10 - 5;;
        float z = QRandomGenerator::global()->generateDouble() * 10 - 5;;
        data << QVector3D(x, y, z);
    }

    series->dataProxy()->addItems(data);
    m_scatter->addSeries(series);

    // 可选：设置散点颜色和大小
    series->setBaseColor(Qt::cyan);
    series->setItemSize(0.1f);
}

DataMeasurementPage::~DataMeasurementPage()
{
    delete ui;
}
