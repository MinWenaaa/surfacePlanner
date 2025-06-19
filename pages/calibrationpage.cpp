#include <cmath>

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>

#include "calibrationpage.h"
#include "ui_calibrationpage.h"

const int bubble_r = 36;

CalibrationPage::CalibrationPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CalibrationPage)
{
    ui->setupUi(this);

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

CalibrationPage::~CalibrationPage()
{
    delete ui;
}
