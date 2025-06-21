#ifndef CALIBRATIONPAGE_H
#define CALIBRATIONPAGE_H

#include <QWidget>
#include <QGraphicsScene>
#include <QtDataVisualization>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
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

private slots:
    void on_connectionButton_clicked();

    void on_measureButton_clicked();

    void on_sampleMod_currentIndexChanged(int index);

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

    QTextEdit* logEdit;

    void renderInclination(double, double);
    void setupView();
    void appendLog(const QString& message, QColor color = QColor(33, 33, 33));

};

#endif // CALIBRATIONPAGE_H
