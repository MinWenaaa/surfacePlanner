#ifndef CALIBRATIONPAGE_H
#define CALIBRATIONPAGE_H

#include <QWidget>
#include <QGraphicsScene>
#include <QtDataVisualization>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtWidgets/QWidget>
#include <QTextEdit>

#include "measurementfigure.h"


namespace Ui {
class CalibrationPage;
}

class CalibrationPage : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationPage(QWidget *parent = nullptr);
    ~CalibrationPage();

public slots:
    void receivePositionChange(double x, double y, double z);
    void receiveInclinationChange(double x, double y);
    void receiveSingleMeasurement(double x, double y, double z);

signals:
    void requestAnalysis(const QVector<QVector<QPointF>>& elevationData);

private slots:
    void on_connectionButton_clicked();
    void on_measureButton_clicked();
    void on_sampleMod_currentIndexChanged(int index);
    void on_refreshButton_clicked();
    void on_continueButton_clicked();

    void handleAnalysisResults(int dataset1, int dataset2, double maxDiff, double correlation);
    void handleAnalysisError(const QString& message);
    void onAnalysisFInished();

private:
    Ui::CalibrationPage *ui;
    measurementFigure *measureFig;
    QGraphicsScene *scene;
    QGraphicsEllipseItem *circle;      // 圆对象

    bool isMeasuring;
    int measureNum;

    int pointNum;

    QTextEdit* logEdit;

    void renderInclination(double, double);
    void appendLog(const QString& message, QColor color = QColor(33, 33, 33));
    void startAnalysis();
    void setupView();

};

#endif // CALIBRATIONPAGE_H
