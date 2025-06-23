#ifndef DATAMEASUREMENTPAGE_H
#define DATAMEASUREMENTPAGE_H

#include <QWidget>
#include <QMainWindow>
#include <QLibrary>
#include <QtDataVisualization>
#include <QTableView>
#include <QStandardItemModel>

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

private:

    QStandardItemModel *convertPoint;

    Q3DScatter *m_scatter;  // 3D 散点图
    QWidget *m_scatterContainer;
    QLabel *imageLabel;

    double tempX, tempY, tempZ;
    int tempConnectionPMeasNum, ConnectionPNum;
    QString filePath;

    QTextEdit* logEdit;

    void setupView();
    void initTableView();   // 初始化表格
    void saveConnectionData();
    void appendLog(const QString& message, QColor color = QColor(33, 33, 33));

};

#endif // DATAMEASUREMENTPAGE_H
