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

    //ui->tabWidget->tabBar()->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeTab(int index) {
    ui->tabWidget->setCurrentIndex(index);

    auto wrapper = &LMFWrapper::instance();
    // switch(index) {
    // case 0:

    // case 1:

    // }
}
