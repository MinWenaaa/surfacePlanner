#ifndef CALIBRATIONPAGE_H
#define CALIBRATIONPAGE_H

#include <QWidget>
#include <QGraphicsScene>
#include <QtDataVisualization>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtWidgets/QWidget>

namespace Ui {
class CalibrationPage;
}

class CalibrationPage : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationPage(QWidget *parent = nullptr);
    ~CalibrationPage();

private slots:
    void on_connectionButton_clicked();

private:
    Ui::CalibrationPage *ui;

    QGraphicsScene *scene;
    QGraphicsEllipseItem *circle;      // 圆对象

    QWidget *view_3d, *view_elevation, *view_plan;
    Q3DScatter *m_scatter;
    QChartView *m_elevationChartView;
    QChartView *m_planChartView;

    void renderInclination(double, double);

    void setupView();

};

#endif // CALIBRATIONPAGE_H
