#pragma once

#ifdef TRACKERWRAPPER_EXPORTS
#define TRACKER_API __declspec(dllexport)
#else
#define TRACKER_API __declspec(dllimport)
#endif

#include <functional>
typedef void (*PositionChangedCallback)(float, float, float, void*);
typedef void (*MeasurementArrivedCallback)(double, double, double, void*);
typedef void (*ImageArrivedCallback)(const char*, void*);
typedef void (*InclinationChangedCallback)(float, float, bool, void*);

class TrackerWrapper {
public:
    static TrackerWrapper& instance() {
        static TrackerWrapper ins;
        return ins;
    }
    static PositionChangedCallback onPostionChangedCallback;
    static MeasurementArrivedCallback onMeasurementArrivedCallback;
    static ImageArrivedCallback onImageArrivedCallback;
    static InclinationChangedCallback onInclinationChangedCallback;

    static void* positionChangedUserData;
    static void* measurementArrivedUserData;
    static void* imageArrivedUserData;
    static void* inclinationChangedUserData;

private:
    TrackerWrapper() = default;
    ~TrackerWrapper() = default;
    TrackerWrapper(const TrackerWrapper& other) = delete;
    TrackerWrapper& operator=(const TrackerWrapper& other) = delete;
};

extern "C" {
    // Connection management
    TRACKER_API bool TrackerConnect(const char* ipAddress);
    TRACKER_API void TrackerDisconnect();
    TRACKER_API bool IsTrackerConnected();

    //         
    TRACKER_API bool setMeasurementProfile(const char* profileName);
    TRACKER_API bool setDistanceSeperartion(double seoeration);
    TRACKER_API bool setTimeSeperation(double seperation);

    TRACKER_API bool startMeasurement();
    TRACKER_API bool stopMeasurement();

    //  ص ע  
    TRACKER_API void setOnPositionChangedCallback(PositionChangedCallback callback, void* userData = nullptr);
    TRACKER_API void setOnMeasurementArrivedCallback(MeasurementArrivedCallback callback, void* userData = nullptr);
    TRACKER_API void setOnImageArrivedCallback(ImageArrivedCallback callback, void* userData = nullptr);
    TRACKER_API void setInclinationChangedCallback(InclinationChangedCallback callback, void* userData = nullptr);


    // Camera control
    TRACKER_API bool StartOverviewCamera();
    TRACKER_API bool StopOverviewCamera();

    // Error handling - MUST BE RENAMED!
    TRACKER_API const char* GetTrackerLastError();  // NOT GetLastError!
}