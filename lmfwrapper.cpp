#include <QTimer>

#include "lmfwrapper.h"
#include "include/TrackerWrapper.h"

//const char* address = "192.168.250.1";
//const char* address = "AT500Simulator";

void LMFWrapper::init() {
    this->moveToThread(workerThread());
}

QThread* LMFWrapper::workerThread() {
    static QThread* thread = []{
        QThread* t = new QThread();
        t->start();
        return t;
    }();
    return thread;
}

bool LMFWrapper::connectTo(const char* address) {
    init();
    bool flag = TrackerConnect(address);

    if(flag){
        setOnPositionChangedCallback(&LMFWrapper::onPositionChanged, this);
        setOnMeasurementArrivedCallback(&LMFWrapper::onMeasurementArrived, this);
        setOnImageArrivedCallback(&LMFWrapper::onImageArrived, this);
        setInclinationChangedCallback(&LMFWrapper::onInclinationChanged, this);

        setMeasurementProfile(ContinuousTime);
        return true;
    }
    return false;
}

void LMFWrapper::onPositionChanged(float x, float y, float z, void* userData) {
    LMFWrapper* wrapper = static_cast<LMFWrapper*>(userData);
    QMetaObject::invokeMethod(wrapper, [=](){
        wrapper->handlePositionChanged(x, y, z);
    }, Qt::QueuedConnection);
}

void LMFWrapper::onMeasurementArrived(double x, double y, double z, int type, void* userData) {
    LMFWrapper* wrapper = static_cast<LMFWrapper*>(userData);
    QMetaObject::invokeMethod(wrapper, [=](){
        wrapper->handleMeasurementArrived(x/1000, y/1000, z, type);
    }, Qt::QueuedConnection);
}

void LMFWrapper::onImageArrived(const char* data, void* userData) {
    LMFWrapper* wrapper = static_cast<LMFWrapper*>(userData);
    QByteArray imageData(data, data ? strlen(data) : 0);
    QMetaObject::invokeMethod(wrapper, [=](){
        wrapper->handleImageArrived(imageData);
    }, Qt::QueuedConnection);
}

void LMFWrapper::onInclinationChanged(float x, float y, bool flag, void* userData) {
    LMFWrapper* wrapper = static_cast<LMFWrapper*>(userData);
    QMetaObject::invokeMethod(wrapper, [=](){
        wrapper->handleInclinationChanged(x, y, flag);
    }, Qt::QueuedConnection);
}

LMFWrapper::~LMFWrapper() {
    workerThread()->quit();
    workerThread()->wait();
}

void LMFWrapper::handlePositionChanged(float x, float y, float z) {
    emit positionChanged(x/1000, y/1000, z);
}

void LMFWrapper::handleMeasurementArrived(double x, double y, double z, int type) {
    if(type == 0) emit singleMeasurementArrived(x, y, z);
    if(type == 1) emit stationaryMeasuremntArrived(x, y, z);
}

void LMFWrapper::handleImageArrived(const QByteArray& data) {
    emit imageArrived(data);
}

void LMFWrapper::handleInclinationChanged(double x, double y, bool flag) {
    emit inclinationChanged(x, y, flag);
}

void LMFWrapper::sendTestData() {
    QTimer* timer = new QTimer(this);

    // 初始化坐标值
    static double x = 0.0;
    static double y = 0.0;
    const double z = 1.0;
    const double dx = 0.1;
    const double dy = 0.1;

    // 连接定时器信号
    connect(timer, &QTimer::timeout, this, [=]() mutable {

        emit singleMeasurementArrived(x, y, z);

        x += dx;
        if(x>10) {
            y+=dy;
            x = 0;
        }

        if (x > 10.0) {
            x = 0.0;
        }

        if (y > 3.0) {
            y = 0.0;
        }
    });

    // 启动定时器，100ms间隔
    timer->start(100);
}

void LMFWrapper::stationaryMeasurementThis() {
    QTimer::singleShot(500, this, [](){
        stationaryMeasurement();
    });
}
