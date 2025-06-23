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

    calibrationPage = ui->calibration_page;
    measurementPage = ui->datameasurement_page;

    auto wrapper = &LMFWrapper::instance();
    connect(wrapper, &LMFWrapper::changeTab, this, &MainWindow::changeTab);

    changeTab(0);
    //ui->tabWidget->tabBar()->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeTab(int index) {
    ui->tabWidget->setCurrentIndex(index);

    auto wrapper = &LMFWrapper::instance();
    switch(index) {
    case 0:
        connect(wrapper, &LMFWrapper::inclinationChanged, this->calibrationPage, &CalibrationPage::receiveInclinationChange);
        connect(wrapper, &LMFWrapper::singleMeasurementArrived, this->calibrationPage, &CalibrationPage::receiveSingleMeasurement);
        connect(wrapper, &LMFWrapper::positionChanged, this->calibrationPage, &CalibrationPage::receivePositionChange);
    case 1:
        connect(wrapper, &LMFWrapper::inclinationChanged, this->measurementPage, &DataMeasurementPage::receiveInclinationChange);
        connect(wrapper, &LMFWrapper::singleMeasurementArrived, this->measurementPage, &DataMeasurementPage::receiveSingleMeasurement);
        connect(wrapper, &LMFWrapper::positionChanged, this->measurementPage, &DataMeasurementPage::receivePositionChange);
    }
}
