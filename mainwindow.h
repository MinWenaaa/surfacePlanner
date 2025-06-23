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

private slots:
    void changeTab(int index);
};
#endif // MAINWINDOW_H
