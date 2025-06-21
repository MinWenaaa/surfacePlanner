#include <cmath>

#include <QString>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QVBoxLayout>
#include <QtCharts/QLineSeries>  // 确保这行也存在
#include <QtCharts/QChartView>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/Q3DTheme>

#include "calibrationpage.h"
#include "ui_calibrationpage.h"
#include "include/TrackerWrapper.h"
#include "tools/elevationanalyzer.h"
#include "lmfwrapper.h"

const int bubble_r = 36;
//const char* address = "192.168.250.1";
const char* address = "AT500Simulator";

const QColor colors[] = {QColor(249, 231, 167), QColor(239, 118, 123), QColor(67, 163, 239)};
const QColor backgroundColor = QColor("#f9f9f9");

CalibrationPage::CalibrationPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::CalibrationPage), isMeasuring(false), measureNum(0),
    last_x(0), last_y(0), currentDistance(0), pointNum(0)
{
    ui->setupUi(this);
    view_3d = ui->view_3d;
    view_elevation = ui->view_elevation;
    view_plan = ui->view_plan;
    logEdit = ui->logEdit;
    logEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 隐藏垂直滚动条
    logEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏水平滚动条

    auto wrapper = &LMFWrapper::instance();
    connect(wrapper, &LMFWrapper::measurementArrived,
                this, &CalibrationPage::addPoint);

    ui->refreshButton->setEnabled(false);
    ui->continueButton->setEnabled(false);

    setupView();
}

CalibrationPage::~CalibrationPage()
{
    delete ui;
}

void CalibrationPage::on_connectionButton_clicked() {
    LMFWrapper::instance().connectTo(address);
}


void CalibrationPage::on_measureButton_clicked() {
    if(!isMeasuring) {
        tempElevationSerious = m_elevationSeries[measureNum];
        tempPlanSerious = m_planSeries[measureNum];
        temp3dSerious = m_3dSeries[measureNum];
        planChart->addSeries(tempPlanSerious);
        elevationChart->addSeries(tempElevationSerious);
        m_scatter->addSeries(temp3dSerious);
        tempPlanSerious->attachAxis(planAxisX);  // 平面图绑定平面图的X轴
        tempPlanSerious->attachAxis(planAxisY);  // 平面图绑定平面图的Y轴
        tempElevationSerious->attachAxis(eleAxisX);  // 高程图绑定高程图的X轴
        tempElevationSerious->attachAxis(eleAxisY);  // 高程图绑定高程图的Y轴
        pointNum = 0; currentDistance = 0;

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
        stopMeasurement();

        ui->measureStatusLabel->setText("");
        isMeasuring = false;
        ui->measureButton->setText("开始测量");
        measureNum++;
        appendLog("第 " + QString::number(measureNum) + " 次测量结束", colors[measureNum-1]);

        if(measureNum==3) {
            appendLog("已完成全部测量，等待生成报告...");
            ui->measureButton->setEnabled(false);
            ui->refreshButton->setEnabled(true);
            startAnalysis();
        }
    }

}


void CalibrationPage::on_sampleMod_currentIndexChanged(int index) {
    if(isMeasuring) return;

    setMeasurementProfile(index+1);
    ui->UnitLabel->setText(index ? "m": "ms");

}

