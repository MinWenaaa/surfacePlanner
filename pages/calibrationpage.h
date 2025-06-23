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
    void on_refreshButton_clicked();
    void on_continueButton_clicked();

    void handleAnalysisResults(int dataset1, int dataset2, double maxDiff, double correlation);
    void handleAnalysisError(const QString& message);
    void onAnalysisFInished();

private:
    Ui::CalibrationPage *ui;

    bool isMeasuring;
    int measureNum;

    QGraphicsScene *scene;
    QGraphicsEllipseItem *circle;      // 圆对象

    double last_x, last_y, currentDistance;
    double max_x, min_x, max_y, min_y, max_z, min_z;

    int pointNum;

    QTextEdit* logEdit;

    void renderInclination(double, double);
    void setupView();
    void appendLog(const QString& message, QColor color = QColor(33, 33, 33));
    void startAnalysis();

};

#endif // CALIBRATIONPAGE_H
