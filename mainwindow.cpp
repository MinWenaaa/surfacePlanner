#include <QTabBar>

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
    // connect(wrapper, &LMFWrapper::measurementArrived,
    //         this, &MainWindow::updateMeasurementArrived);
    connect(wrapper, &LMFWrapper::imageArrived,
            this, &MainWindow::updateImageArrived);
    connect(wrapper, &LMFWrapper::inclinationChanged,
            this, &MainWindow::updateInclinationChanged);
    connect(wrapper,&LMFWrapper::changeTab,
            this, &MainWindow::changeTab);

    calibrationPage = ui->calibration_page;
    measurementPage = ui->datameasurement_page;

    //ui->tabWidget->tabBar()->setEnabled(false);
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

void MainWindow::changeTab(int index) {
    ui->tabWidget->setCurrentIndex(index);
}
