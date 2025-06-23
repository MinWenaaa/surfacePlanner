#include <QRandomGenerator>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCharts/QChartView>

#include "include/TrackerWrapper.h"
#include "datameasurementpage.h"
#include "ui_datameasurementpage.h"
#include "lmfwrapper.h"

DataMeasurementPage::DataMeasurementPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::DataMeasurementPage), tempConnectionPMeasNum(0), ConnectionPNum(0),
    tempX(0), tempY(0), tempZ(0)
{
    ui->setupUi(this);
    imageLabel = ui->imageLabel;
    logEdit = ui->logEdit;

    convertPoint = new QStandardItemModel(this);
    initTableView();

    ui->tableView->setModel(convertPoint);

    setupView();

    auto wrapper = &LMFWrapper::instance();
    connect(this, &DataMeasurementPage::stationaryMeasureSig,
            wrapper, &LMFWrapper::stationaryMeasurementThis);


}

void DataMeasurementPage::setupView() {


}

void DataMeasurementPage::initTableView() {

    convertPoint->setHorizontalHeaderLabels({"点名", "X", "Y", "Z"});

    convertPoint->setColumnCount(4);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);

}

void DataMeasurementPage::on_fileButton_clicked()
{
    filePath = QFileDialog::getSaveFileName(
        nullptr,                       // 父窗口
        "新建工程文件",                  // 对话框标题
        "D:/Code/RTK-LarserBenchmark/reslut/",             // 默认目录(用户主目录)
        "所有文件 (*);;文本文件 (*.txt);;CSV文件 (*.csv)"  // 文件过滤器
        );

    if (filePath.isEmpty()) {
        qDebug() << "用户取消了文件选择";
        return;
    }

    ui->file_pathLabel->setText(filePath);

    // StartOverviewCamera();
    //auto wrapper = &LMFWrapper::instance();
    // connect(wrapper, &LMFWrapper::imageArrived,
    //      this, &DataMeasurementPage::renderBitmap);
    // connect(wrapper, &LMFWrapper::singleMeasurementArrived,
    //      this, &DataMeasurementPage::addConnectionPoint);

}

void DataMeasurementPage::on_addPointButton_clicked() {
    QString name = ui->pointNameEdit->text();
    ui->pointNameEdit->setText("");
    int row = convertPoint->rowCount();
    convertPoint->insertRow(row);
    convertPoint->setItem(row, 0, new QStandardItem(name));
    ui->stationStatusLabel->setText("测量中");
    appendLog("公共点 " + name + " 开始测量...");

    auto wrapper = &LMFWrapper::instance();
    connect(wrapper, &LMFWrapper::stationaryMeasuremntArrived,
            this, &DataMeasurementPage::addConnectionPoint);
    setMeasurementProfile(0);
    emit stationaryMeasureSig();
}

void DataMeasurementPage::addConnectionPoint(double x, double y, double z) {
    if(tempConnectionPMeasNum<10) {
        tempX += x; tempY += y; tempZ += z;
        emit stationaryMeasureSig();
        tempConnectionPMeasNum++;
    }
    if(tempConnectionPMeasNum==9) {
        addAverConnectionPoint(tempX/10, tempY/10, tempZ/10);
        tempX = 0, tempY = 0, tempZ = 0;
        tempConnectionPMeasNum = 0;
        ui->stationStatusLabel->setText("");

        auto wrapper = &LMFWrapper::instance();
        disconnect(wrapper, &LMFWrapper::stationaryMeasuremntArrived,
                   this, &DataMeasurementPage::addConnectionPoint);
    }
}

void DataMeasurementPage::receivePositionChange() {

}

void DataMeasurementPage::receiveInclinationChange() {

}

void DataMeasurementPage::receiveSingleMeasurement() {

}


void DataMeasurementPage::addAverConnectionPoint(double x, double y, double z) {
    int row = convertPoint->rowCount() - 1;

    convertPoint->setItem(row, 1, new QStandardItem(QString::number(x)));
    convertPoint->setItem(row, 2, new QStandardItem(QString::number(y)));
    convertPoint->setItem(row, 3, new QStandardItem(QString::number(z)));

    ui->tableView->resizeColumnsToContents();
}

void DataMeasurementPage::on_removeConnectionButton_clicked() {
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if(selected.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要删除的行");
        return;
    }

    for(const QModelIndex &index : selected) {
        convertPoint->removeRow(index.row());
    }
}

void DataMeasurementPage::saveConnectionData() {
    if(filePath.isEmpty()) {
        QMessageBox::warning(this, "警告", "未建立工程");
        return;
    }

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件");
        return;
    }

    QTextStream out(&file);
    out << "CONNECTION POINT DATA\n";
    for(int row = 0; row < convertPoint->rowCount(); ++row) {
        out << convertPoint->item(row, 0)->text() << ","
            << convertPoint->item(row, 1)->text() << ","
            << convertPoint->item(row, 2)->text() << ","
            << convertPoint->item(row, 3)->text() << "\n";
    }
    file.close();
}

void DataMeasurementPage::on_sampleMod_currentIndexChanged(int index) {
    if(isMeasuring) return;

    setMeasurementProfile(index+1);
    ui->UnitLabel->setText(index ? "m": "ms");
}

void DataMeasurementPage::on_measureButton_clicked() {
    saveConnectionData();

    if(!isMeasuring) {

    } else {

    }
}

void DataMeasurementPage::renderBitmap(const char* bitmapData) {

    QImage image(reinterpret_cast<const uchar*>(bitmapData), 640, 480, QImage::Format_RGB888);

    QPixmap pixmap = QPixmap::fromImage(image);

    imageLabel->setPixmap(pixmap.scaled(imageLabel->size(),
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
}

void DataMeasurementPage::appendLog(const QString& message, QColor color) {
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss] ");

    QTextCharFormat format;
    format.setForeground(QBrush(color)); // 设置文字颜色

    QTextCursor cursor(logEdit->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(timestamp, format); // 插入带格式的时间戳
    cursor.insertText(message + "\n", format); // 插入带格式的日志消息

    logEdit->ensureCursorVisible();
}

DataMeasurementPage::~DataMeasurementPage()
{
    delete ui;
}



