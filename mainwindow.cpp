#include "mainwindow.h"
#include "ui_datameasurementpage.h"
#include "ui_mainwindow.h"
#include "lmfwrapper.h"
#include "include/TrackerWrapper.h"

//const char* address = "AT500Simulator";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto wrapper = &LMFWrapper::instance();
    connect(wrapper, &LMFWrapper::positionChanged,
            this, &MainWindow::updatePositionChanged);
    connect(wrapper, &LMFWrapper::measurementArrived,
            this, &MainWindow::updateMeasurementArrived);
    connect(wrapper, &LMFWrapper::imageArrived,
            this, &MainWindow::updateImageArrived);
    connect(wrapper, &LMFWrapper::inclinationChanged,
            this, &MainWindow::updateInclinationChanged);

    calibrationPage = ui->calibration_page;
    measurementPage = ui->datameasurement_page;
    measurementPage->ui->testLabel->setText("新的文本内容");
}

void MainWindow::updatePositionChanged(float, float, float) {

}

void MainWindow::updateMeasurementArrived(double, double, double) {

}

void MainWindow::updateImageArrived(const char*) {

}

void MainWindow::updateInclinationChanged(float, float, bool) {

}

MainWindow::~MainWindow()
{
    delete ui;
}

