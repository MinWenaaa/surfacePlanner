#ifndef MEASUREMENTFIGURE_H
#define MEASUREMENTFIGURE_H

#include <QWidget>
#include <QtDataVisualization>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>

const QColor colors[] = {QColor(249, 231, 167), QColor(239, 118, 123), QColor(67, 163, 239), QColor("#629433")};
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

private:
    Ui::measurementFigure *ui;

    void setUpView();

    QWidget *view_3d, *view_elevation, *view_plan;
    Q3DScatter *m_scatter;
    QChartView *m_elevationChartView;
    QChartView *m_planChartView;
    QChart *elevationChart, *planChart;
    QValueAxis *eleAxisX, *eleAxisY, *planAxisX, *planAxisY;

    QVector<QVector<QPointF>> m_elevationData;
    QList<QLineSeries*> m_elevationSeries;  // 高程图表系列
    QList<QScatterSeries*> m_planSeries;    // 平面图表系列
    QList<QScatter3DSeries*> m_3dSeries;    // 3D图表系列
    QLineSeries* tempElevationSerious;
    QScatterSeries* tempPlanSerious;
    QScatter3DSeries* temp3dSerious;

};

#endif // MEASUREMENTFIGURE_H
