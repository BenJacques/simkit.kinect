#include "kinect.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>

using namespace kinect;

Kinect::Kinect()
{
   
}

Kinect::~Kinect()
{
   
}

bool Kinect::Connect(int32_t exposureValue, DATA_DIRS_T &data_dirs){
    uint32_t device_count = k4a_device_get_installed_count();
    m_data_dirs = data_dirs;
    if (device_count == 0)
    {
        printf("No K4A devices found\n");
        return false;
    }

    if (K4A_RESULT_SUCCEEDED != k4a_device_open(K4A_DEVICE_DEFAULT, &m_device))
    {
        printf("Failed to open device\n");
        return false;
    }

    // Configure the device for ideal streaming parameters.
    // TODO: Change this to a config file later.

    m_deviceConfig.color_format = K4A_IMAGE_FORMAT_COLOR_MJPG;
    m_deviceConfig.color_resolution = K4A_COLOR_RESOLUTION_2160P;
    //config.color_resolution = K4A_COLOR_RESOLUTION_3072P;
    m_deviceConfig.depth_mode = K4A_DEPTH_MODE_WFOV_2X2BINNED;
    m_deviceConfig.camera_fps = K4A_FRAMES_PER_SECOND_30;


    if (K4A_RESULT_SUCCEEDED != k4a_device_start_cameras(m_device, &m_deviceConfig))
    {
        printf("Failed to start device\n");
        return false;
    }

    if (exposureValue > 0)
    {
        if (K4A_RESULT_SUCCEEDED != k4a_device_set_color_control(m_device,
                                                                 K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE,
                                                                 K4A_COLOR_CONTROL_MODE_MANUAL,
                                                                 exposureValue))
        {
            std::cout << "[Streaming Service] Failed to set the exposure" << std::endl;
            return false;
        }
    }
    else
    {
        if (K4A_RESULT_SUCCEEDED != k4a_device_set_color_control(m_device,
                                                                 K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE,
                                                                 K4A_COLOR_CONTROL_MODE_AUTO,
                                                                 0))
        {
            std::cout << "[Streaming Service] Failed to set auto exposure" << std::endl;
            return false;
        }
    }

    return true;
}
bool Kinect::TrySaveCalibrationFile(const char* base_dir){
    // Write calibration.json to disk for later analysis
    size_t calibration_size = 0;
    k4a_buffer_result_t buffer_result = k4a_device_get_raw_calibration(m_device, NULL, &calibration_size);
    if (buffer_result == K4A_BUFFER_RESULT_TOO_SMALL)
    {
        std::vector<uint8_t> calibration_buffer = std::vector<uint8_t>(calibration_size);
        buffer_result = k4a_device_get_raw_calibration(m_device, calibration_buffer.data(), &calibration_size);
        if (buffer_result == K4A_BUFFER_RESULT_SUCCEEDED)
        {
            // Remove the null-terminated byte from the file before writing it.
            if (calibration_size > 0 && calibration_buffer[calibration_size - 1] == '\0')
            {
                calibration_size--;
            }
            std::string file_name = base_dir;
            file_name += "/calibration.json";
            std::ofstream file_object(file_name, std::ios::out| std::ios::binary);
            
            file_object.write(reinterpret_cast<char*>(calibration_buffer.data()), 
                calibration_size);
            file_object.flush();
            file_object.close();
        }
        else
        {
            printf("Failed to read device calibration\n");
            return false;
        }
    }
    else
    {
        printf("Failed to read device calibration2\n");
        return false;
    }
    return true;
}

void Kinect::save_color_image(k4a_image_t image, std::string dir, std::string frame_s){

    // code for storing the buffer in an image file
    uint8_t* image_buffer = k4a_image_get_buffer(image);
    size_t image_buffer_size = k4a_image_get_size(image);
    std::string file_name = dir;
    file_name += frame_s;
    file_name += ".jpeg";
    std::ofstream file_object(file_name, std::ios::out| std::ios::binary);
    
    file_object.write(reinterpret_cast<char*>(image_buffer), 
        image_buffer_size);
    file_object.flush();
    file_object.close();
}

void Kinect::save_depth_or_ir_image(k4a_image_t image, std::string dir, std::string frame_s){
    // code for storing the buffer in an image file
    uint8_t* image_buffer = k4a_image_get_buffer(image);
    size_t image_buffer_size = k4a_image_get_size(image);
    std::string file_name = dir;
    file_name += frame_s;
    file_name += ".bin";
    std::ofstream file_object(file_name, std::ios::out| std::ios::binary);
    
    file_object.write(reinterpret_cast<char*>(image_buffer), 
        image_buffer_size);
    file_object.flush();
    file_object.close();  
}

int Kinect::Run(int capture_frame_count){
    long last_timestamp = 0;
    int frames_captured = 0;
    std::string frame_s = "";
    const int32_t TIMEOUT_IN_MS = 1000;

    if (capture_frame_count < 1){
        capture_frame_count = INT32_MAX;
    }

    m_streaming = true;

    printf("Capturing %d frames.", capture_frame_count);
    while (m_streaming && frames_captured++ < capture_frame_count)
    {
        // Get a depth frame
        switch (k4a_device_get_capture(m_device, &m_capture, TIMEOUT_IN_MS))
        {
        case K4A_WAIT_RESULT_SUCCEEDED:
            break;
        case K4A_WAIT_RESULT_TIMEOUT:
            printf("Timed out waiting for a capture\n");
            continue;
            break;
        case K4A_WAIT_RESULT_FAILED:
            printf("Failed to read a capture\n");
            return K4A_WAIT_RESULT_FAILED;
        }

        
        double curr_temp = (double)k4a_capture_get_temperature_c(m_capture);
        frame_s = std::to_string(frames_captured);
        printf("Capture %s %fC", frame_s.c_str(), curr_temp);
        m_lastTemp = curr_temp;
        // Probe for a color image
        k4a_image_t color_image = k4a_capture_get_color_image(m_capture);
        // probe for a IR16 image
        k4a_image_t ir_image = k4a_capture_get_ir_image(m_capture);
        // Probe for a depth16 image
        k4a_image_t depth_image = k4a_capture_get_depth_image(m_capture);

        if (color_image != NULL && ir_image != NULL && depth_image != NULL){
            long new_timestamp = (long)k4a_image_get_device_timestamp_usec(color_image);
            auto time_diff = (double)(new_timestamp - last_timestamp)/1000.0;
            last_timestamp = new_timestamp;
            printf(" | Time between captures: %f(ms)\n",
                    time_diff);

            // Save all the images
            save_color_image(color_image,m_data_dirs.colorFileDirectory, frame_s);
            save_depth_or_ir_image(ir_image, m_data_dirs.irFileDirectory, frame_s);
            save_depth_or_ir_image(depth_image, m_data_dirs.depthFileDirectory, frame_s);
        }

        if (color_image != NULL) k4a_image_release(color_image);
        if (ir_image != NULL) k4a_image_release(ir_image);
        if (depth_image != NULL) k4a_image_release(depth_image);

        // release capture
        k4a_capture_release(m_capture);     
    }
    m_streaming = false;
    return 0;
}

bool Kinect::GetRecentTemparature(double &last_temp){
    if (m_device == NULL){
        return false;
    }
    if (m_streaming == false){
        return false;
    }
    last_temp = m_lastTemp;
    
    return true;
}
std::thread Kinect::RunThread(int capture_frame_count){
    return std::thread(&Kinect::Run, this, capture_frame_count);
}
void Kinect::Stop(){
    m_streaming = false;
    if (m_device != NULL){
        k4a_device_close(m_device);
    }
}