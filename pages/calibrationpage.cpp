#include <cmath>

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QVBoxLayout>
#include <QtCharts/QValueAxis>  // 添加这行
#include <QtCharts/QLineSeries>  // 确保这行也存在
#include <QtCharts/QChartView>

#include "calibrationpage.h"
#include "ui_calibrationpage.h"
#include "include/TrackerWrapper.h"

const int bubble_r = 36;
//const char* address = "192.168.250.1";
const char* address = "AT500Simulator";

CalibrationPage::CalibrationPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CalibrationPage)
{
    ui->setupUi(this);
    view_3d = ui->view_3d;
    view_elevation = ui->view_elevation;
    view_plan = ui->view_plan;

    setupView();

    scene = new QGraphicsScene(this);
    scene->setSceneRect(-10, -10, 20, 20); // 设置坐标范围
    ui->Inclination_container->setStyleSheet("background: transparent; border: 0px;");
    ui->Inclination_container->setScene(scene);
    ui->Inclination_container->setRenderHint(QPainter::Antialiasing);
    ui->Inclination_container->scale(1, -1);

    circle = new QGraphicsEllipseItem(-bubble_r/2, -bubble_r/2, bubble_r, bubble_r);
    circle->setPen(QPen(Qt::green, 0));
    circle->setBrush(QBrush(Qt::green));
    scene->addItem(circle);

    auto crosshairH = scene->addLine(-10, 0, 10, 0, QPen(Qt::black, 0.5));
    auto crosshairV = scene->addLine(0, -10, 0, 10, QPen(Qt::black, 0.5));
    crosshairH->setZValue(1);
    crosshairV->setZValue(1);
}

void CalibrationPage::renderInclination(double x, double y) {
    if (!circle) return;

    // 根据偏差确定颜色
    QColor color = std::sqrt(x*x+y*y) > 10 ? Qt::red : Qt::green;

    circle->setRect(x - bubble_r/2, y - bubble_r/2, bubble_r, bubble_r);  // 半径为3
    circle->setPen(QPen(color, 0));
    circle->setBrush(QBrush(color));
}

void CalibrationPage::setupView() {

    QChart *chart = new QChart();

    // 设置坐标轴
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(0, 30);
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 2);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    m_elevationChartView = new QChartView(chart);
    m_elevationChartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout2 = new QVBoxLayout(view_elevation);
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->addWidget(m_elevationChartView);



    QChart *chart2 = new QChart();
    // 设置坐标轴
    QValueAxis *axisX2 = new QValueAxis();
    axisX2->setRange(0, 30);
    QValueAxis *axisY2 = new QValueAxis();
    axisY2->setRange(0, 2);

    chart2->addAxis(axisX2, Qt::AlignBottom);
    chart2->addAxis(axisY2, Qt::AlignLeft);

    // 创建图表视图
    m_planChartView = new QChartView(chart2);
    m_planChartView->setRenderHint(QPainter::Antialiasing);

    // // 替换原来的view_elevation
    QVBoxLayout *layout3 = new QVBoxLayout(view_3d);
    layout3->setContentsMargins(0, 0, 0, 0);
    layout3->addWidget(m_planChartView);



    m_scatter = new Q3DScatter();
    m_scatter->setAspectRatio(1.0);
    m_scatter->setHorizontalAspectRatio(1.0);
    m_scatter->scene()->activeCamera()->setZoomLevel(200);
    QWidget *container = QWidget::createWindowContainer(m_scatter);
    QVBoxLayout *layout = new QVBoxLayout(view_plan);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(container);

}

CalibrationPage::~CalibrationPage()
{
    delete ui;
}

void CalibrationPage::on_connectionButton_clicked() {
    TrackerConnect(address);
}

