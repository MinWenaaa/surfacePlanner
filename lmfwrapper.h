#ifndef LMFWRAPPER_H
#define LMFWRAPPER_H

#include <QObject>
#include <QThread>

enum profileName {
    Stationary,
    ContinuousTime,
    ContinuousDistance
};


class LMFWrapper : public QObject
{
    Q_OBJECT
public:
    explicit LMFWrapper(QObject *parent = nullptr)
        : QObject(parent) {}
    ~LMFWrapper();

    static LMFWrapper& instance() {
        static LMFWrapper inst;
        return inst;
    }


    bool connectTo(const char*);

    static void onPositionChanged(float x, float y, float z, void* userData);
    static void onMeasurementArrived(double x, double y, double z, int type, void* userData);
    static void onImageArrived(const char* data, void* userData);
    static void onInclinationChanged(float x, float y, bool flag, void* userData);

    void sendTestData();

public slots:
    void stationaryMeasurementThis();

private:
    void init();
    static QThread* workerThread();

    void handlePositionChanged(float x, float y, float z);
    void handleMeasurementArrived(double x, double y, double z, int type);
    void handleImageArrived(const char* data);
    void handleInclinationChanged(double x, double y, bool flag);

signals:
    void positionChanged(float, float, float);
    void singleMeasurementArrived(double, double, double);
    void stationaryMeasuremntArrived(double, double, double);
    void imageArrived(const char*);
    void inclinationChanged(float, float, bool);

    void changeTab(int index);


};

#endif // LMFWRAPPER_H
