#include <cmath>

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QVBoxLayout>
#include <QtCharts/QValueAxis>  // 添加这行
#include <QtCharts/QLineSeries>  // 确保这行也存在
#include <QtCharts/QChartView>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/Q3DTheme>

#include "calibrationpage.h"
#include "ui_calibrationpage.h"
#include "include/TrackerWrapper.h"

const int bubble_r = 36;
//const char* address = "192.168.250.1";
const char* address = "AT500Simulator";

const QColor colors[] = {QColor(249, 231, 167), QColor(239, 118, 123), QColor(67, 163, 239)};
const QColor backgroundColor = QColor("#f9f9f9");

CalibrationPage::CalibrationPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::CalibrationPage), isMeasuring(false), measureNum(0)
{
    ui->setupUi(this);
    view_3d = ui->view_3d;
    view_elevation = ui->view_elevation;
    view_plan = ui->view_plan;
    logEdit = ui->logEdit;
    logEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 隐藏垂直滚动条
    logEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏水平滚动条

    setupView();
}

CalibrationPage::~CalibrationPage()
{
    delete ui;
}

void CalibrationPage::on_connectionButton_clicked() {
    TrackerConnect(address);
}


void CalibrationPage::on_measureButton_clicked() {
    if(!isMeasuring) {
        QString text = ui->seperationEdit->text();
        bool ok;
        double value = text.toDouble(&ok);
        if(ui->sampleMod->currentIndex()) setDistanceSeperartion(ok? value: 0.1);
        else setTimeSeperation(ok ? value : 100);
        startMeasurement();
        ui->measureStatusLabel->setText("测量中");
        isMeasuring = true;
        ui->measureButton->setText("结束测量");
        appendLog("开始：第 " + QString::number(measureNum+1) + " 次测量", colors[measureNum]);

    } else {
        ui->measureStatusLabel->setText("");
        isMeasuring = false;
        ui->measureButton->setText("开始测量");
        measureNum++;
        appendLog("第 " + QString::number(measureNum) + " 次测量结束", colors[measureNum-1]);
        if(measureNum==3) {
            appendLog("已完成全部测量，等待生成报告...");
        }
    }

}


void CalibrationPage::on_sampleMod_currentIndexChanged(int index) {
    if(isMeasuring) return;

    setMeasurementProfile(index+1);
    ui->UnitLabel->setText(index ? "m": "ms");

}

void CalibrationPage::renderInclination(double x, double y) {
    if (!circle) return;

    // 根据偏差确定颜色
    QColor color = std::sqrt(x*x+y*y) > 10 ? Qt::red : Qt::green;

    circle->setRect(x - bubble_r/2, y - bubble_r/2, bubble_r, bubble_r);  // 半径为3
    circle->setPen(QPen(color, 0));
    circle->setBrush(QBrush(color));

    ui->inclination_x->setText("x: "+QString::number(x));
    ui->inclination_y->setText("y: "+QString::number(y));
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
    m_elevationChartView->setStyleSheet("background: transparent; border: none;");
    m_elevationChartView->setRenderHint(QPainter::Antialiasing);
    chart->setBackgroundBrush(Qt::transparent);

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
    m_planChartView->setStyleSheet("background: transparent; border: none;");
    m_planChartView->setRenderHint(QPainter::Antialiasing);
    chart2->setBackgroundBrush(Qt::transparent);

    // // 替换原来的view_elevation
    QVBoxLayout *layout3 = new QVBoxLayout(view_3d);
    layout3->setContentsMargins(0, 0, 0, 0);
    layout3->addWidget(m_planChartView);



    m_scatter = new Q3DScatter();
    m_scatter->setAspectRatio(1.0);
    m_scatter->setHorizontalAspectRatio(1.0);
    m_scatter->scene()->activeCamera()->setZoomLevel(200);
    QWidget *container = QWidget::createWindowContainer(m_scatter);
    container->setAttribute(Qt::WA_TranslucentBackground);
    container->setStyleSheet("background: transparent; border: none;");
    QVBoxLayout *layout = new QVBoxLayout(view_plan);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(container);
    //m_scatter->setBackgroundColor(Qt::transparent);  // 主背景透明

    Q3DTheme *theme = m_scatter->activeTheme();
    theme->setGridEnabled(false);
    theme->setLabelBorderEnabled(false);  // 启用坐标轴边框
    theme->setBackgroundEnabled(false);  // 禁用主题背景
    theme->setBackgroundColor(Qt::transparent);  // 主题背景透明
    theme->setWindowColor(backgroundColor);


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

void CalibrationPage::appendLog(const QString& message, QColor color) {
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss] ");

    // 方法1：使用 QTextCharFormat（推荐）
    QTextCharFormat format;
    format.setForeground(QBrush(color)); // 设置文字颜色

    QTextCursor cursor(logEdit->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(timestamp, format); // 插入带格式的时间戳
    cursor.insertText(message + "\n", format); // 插入带格式的日志消息

    logEdit->ensureCursorVisible();
}
