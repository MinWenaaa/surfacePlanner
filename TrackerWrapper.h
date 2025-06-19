#pragma once

#ifdef TRACKERWRAPPER_EXPORTS
#define TRACKER_API __declspec(dllexport)
#else
#define TRACKER_API __declspec(dllimport)
#endif

#include <functional>

class TrackerWrapper {
public:
    static TrackerWrapper& instance() {
        static TrackerWrapper ins;
        return ins;
    }
    static std::function<void(float, float, float)> onPostionChangedCallback;
    static std::function<void(double, double, double)> onMeasurementArrivedCallback;
    static std::function<void(char*)> onImageArrivedCallback;
	static std::function<void(float, float, bool)> onInclinationChangedCallback;

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

    // ÒÇÆ÷ÉèÖÃ
    TRACKER_API bool setMeasurementProfile(const char* profileName);
    TRACKER_API bool setDistanceSeperartion(double seoeration);
    TRACKER_API bool setTimeSeperation(double seperation);

    TRACKER_API bool startMeasurement();
    TRACKER_API bool stopMeasurement();

    // »Øµ÷×¢²á
    TRACKER_API void setOnPositionChangedCallback(std::function<void(float, float, float)> callback);
    TRACKER_API void setOnMeasurementArrivedCallback(std::function<void(double, double, double)> callback);
    TRACKER_API void setOnImageArrivedCallback(std::function<void(char*)> callback);
	TRACKER_API void setInclinationChangedCallback(std::function<void(float, float, bool)> callback);


    // Camera control
    TRACKER_API bool StartOverviewCamera();
    TRACKER_API bool StopOverviewCamera();

    // Error handling - MUST BE RENAMED!
    TRACKER_API const char* GetTrackerLastError();  // NOT GetLastError!
}