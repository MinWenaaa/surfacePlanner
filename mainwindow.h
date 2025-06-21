#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pages/datameasurementpage.h"
#include "pages/calibrationpage.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    DataMeasurementPage *measurementPage;
    CalibrationPage *calibrationPage;

    void updatePositionChanged(float, float, float);
    void updateMeasurementArrived(double, double, double);
    void updateImageArrived(const char*);
    void updateInclinationChanged(float, float, bool);

private slots:
    void changeTab(int index);
};
#endif // MAINWINDOW_H
