#ifndef KINECT_H
#define KINECT_H
#include <k4a/k4a.h>
#include <string>
#include "settings.h"
#include <thread>
#include <atomic>

namespace kinect
{
class Kinect
{

public:
    Kinect();
    ~Kinect();

    bool Connect(int32_t exposureValue, settings::Settings &curr_settings);
    bool TrySaveCalibrationFile(const char* base_dir);
    int Run(int capture_frame_count);
    std::thread RunThread(int capture_frame_count);
    void Stop();
    bool GetRecentTemparature(double &last_temp);

private:
    //std::atomic<bool> m_streaming;
    std::atomic <bool> m_streaming;
    std::atomic <double> m_lastTemp;

    k4a_device_t m_device;
    k4a_capture_t m_capture;
    k4a_device_configuration_t m_deviceConfig;
    settings::DATA_DIRS_T m_data_dirs;

    void save_color_image(k4a_image_t image, std::string dir, std::string frame_s);
    void save_depth_or_ir_image(k4a_image_t image, std::string dir, std::string frame_s);
};
} // end namespace Kinect
#endif