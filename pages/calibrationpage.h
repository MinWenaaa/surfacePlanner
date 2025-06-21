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
    void addPoint(double, double, double);

signals:
    void requestAnalysis(const QVector<QVector<QPointF>>& elevationData);

private slots:
    void on_connectionButton_clicked();
    void on_measureButton_clicked();
    void on_sampleMod_currentIndexChanged(int index);

    void handleAnalysisResults(int dataset1, int dataset2, double maxDiff, double correlation);
    void handleAnalysisError(const QString& message);
    void onAnalysisFInished();

private:
    Ui::CalibrationPage *ui;

    bool isMeasuring;
    int measureNum;

    QGraphicsScene *scene;
    QGraphicsEllipseItem *circle;      // 圆对象

    QWidget *view_3d, *view_elevation, *view_plan;
    Q3DScatter *m_scatter;
    QChartView *m_elevationChartView;
    QChartView *m_planChartView;
    QChart *elevationChart, *planChart;
    QValueAxis *eleAxisX, *eleAxisY, *planAxisX, *planAxisY;

    QVector<QVector<QPointF>> m_elevationData;
    QVector<QVector<QPointF>> m_planData;
    QVector<QVector<QVector3D>> m_3dData;
    QList<QLineSeries*> m_elevationSeries;  // 高程图表系列
    QList<QScatterSeries*> m_planSeries;    // 平面图表系列
    QList<QScatter3DSeries*> m_3dSeries;    // 3D图表系列
    QLineSeries* tempElevationSerious;
    QScatterSeries* tempPlanSerious;
    QScatter3DSeries* temp3dSerious;
    double last_x, last_y, currentDistance;

    int pointNum;

    QTextEdit* logEdit;

    void renderInclination(double, double);
    void setupView();
    void appendLog(const QString& message, QColor color = QColor(33, 33, 33));
    void startAnalysis();

};

#endif // CALIBRATIONPAGE_H
