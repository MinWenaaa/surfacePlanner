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
const QColor colors[] = {QColor(249, 231, 167), QColor(239, 118, 123), QColor(67, 163, 239), QColor("#629433")};

CalibrationPage::CalibrationPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::CalibrationPage), isMeasuring(false), measureNum(0)
{
    ui->setupUi(this);
    logEdit = ui->logEdit;
    logEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 隐藏垂直滚动条
    logEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏水平滚动条

    measureFig = ui->measureFigure;
    ui->refreshButton->setEnabled(false);
    ui->continueButton->setEnabled(false);
    ui->info_container->hide();

    setupView();

}

CalibrationPage::~CalibrationPage()
{
    delete ui;
}

void CalibrationPage::setupView() {


    scene = new QGraphicsScene(this);
    scene->setSceneRect(-10, -10, 20, 20); // 设置坐标范围
    ui->Inclination_container->setStyleSheet("background: transparent; border: 0px;");
    ui->Inclination_container->setScene(scene);
    ui->Inclination_container->setRenderHint(QPainter::Antialiasing);
    ui->Inclination_container->scale(1, -1);

    QGraphicsEllipseItem* outerCircle = new QGraphicsEllipseItem(-bubble_r*1.5, -bubble_r*1.5, bubble_r*3, bubble_r*3);
    outerCircle->setPen(QPen(Qt::black, 0.5));
    outerCircle->setBrush(Qt::NoBrush);
    scene->addItem(outerCircle);
    QGraphicsEllipseItem* innerCircle = new QGraphicsEllipseItem(-bubble_r*0.8, -bubble_r*0.8, bubble_r*1.6, bubble_r*1.6);
    innerCircle->setPen(QPen(Qt::black, 0.5));
    innerCircle->setBrush(Qt::NoBrush);
    scene->addItem(innerCircle);

    circle = new QGraphicsEllipseItem(-bubble_r/2, -bubble_r/2, bubble_r, bubble_r);
    circle->setPen(QPen(colors[3], 0));
    circle->setBrush(QBrush(colors[3]));
    scene->addItem(circle);

    auto crosshairH = scene->addLine(-bubble_r*1.5, 0, bubble_r*1.5, 0, QPen(Qt::black, 0.5));
    auto crosshairV = scene->addLine(0, -bubble_r*1.5, 0, bubble_r*1.5, QPen(Qt::black, 0.5));
    crosshairH->setZValue(1);
    crosshairV->setZValue(1);

}

void CalibrationPage::receivePositionChange(double x, double y, double z) {

}
void CalibrationPage::receiveInclinationChange(double x, double y) {

}
void CalibrationPage::receiveSingleMeasurement(double x, double y, double z) {
    measureFig->addPoint(x, y, z);
}


void CalibrationPage::on_connectionButton_clicked() {
    LMFWrapper::instance().connectTo(address);
    ui->info_container->show();
    ui->statusLabel->setText("连接成功");
    ui->adressLabel->setText(address);
    ui->nameLabel->setText("AT500");
}


void CalibrationPage::on_measureButton_clicked() {
    if(!isMeasuring) {
        pointNum = 0;
        QString text = ui->seperationEdit->text();
        bool ok;
        double value = text.toDouble(&ok);
        if(ui->sampleMod->currentIndex()) setDistanceSeperartion(ok? value: 0.1);
        else setTimeSeperation(ok ? value : 100);
        startMeasurement();
        //LMFWrapper::instance().sendTestData();

        ui->measureStatusLabel->setText("测量中");
        isMeasuring = true;
        ui->measureButton->setText("结束测量");
        appendLog("开始：第 " + QString::number(measureNum+1) + " 次测量", colors[measureNum]);

        measureFig->addSerious("重复观测" + QString::number(measureNum+1), colors[measureNum]);

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

void CalibrationPage::on_refreshButton_clicked() {
    appendLog("数据清空");
    ui->continueButton->setEnabled(false);
}

void CalibrationPage::renderInclination(double x, double y) {
    if (!circle) return;

    QColor color = std::sqrt(x*x+y*y) > 10 ? colors[1] : colors[3];

    circle->setRect(x - bubble_r/2, y - bubble_r/2, bubble_r, bubble_r);  // 半径为3
    circle->setPen(QPen(color, 0));
    circle->setBrush(QBrush(color));

    ui->inclination_x->setText("x: "+QString::number(x));
    ui->inclination_y->setText("y: "+QString::number(y));
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

    emit requestAnalysis(measureFig->getEleData());

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
    appendLog("处理完成");
}


void CalibrationPage::on_continueButton_clicked() {
    emit LMFWrapper::instance().changeTab(1);
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

