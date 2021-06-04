#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <k4a/k4a.h>
#include <fstream>
#include <vector>
#include <sys/stat.h>

#include <string>
#include <string.h>
#include <errno.h>

#include "common_macros.h"
#include "date.h"

static const char* root_dir = "/media/FarmData01/Datasets/";
std::string m_depthFileDirectory = "";
std::string m_colorFileDirectory = "";
std::string m_irFileDirectory = "";

int mkdir_p(const char *path)
{
    /* Adapted from http://stackoverflow.com/a/2336245/119527 */
    const size_t len = strlen(path);
    char _path[PATH_MAX];
    char *p; 

    errno = 0;

    /* Copy string so its mutable */
    if (len > sizeof(_path)-1) {
        errno = ENAMETOOLONG;
        return -1; 
    }   
    strcpy(_path, path);

    /* Iterate the string */
    for (p = _path + 1; *p; p++) {
        if (*p == '/') {
            /* Temporarily truncate */
            *p = '\0';

            if (mkdir(_path, S_IRWXU) != 0) {
                if (errno != EEXIST)
                    return -1; 
            }

            *p = '/';
        }
    }   

    if (mkdir(_path, S_IRWXU) != 0) {
        if (errno != EEXIST)
            return -1; 
    }   

    return 0;
}

bool create_data_capture_directories(const char *fileDirectory){

 
   // create the directories for storing the captures
   std::string base_dir = fileDirectory;
   base_dir += "/camera_1";
   m_depthFileDirectory = base_dir;
   m_depthFileDirectory += "/Depth/";
   m_colorFileDirectory = base_dir;
   m_colorFileDirectory  += "/Color/";
   m_irFileDirectory = base_dir;
   m_irFileDirectory += "/Infrared/";
 
   int nError = 0;
   printf("Creating depth directory: %s", m_depthFileDirectory.c_str());
   nError = mkdir_p(m_depthFileDirectory.c_str());
   if (nError != 0)
   {
       printf("Depth directory (%s) creation failed. Exiting...", m_depthFileDirectory.c_str());
       return false;
   }
 
    printf("Creating color directory: %s", m_colorFileDirectory.c_str());
   nError = mkdir_p(m_colorFileDirectory.c_str());
   if (nError != 0)
   {
       printf("Color directory creation failed. Exiting...");
       return false;
   }
 
    printf("Creating infrared directory: %s", m_irFileDirectory.c_str());
   nError = mkdir_p(m_irFileDirectory.c_str());
   if (nError != 0)
   {
       printf("Infrared directory creation failed. Exiting...");
       return false;
   }
 
   return true;
}

void try_save_calibration(const char* base_dir, k4a_device_t device){
         // Write calibration.json to disk for later analysis
    size_t calibration_size = 0;
    k4a_buffer_result_t buffer_result = k4a_device_get_raw_calibration(device, NULL, &calibration_size);
    if (buffer_result == K4A_BUFFER_RESULT_TOO_SMALL)
    {
        std::vector<uint8_t> calibration_buffer = std::vector<uint8_t>(calibration_size);
        buffer_result = k4a_device_get_raw_calibration(device, calibration_buffer.data(), &calibration_size);
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
        }
    }
    else
    {
        printf("Failed to read device calibration2\n");
    }
}

