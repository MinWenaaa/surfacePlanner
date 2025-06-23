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

#include "measurementfigure.h"


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
    void renderBitmap(const char* bitmapData);

signals:
    void stationaryMeasureSig();

private slots:
    void on_addPointButton_clicked();
    void on_fileButton_clicked();
    void addConnectionPoint(double x, double y, double z);
    void addAverConnectionPoint(double x, double y, double z);        // 添加点

    void on_removeConnectionButton_clicked();
    void on_measureButton_clicked();
    void on_sampleMod_currentIndexChanged(int index);

    void on_pushButton_clicked();

private:

    QStandardItemModel *convertPoint;

    measurementFigure *measureFig;
    double last_x, last_y, currentDistance;
    double max_x, min_x, max_y, min_y, max_z, min_z;

    double tempX, tempY, tempZ;
    int tempConnectionPMeasNum, ConnectionPNum;
    QString filePath;
    bool isMeasuring;

    QLabel *imageLabel;
    QTextEdit* logEdit;

    void initTableView();   // 初始化表格
    void saveConnectionData();
    void appendLog(const QString& message, QColor color = QColor(33, 33, 33));

};

#endif // DATAMEASUREMENTPAGE_H
