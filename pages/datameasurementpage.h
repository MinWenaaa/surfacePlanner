#ifndef DATAMEASUREMENTPAGE_H
#define DATAMEASUREMENTPAGE_H

#include <QWidget>
#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QLibrary>
#include <QtDataVisualization>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>


class QPushButton;
class QLabel;
class QTextEdit;


namespace Ui {
class DataMeasurementPage;
}

class DataMeasurementPage : public QWidget
{
    Q_OBJECT

public:
    explicit DataMeasurementPage(QWidget *parent = nullptr);
    ~DataMeasurementPage();

    Ui::DataMeasurementPage *ui;

public slots:
    void receivePositionChange();
    void receiveInclinationChange();
    void receiveSingleMeasurement();

signals:
    void stationaryMeasureSig();

private slots:
    void on_addPointButton_clicked();
    void on_fileButton_clicked();
    void renderBitmap(const char* bitmapData);
    void addConnectionPoint(double x, double y, double z);
    void addAverConnectionPoint(double x, double y, double z);        // 添加点

    void on_removeConnectionButton_clicked();
    void on_measureButton_clicked();
    void on_sampleMod_currentIndexChanged(int index);

private:

    QStandardItemModel *convertPoint;

    QWidget *view_3d, *view_elevation, *view_plan;
    Q3DScatter *m_scatter;
    QChartView *m_elevationChartView;
    QChartView *m_planChartView;
    QChart *elevationChart, *planChart;
    QWidget *m_scatterContainer;
    QList<QLineSeries*> m_elevationSeries;  // 高程图表系列
    QList<QScatterSeries*> m_planSeries;    // 平面图表系列
    QList<QScatter3DSeries*> m_3dSeries;    // 3D图表系列
    QLineSeries* tempElevationSerious;
    QScatterSeries* tempPlanSerious;
    QScatter3DSeries* temp3dSerious;
    QValueAxis *eleAxisX, *eleAxisY, *planAxisX, *planAxisY;
    double last_x, last_y, currentDistance;
    double max_x, min_x, max_y, min_y, max_z, min_z;

    double tempX, tempY, tempZ;
    int tempConnectionPMeasNum, ConnectionPNum;
    QString filePath;
    bool isMeasuring;

    QLabel *imageLabel;
    QTextEdit* logEdit;

    void setupView();
    void initTableView();   // 初始化表格
    void saveConnectionData();
    void appendLog(const QString& message, QColor color = QColor(33, 33, 33));

};

#endif // DATAMEASUREMENTPAGE_H