void CalibrationPage::addPoint(double x, double y, double z ) {
    if(measureNum>2) return;
    if(pointNum){
        currentDistance += std::sqrt((x-last_x)*(x-last_x) + (y-last_y) *(y-last_y));
        if(measureNum==0) eleAxisX->setRange(0, currentDistance + 1);
    } else if(measureNum==0){
        eleAxisY->setRange(0, z + 1);
    }
    pointNum++;
    last_x = x; last_y = y;
    QPointF elevationPoint(currentDistance, z);
    QPointF planPoint(x, y);
    // QPointF elevationPoint(pointNum, z);
    // QPointF planPoint(pointNum, pointNum);
    QVector3D threeDPoint(x, y, z);

    m_elevationData[measureNum].append(elevationPoint);
    m_planData[measureNum].append(planPoint);
    m_3dData[measureNum].append(threeDPoint);
    tempElevationSerious->append(elevationPoint);
    tempPlanSerious->append(planPoint);
    temp3dSerious->dataProxy()->addItem(threeDPoint);

    planAxisX->setRange(x - 1, x + 1); // 根据数据调整
    planAxisY->setRange(y - 1, y + 1);

    // 刷新图表
    planChart->update();
    elevationChart->update();

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

    for (int i = 0; i < 3; ++i) {
        m_elevationData.append(QVector<QPointF>());
        m_planData.append(QVector<QPointF>());
        m_3dData.append(QVector<QVector3D>());
    }

    elevationChart = new QChart();
    eleAxisX = new QValueAxis();
    eleAxisX->setRange(0, 30);
    eleAxisY = new QValueAxis();
    eleAxisY->setRange(0, 2);
    elevationChart->addAxis(eleAxisX, Qt::AlignBottom);
    elevationChart->addAxis(eleAxisY, Qt::AlignLeft);

    m_elevationChartView = new QChartView(elevationChart);
    m_elevationChartView->setRenderHint(QPainter::Antialiasing);
    m_elevationChartView->setStyleSheet("background: transparent; border: none;");
    elevationChart->setBackgroundBrush(Qt::transparent);

    QVBoxLayout *layout2 = new QVBoxLayout(view_elevation);
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->addWidget(m_elevationChartView);

    for (int i = 0; i < 3; ++i) {
        QLineSeries *series = new QLineSeries();
        series->setName(QString("重复测量%1").arg(i+1));
        series->setColor(colors[i]);
        m_elevationSeries.append(series);
    }




    planChart = new QChart();
    planAxisX = new QValueAxis();
    planAxisX->setRange(0, 30);
    planAxisY = new QValueAxis();
    planAxisY->setRange(0, 2);
    planChart->addAxis(planAxisX, Qt::AlignBottom);
    planChart->addAxis(planAxisY, Qt::AlignLeft);

    m_planChartView = new QChartView(planChart);
    m_planChartView->setRenderHint(QPainter::Antialiasing);
    m_planChartView->setStyleSheet("background: transparent; border: none;");
    planChart->setBackgroundBrush(Qt::transparent);

    QVBoxLayout *layout3 = new QVBoxLayout(view_3d);
    layout3->setContentsMargins(0, 0, 0, 0);
    layout3->addWidget(m_planChartView);

    for (int i = 0; i < 3; ++i) {
        QScatterSeries *series = new QScatterSeries();
        series->setName(QString("重复测量%1").arg(i+1));
        series->setColor(colors[i]);
        series->setMarkerSize(8);
        m_planSeries.append(series);
    }



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

    for (int i = 0; i < 3; ++i) {
        QScatter3DSeries *series = new QScatter3DSeries();
        series->setMesh(QAbstract3DSeries::MeshPoint);
        series->setItemSize(0.1f);
        series->setBaseColor(colors[i]);
        m_3dSeries.append(series);
    }



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

    QTextCharFormat format;
    format.setForeground(QBrush(color)); // 设置文字颜色

    QTextCursor cursor(logEdit->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(timestamp, format); // 插入带格式的时间戳
    cursor.insertText(message + "\n", format); // 插入带格式的日志消息

    logEdit->ensureCursorVisible();
}

void CalibrationPage::startAnalysis() {
    QThread* workerThread = new QThread(this);
    ElevationAnalyzer* analyzer = new ElevationAnalyzer();

    // 将分析器移动到新线程
    analyzer->moveToThread(workerThread);

    // 连接信号槽
    connect(workerThread, &QThread::finished, analyzer, &QObject::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);

    connect(this, &CalibrationPage::requestAnalysis, analyzer, &ElevationAnalyzer::analyzeElevationData);
    connect(analyzer, &ElevationAnalyzer::analysisComplete,
            this, &CalibrationPage::handleAnalysisResults);
    connect(analyzer, &ElevationAnalyzer::errorOccurred,
            this, &CalibrationPage::handleAnalysisError);
    connect(analyzer, &ElevationAnalyzer::allFinished,
            this, &CalibrationPage::onAnalysisFInished);

    // 启动线程
    workerThread->start();

    ui->label_num->setText("测量序号");
    ui->label_maxDiff->setText("最大高差");
    ui->label_correlation->setText("相关系数");

    emit requestAnalysis(m_elevationData);
}

void CalibrationPage::handleAnalysisResults(int dataset1, int dataset2, double maxDiff, double correlation) {
    QString current = ui->label_num->text();
    ui->label_num->setText(current + '\n' + QString("%1").arg(QString::number(dataset1) + " & " + QString::number(dataset2), 12, ' '));
    current = ui->label_maxDiff->text();
    ui->label_maxDiff->setText(current + '\n' + QString::number(maxDiff));
    current = ui->label_correlation->text();
    ui->label_correlation->setText(current + '\n' + QString::number(correlation));
}
void CalibrationPage::handleAnalysisError(const QString& message) {
    appendLog("处理错误："+message);
}

void CalibrationPage::onAnalysisFInished() {
    ui->continueButton->setEnabled(true);
}
