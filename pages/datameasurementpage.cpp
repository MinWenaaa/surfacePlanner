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
    tempX(0), tempY(0), tempZ(0), measureComple(true), fileNum(0)
{
    ui->setupUi(this);
    imageLabel = ui->imageLabel;
    logEdit = ui->logEdit;
    measureButton = ui->measureButton;
    stopAContiButton = ui->stopAndContinueButton;

    stopAContiButton->setEnabled(false);
    ui->saveButton->setEnabled(false);

    convertPoint = new QStandardItemModel(this);
    initTableView();

    ui->tableView->setModel(convertPoint);

    measureFig = ui->measureFigure;
    measureFig->setUpView(1);

}


void DataMeasurementPage::initTableView() {

    convertPoint->setHorizontalHeaderLabels({"点名", "X", "Y", "Z"});

    convertPoint->setColumnCount(4);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);

}

void DataMeasurementPage::on_fileButton_clicked() {
    // 打开文件浏览器需要关闭摄像机避免界面卡死
    StopOverviewCamera();
    filePath = QFileDialog::getSaveFileName(
        nullptr,                       // 父窗口
        "新建工程文件",                  // 对话框标题
        "D:/Code/RTK-LarserBenchmark/",             // 默认目录(用户主目录)
        "所有文件 (*);;文本文件 (*.txt);;CSV文件 (*.csv)"  // 文件过滤器
        );

    if (filePath.isEmpty()) {
        qDebug() << "用户取消了文件选择";
        return;
    }

    ui->file_pathLabel->setText(filePath);
    fileNum = 0;

    StartOverviewCamera();
}

void DataMeasurementPage::on_addPointButton_clicked() {
    if(filePath.isEmpty()) return;

    QString name = ui->pointNameEdit->text();
    ui->pointNameEdit->setText("");
    int row = convertPoint->rowCount();
    convertPoint->insertRow(row);
    convertPoint->setItem(row, 0, new QStandardItem(name));
    ui->stationStatusLabel->setText("测量中");
    appendLog("公共点 " + name + " 开始测量...");

    auto wrapper = &LMFWrapper::instance();
    connect(wrapper, &LMFWrapper::stationaryMeasuremntArrived,
               this, &DataMeasurementPage::receiveStationaryData);

    setMeasurementProfile(0);
    emit stationaryMeasureSig();
}

void DataMeasurementPage::receiveStationaryData(double x, double y, double z) {
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
                   this, &DataMeasurementPage::receiveStationaryData);
    }
}

void DataMeasurementPage::addAverConnectionPoint(double x, double y, double z) {
    int row = convertPoint->rowCount() - 1;

    convertPoint->setItem(row, 1, new QStandardItem(QString::number(x)));
    convertPoint->setItem(row, 2, new QStandardItem(QString::number(y)));
    convertPoint->setItem(row, 3, new QStandardItem(QString::number(z)));

    ui->tableView->resizeColumnsToContents();
}

void DataMeasurementPage::receivePositionChange() {

}

void DataMeasurementPage::receiveSingleMeasurement(double x, double y, double z) {
    if(!isMeasuring) return;
    measureFig->addPoint(x, y, z);
    dataPoint.append(QVector3D(x, y, z));
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

    if(measureComple) {
        QString text = ui->seperationEdit->text();
        bool ok;
        double value = text.toDouble(&ok);
        setMeasurementProfile(ui->sampleMod->currentIndex()+1);
        if(ui->sampleMod->currentIndex()) setDistanceSeperartion(ok? value: 0.1);
        else setTimeSeperation(ok ? value : 100);
        startMeasurement();
        //LMFWrapper::instance().sendTestData();

        ui->measureStatusLabel->setText("测量中");
        isMeasuring = true; measureComple = false;
        ui->measureButton->setText("结束测量");
        appendLog("测量开始！");
        stopAContiButton->setEnabled(true);

        measureFig->addSerious("", QColor(67, 163, 239));

    } else {
        stopMeasurement();

        ui->measureStatusLabel->setText("");
        isMeasuring = false; measureComple = true;
        ui->measureButton->setText("开始测量");
        appendLog("测量结束！");
        stopAContiButton->setEnabled(false);
        measureButton->setEnabled(false);
        ui->saveButton->setEnabled(true);
    }
}

void DataMeasurementPage::on_stopAndContinueButton_clicked() {
    if(isMeasuring) {
        stopMeasurement();
        stopAContiButton->setText("继续");
        tempSaveEleData();
        isMeasuring = false;
        appendLog("测量暂停，数据已保存");
    } else {
        startMeasurement();
        stopAContiButton->setText("暂停");
        appendLog("测量继续");
        isMeasuring = true;
    }

}

void DataMeasurementPage::on_saveButton_clicked() {
    measureButton->setEnabled(true);
    measureFig->clearAllData();
    tempSaveEleData();
    ui->saveButton->setEnabled(false);
}

void DataMeasurementPage::tempSaveEleData() {
    if(filePath.isEmpty()) {
        QMessageBox::warning(this, "警告", "未建立工程");
        return;
    }
    QFile file(filePath+"_" + QString::number(fileNum) + ".p");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件");
        return;
    }

    QTextStream out(&file);
    for(int i =0; i<dataPoint.count();i++) {
        QVector3D *point = &dataPoint[i];
        out << QString::number(point->x()) << "," << QString::number(point->y()) << "," << QString::number(point->z()) <<"\n";
    }
    dataPoint.clear();
    file.close();

    fileNum++;
}

void DataMeasurementPage::receiveImageData(const char* bitmapData) {

    QImage image(reinterpret_cast<const uchar*>(bitmapData), 640, 480, QImage::Format_RGB888);
    QImage mirroredImage = image.mirrored(false, true);  // 水平不翻转(false)，垂直翻转(true)

    QPixmap pixmap = QPixmap::fromImage(mirroredImage);
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





