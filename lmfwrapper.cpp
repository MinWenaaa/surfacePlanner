#include "lmfwrapper.h"

LMFWrapper::LMFWrapper(QObject *parent)
    : QObject{parent}
{}

void onPositionChanged(float x, float y, float z) {
    emit LMFWrapper::instance().onPositionChanged(x, y, z);
}

void onMeasurementArrived(double x, double y, double z) {
    emit LMFWrapper::instance().newMeasurement(x, y, z);
}

void onImageArrived(const char* data) {
    emit LMFWrapper::instance().onImageArrived(data);
}

void onInclinationChanged(double x, double y, bool flag) {
    emit LMFWrapper::instance().onInclinationChanged(x, y, flag);
}
