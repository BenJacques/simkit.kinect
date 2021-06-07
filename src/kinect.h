#ifndef KINECT_H
#define KINECT_H
#include <k4a/k4a.h>
#include <string>
#include "settings.h"

namespace kinect
{
class Kinect
{
public:
    Kinect();
    ~Kinect();
    bool Connect(int32_t exposureValue);
    bool TrySaveCalibrationFile(const char* base_dir);
    int Run(int capture_frame_count, DATA_DIRS_T &data_dirs);
    void Stop();

private:

    bool m_streaming;

    k4a_device_t m_device;
    k4a_capture_t m_capture;
    k4a_device_configuration_t m_deviceConfig;

    void save_color_image(k4a_image_t image, std::string dir, std::string frame_s);
    void save_depth_or_ir_image(k4a_image_t image, std::string dir, std::string frame_s);
};
} // end namespace Kinect
#endif