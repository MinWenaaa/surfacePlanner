#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/TrackerWrapper.h"

const char* address = "AT500Simulator";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    calibrationPage = new CalibrationPage(this);
    ui->tabWidget->addTab(calibrationPage, "仪器检校");
    calibrationPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    measurementPage = new DataMeasurementPage(this);
    ui->tabWidget->addTab(measurementPage, "数据测量");
    measurementPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

MainWindow::~MainWindow()
{
    delete ui;
}

