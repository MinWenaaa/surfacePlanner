#define TRACKERWRAPPER_EXPORTS
#include "TrackerWrapper.h"
#include <vcclr.h>
#include <msclr/marshal_cppstd.h>
#include <string>
#include <iostream>

using namespace System;
using namespace LMF::Tracker;
using namespace LMF::Tracker::Measurements;
using namespace LMF::Tracker::Measurements::Profiles;
using namespace LMF::Tracker::MeasurementResults;
using namespace LMF::Tracker::Enums;
using namespace LMF::Tracker::ErrorHandling;
using namespace LMF::Tracker::Targets;

std::function<void(float, float, float)> TrackerWrapper::onPostionChangedCallback = nullptr;
std::function<void(double, double, double)> TrackerWrapper::onMeasurementArrivedCallback = nullptr;
std::function<void(char*)> TrackerWrapper::onImageArrivedCallback = nullptr;

// Internal managed class to hold tracker instances
ref class ManagedWrapper {
public:
	static LMF::Tracker::Tracker^ LMFTracker = nullptr;
	static LMF::Tracker::MeasurementResults::Measurement^ LastMeasurement = nullptr;
	static LMF::Tracker::Triggers::ProbeButtonTrigger^ ProbeTrigger = nullptr;
	static LMF::Tracker::MeasurementResults::SingleShotMeasurement3D^ newPosition = nullptr;
    static System::String^ LastError = "";

    // 影像数据流  
    static void OnImageArrived(LMF::Tracker::OVC::OverviewCamera^ sender, array<System::Byte>^% image, OVC::ATRCoordinateCollection^ atrcoordinates) {
        pin_ptr<Byte> pinnedImage = &image[0];
        uint8_t* nativeImage = reinterpret_cast<uint8_t*>(pinnedImage);
		TrackerWrapper::instance().onImageArrivedCallback(reinterpret_cast<char*>(nativeImage));
    }

    // 坐标数据流  
    static void OnTargetPostionChanged(LMF::Tracker::Tracker^ sender, LMF::Tracker::MeasurementResults::SingleShotMeasurement3D^ position) {
        float coord1 = position->Position->Coordinate1->Value;
        float coord2 = position->Position->Coordinate2->Value;
        float coord3 = position->Position->Coordinate3->Value;

		TrackerWrapper::instance().onPostionChangedCallback(coord1, coord2, coord3);

    }

    static void OnMeasurementArrived(MeasurementSettings^ sender, MeasurementCollection^ measurements, LmfException^ exception) {
        if (measurements != nullptr && measurements->Count > 0) {
            std::cout << "onMeasurementArrived!" << std::endl;
            MeasurementResults::Measurement^ LastMeasurement = nullptr;
            for each(auto measurement in measurements) {
                LastMeasurement = measurement;
                break; // 只取第一个
            }
            SingleShotMeasurement3D^ singleShotMeas3D = dynamic_cast<SingleShotMeasurement3D^>(LastMeasurement);
            if (singleShotMeas3D != nullptr) {
                // 处理测量数据  
                double coord1 = singleShotMeas3D->Position->Coordinate1->Value;
                double coord2 = singleShotMeas3D->Position->Coordinate2->Value;
                double coord3 = singleShotMeas3D->Position->Coordinate3->Value;
				TrackerWrapper::instance().onMeasurementArrivedCallback(coord1, coord2, coord3);
            }
            return;
        }
        else if (exception != nullptr) {
            LastError = exception->Description;
            return;
        }
		std::cout << "No measurements or exception!" << std::endl;
    }

    static void OnInclinationChanged(Inclination::InclinationMonitoring^ sender) {
        Inclination::InclinationMeasurement^ currentMeasurement = sender->Current;
        double x, y;
        bool flag = true;;
        if (currentMeasurement != nullptr) {
            x = currentMeasurement->X->Value; y = currentMeasurement->Y->Value;
        }
        else flag = false;

    }
};


// Helper to convert System::String to std::string
static std::string ManagedToStdString(System::String^ managedString) {
    if (managedString == nullptr) return "";
    msclr::interop::marshal_context ctx;
    return ctx.marshal_as<std::string>(managedString);
}

