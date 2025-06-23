#ifndef MEASUREMENTFIGURE_H
#define MEASUREMENTFIGURE_H

#include <QWidget>
#include <QtDataVisualization>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>

const QColor backgroundColor = QColor("#f9f9f9");

namespace Ui {
class measurementFigure;
}

class measurementFigure : public QWidget
{
    Q_OBJECT

public:
    explicit measurementFigure(QWidget *parent = nullptr);
    ~measurementFigure();

    void addSerious(const QString& name, const QColor& color);
    void addPoint(double x, double y, double z);
    void clearAllData();
    void setUpView(int type);


    QVector<QVector<QPointF>>& getEleData();
private:
    Ui::measurementFigure *ui;
    int seriousNum, pointNum;
    double last_x, last_y, currentDistance;
    double max_x, min_x, max_y, min_y, max_z, min_z;

    QWidget *view_3d, *view_elevation, *view_plan;
    Q3DScatter *m_scatter;
    QChartView *m_elevationChartView;
    QChartView *m_planChartView;
    QChart *elevationChart, *planChart;
    QValueAxis *eleAxisX, *eleAxisY, *planAxisX, *planAxisY;

    QList<QLineSeries*> m_elevationSeries;  // 高程图表系列
    QList<QScatterSeries*> m_planSeries;    // 平面图表系列
    QList<QScatter3DSeries*> m_3dSeries;    // 3D图表系列
    QLineSeries* tempElevationSerious;
    QScatterSeries* tempPlanSerious;
    QScatter3DSeries* temp3dSerious;

    QVector<QVector<QPointF>> m_eleDataCache;

};

#endif // MEASUREMENTFIGURE_H
