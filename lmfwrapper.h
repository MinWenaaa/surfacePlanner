#ifndef LMFWRAPPER_H
#define LMFWRAPPER_H

#include <QObject>

class LMFWrapper : public QObject
{
    Q_OBJECT
public:
    explicit LMFWrapper(QObject *parent = nullptr);

    static LMFWrapper& instance() {
        static LMFWrapper inst;
        return inst;
    }

signals:
    void onPositionChanged(float, float, float);
    void newMeasurement(double, double, double);
    void onImageArrived(const char*);
    void onInclinationChanged(double, double, bool);
};

void onPositionChanged(float x, float y, float z);
void onMeasurementArrived(double x, double y, double z);
void onImageArrived(const char*);
void onInclinationChanged(double, double, bool);

#endif // LMFWRAPPER_H