extern "C" {

    TRACKER_API bool TrackerConnect(const char* ipAddress) {
        try {
            if (ManagedWrapper::LMFTracker) {
                ManagedWrapper::LMFTracker->Disconnect();
            }

            Connection^ con = gcnew Connection();
            if (strcmp(ipAddress, "0.0.0.0") != 0) {
                ManagedWrapper::LMFTracker = con->Connect(gcnew System::String(ipAddress));
                return ManagedWrapper::LMFTracker != nullptr;
            }
            return false;
        }
        catch (System::Exception^ ex) {
            ManagedWrapper::LastError = ex->Message;
            return false;
        }
    }

    TRACKER_API void TrackerDisconnect() {
        try {
            if (ManagedWrapper::LMFTracker) {
                ManagedWrapper::LMFTracker->Disconnect();
                ManagedWrapper::LMFTracker = nullptr;
            }
        }
        catch (System::Exception^ ex) {
            //ManagedWrapper::LastError = ex->Message;
        }
    }

    TRACKER_API bool IsTrackerConnected() {
        return ManagedWrapper::LMFTracker != nullptr;
    }

    TRACKER_API bool StartOverviewCamera() {
        try {
            if (ManagedWrapper::LMFTracker && ManagedWrapper::LMFTracker->OverviewCamera) {
                ManagedWrapper::LMFTracker->OverviewCamera->StartAsync();
                return true;
            }
            return false;
        }
        catch (System::Exception^ ex) {
            //ManagedWrapper::LastError = ex->Message;
            return false;
        }
    }

    TRACKER_API bool setMeasurementProfile(const char* profileName) {
        for (int i = 0; i < ManagedWrapper::LMFTracker->Measurement->Profiles->Count; i++)
        {
            MeasurementProfile^ profile = ManagedWrapper::LMFTracker->Measurement->Profiles[i];


            std::string nativeStr = msclr::interop::marshal_as<std::string>(profile->Name);
            const char* cstr = nativeStr.c_str();

            if (!strcmp(profileName, cstr)) {
                // Select belonging Profile  
                profile->Select();
                return true;
            }

        }
        return false;
    }

    TRACKER_API bool setDistanceSeperartion(double seperation) {
        if (!ManagedWrapper::LMFTracker) return false;
        Measurements::MeasurementProfileCollection^ profiles = ManagedWrapper::LMFTracker->Measurement->Profiles;
        auto continuousProfile = nullptr;
        for each(auto profile in profiles) {
            if (ContinuousDistanceProfile^ distanceProfile = dynamic_cast<ContinuousDistanceProfile^>(profile)) {
                distanceProfile->DistanceSeparation->ValueInBaseUnits = seperation;
                return true;
            }
        }
        return false;
    }

    TRACKER_API bool setTimeSeperation(double seperation) {
        if (!ManagedWrapper::LMFTracker) return false;
        Measurements::MeasurementProfileCollection^ profiles = ManagedWrapper::LMFTracker->Measurement->Profiles;
        for each(auto profile in profiles) {
            if (ContinuousTimeProfile^ timeProfile = dynamic_cast<ContinuousTimeProfile^>(profile)) {
                timeProfile->TimeSeparation->Value = seperation;
                return true;
            }
        }
        return false;
    }

    TRACKER_API bool startMeasurement() {
        if (ManagedWrapper::LMFTracker->Measurement->Status->Value == EMeasurementStatus::ReadyToMeasure) {
            ManagedWrapper::LMFTracker->Measurement->StartMeasurement();
        }
        return true;
    }

    TRACKER_API bool stopMeasurement() {
        if (ManagedWrapper::LMFTracker == nullptr) return false;
        ManagedWrapper::LMFTracker->Measurement->StopMeasurement();
        return true;
    }

    // 回调注册
    TRACKER_API void setOnPositionChangedCallback(std::function<void(float, float, float)> callback) {
        TrackerWrapper::instance().onPostionChangedCallback = callback;
        ManagedWrapper::LMFTracker->Targets->TargetPositionChanged += gcnew LMF::Tracker::Targets::TargetCollection::TargetPositionChangedHandler(&ManagedWrapper::OnTargetPostionChanged);
    }

    TRACKER_API void setOnMeasurementArrivedCallback(std::function<void(double, double, double)> callback) {
        TrackerWrapper::instance().onMeasurementArrivedCallback = callback;
        ManagedWrapper::LMFTracker->Measurement->MeasurementArrived += gcnew LMF::Tracker::Measurements::MeasurementSettings::MeasurementArrivedHandler(&ManagedWrapper::OnMeasurementArrived);
    }

    TRACKER_API void setOnImageArrivedCallback(std::function<void(char*)> callback) {
        TrackerWrapper::instance().onImageArrivedCallback = callback;
        ManagedWrapper::LMFTracker->OverviewCamera->ImageArrived += gcnew LMF::Tracker::OVC::OverviewCamera::ImageArrivedHandler(&ManagedWrapper::OnImageArrived);
        ManagedWrapper::LMFTracker->OverviewCamera->StartAsync();

    }

    TRACKER_API void setInclinationChangedCallback(std::function<void(float, float, bool)> callback) {
        TrackerWrapper::instance().onPostionChangedCallback = callback;
        ManagedWrapper::LMFTracker->InclinationSensor->Monitoring->Interval = 0.5;
        ManagedWrapper::LMFTracker->InclinationSensor->Monitoring->InclinationChanged += gcnew LMF::Tracker::Inclination::InclinationMonitoring::InclinationChangedHandler(&ManagedWrapper::OnInclinationChanged);
    }
    }

    TRACKER_API bool StopOverviewCamera() {
        try {
            if (ManagedWrapper::LMFTracker && ManagedWrapper::LMFTracker->OverviewCamera) {
                ManagedWrapper::LMFTracker->OverviewCamera->Stop();
                return true;
            }
            return false;
        }
        catch (System::Exception^ ex) {
            //ManagedWrapper::LastError = ex->Message;
            return false;
        }
    }

    TRACKER_API bool GetLastMeasurement(double* x, double* y, double* z) {
        try {
            if (ManagedWrapper::LastMeasurement) {
                // TODO: Adjust based on actual measurement properties
                *x = 0.0; // TrackerManager::LastMeasurement->X;
                *y = 0.0; // TrackerManager::LastMeasurement->Y;
                *z = 0.0; // TrackerManager::LastMeasurement->Z;
                return true;
            }
            return false;
        }
        catch (System::Exception^ ex) {
            //ManagedWrapper::LastError = ex->Message;
            return false;
        }
    }

    TRACKER_API const char* GetTrackerLastError() {  // <- NOT GetLastError!
        static std::string errorBuffer;
        //errorBuffer = ManagedToStdString(ManagedWrapper::LastError);
        return errorBuffer.c_str();
    }
}