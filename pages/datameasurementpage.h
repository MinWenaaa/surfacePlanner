#ifndef DATAMEASUREMENTPAGE_H
#define DATAMEASUREMENTPAGE_H

#include <QWidget>
#include <QtDataVisualization>

namespace Ui {
class DataMeasurementPage;
}

class DataMeasurementPage : public QWidget
{
    Q_OBJECT

public:
    explicit DataMeasurementPage(QWidget *parent = nullptr);
    ~DataMeasurementPage();

private:
    Ui::DataMeasurementPage *ui;
    Q3DScatter *m_scatter;  // 3D 散点图
    QWidget *m_scatterContainer;

    void generateRandomData();
};

#endif // DATAMEASUREMENTPAGE_H