int main(int argc, char* argv[]) {

    std::string s = date::format("%m_%d_%Y", std::chrono::system_clock::now());
    printf("Time: %s", s.c_str());
   std::string base_dir = root_dir;
   base_dir += s;
   if (create_data_capture_directories(base_dir.c_str()) == false){
       return -1;
   }

    int returnCode = 1;
    k4a_device_t device = NULL;
    const int32_t TIMEOUT_IN_MS = 1000;
    int captureFrameCount;
    k4a_capture_t capture = NULL;

    if (argc < 2)
    {
        printf("%s FRAMECOUNT\n", argv[0]);
        printf("Capture FRAMECOUNT color and depth frames from the device using the separate get frame APIs\n");
        returnCode = 2;
        return returnCode;
    }

    captureFrameCount = atoi(argv[1]);
    printf("Capturing %d frames\n", captureFrameCount);

    uint32_t device_count = k4a_device_get_installed_count();

    if (device_count == 0)
    {
        printf("No K4A devices found\n");
        return 0;
    }

    if (K4A_RESULT_SUCCEEDED != k4a_device_open(K4A_DEVICE_DEFAULT, &device))
    {
        printf("Failed to open device\n");
        return returnCode;
    }

    try_save_calibration(base_dir.c_str(), device);
   
   
    // Configure the device for ideal streaming parameters.
    // TODO: Change this to a config file later.
    k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.color_format = K4A_IMAGE_FORMAT_COLOR_MJPG;
    //config.color_resolution = K4A_COLOR_RESOLUTION_1440P;
    config.color_resolution = K4A_COLOR_RESOLUTION_3072P;
    //config.depth_mode = K4A_DEPTH_MODE_WFOV_2X2BINNED;
    config.depth_mode = K4A_DEPTH_MODE_WFOV_UNBINNED;
    config.camera_fps = K4A_FRAMES_PER_SECOND_15;
    // about returning only synchronized images
    // https://microsoft.github.io/Azure-Kinect-Sensor-SDK/master/structk4a__device__configuration__t_a8208974f05d89fc1362c6a0900bdef4d.html#a8208974f05d89fc1362c6a0900bdef4d
    

    if (K4A_RESULT_SUCCEEDED != k4a_device_start_cameras(device, &config))
    {
        printf("Failed to start device\n");
        return returnCode;
    }

    long last_timestamp = 0;
    int frames_captured = 0;
    while (frames_captured++ < captureFrameCount)
    {
        k4a_image_t image;

        // Get a depth frame
        switch (k4a_device_get_capture(device, &capture, TIMEOUT_IN_MS))
        {
        case K4A_WAIT_RESULT_SUCCEEDED:
            break;
        case K4A_WAIT_RESULT_TIMEOUT:
            printf("Timed out waiting for a capture\n");
            continue;
            break;
        case K4A_WAIT_RESULT_FAILED:
            printf("Failed to read a capture\n");
            return returnCode;
        }

        std::string frame_s = std::to_string(frames_captured);
        printf("Capture");

        // Probe for a color image
        image = k4a_capture_get_color_image(capture);
    
        if (image)
        {
            long new_timestamp = (long)k4a_image_get_device_timestamp_usec(image);
            auto time_diff = (double)(new_timestamp - last_timestamp)/1000.0;
            last_timestamp = new_timestamp;
            printf(" | Color res:%4dx%4d stride:%5d %f",
                   k4a_image_get_height_pixels(image),
                   k4a_image_get_width_pixels(image),
                   k4a_image_get_stride_bytes(image),
                   time_diff);

            // code for storing the buffer in an image file
            uint8_t* image_buffer = k4a_image_get_buffer(image);
            size_t image_buffer_size = k4a_image_get_size(image);
            std::string file_name = m_colorFileDirectory;
            file_name += frame_s;
            file_name += ".jpeg";
            std::ofstream file_object(file_name, std::ios::out| std::ios::binary);
            
            file_object.write(reinterpret_cast<char*>(image_buffer), 
                image_buffer_size);
            file_object.flush();
            file_object.close();

            k4a_image_release(image);
        }
        else
        {
            printf(" | Color None                       ");
        }

        // probe for a IR16 image
        image = k4a_capture_get_ir_image(capture);
        if (image != NULL)
        {
            printf(" | Ir16 res:%4dx%4d stride:%5d ",
                   k4a_image_get_height_pixels(image),
                   k4a_image_get_width_pixels(image),
                   k4a_image_get_stride_bytes(image));
            
            // code for storing the buffer in an image file
            uint8_t* image_buffer = k4a_image_get_buffer(image);
            size_t image_buffer_size = k4a_image_get_size(image);
            std::string file_name = m_irFileDirectory;
            file_name += frame_s;
            file_name += ".bin";
            std::ofstream file_object(file_name, std::ios::out| std::ios::binary);
            
            file_object.write(reinterpret_cast<char*>(image_buffer), 
                image_buffer_size);
            file_object.flush();
            file_object.close();            

            k4a_image_release(image);
        }
        else
        {
            printf(" | Ir16 None                       ");
        }

        // Probe for a depth16 image
        image = k4a_capture_get_depth_image(capture);
        if (image != NULL)
        {
            printf(" | Depth16 res:%4dx%4d stride:%5d\n",
                   k4a_image_get_height_pixels(image),
                   k4a_image_get_width_pixels(image),
                   k4a_image_get_stride_bytes(image));

            // code for storing the buffer in an image file
            uint8_t* image_buffer = k4a_image_get_buffer(image);
            size_t image_buffer_size = k4a_image_get_size(image);
            std::string file_name = m_depthFileDirectory;
            file_name += frame_s;
            file_name += ".bin";
            std::ofstream file_object(file_name, std::ios::out| std::ios::binary);
            
            file_object.write(reinterpret_cast<char*>(image_buffer), 
                image_buffer_size);
            file_object.flush();
            file_object.close();

            k4a_image_release(image);
        }
        else
        {
            printf(" | Depth16 None\n");
        }

        // release capture
        k4a_capture_release(capture);
        fflush(stdout);
    }

    returnCode = 0;

    if (device != NULL)
    {
        k4a_device_close(device);
    }

    return returnCode;
}
