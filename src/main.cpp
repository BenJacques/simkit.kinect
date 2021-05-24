#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <k4a/k4a.h>
#include <fstream>
#include "common_macros.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

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

    //k4a fastcapture_streaming example would be good to use 
    k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.color_format = K4A_IMAGE_FORMAT_COLOR_MJPG;
    config.color_resolution = K4A_COLOR_RESOLUTION_720P;
    config.depth_mode = K4A_DEPTH_MODE_WFOV_UNBINNED;
    config.camera_fps = K4A_FRAMES_PER_SECOND_15;
    // about returning only synchronized images
    // https://microsoft.github.io/Azure-Kinect-Sensor-SDK/master/structk4a__device__configuration__t_a8208974f05d89fc1362c6a0900bdef4d.html#a8208974f05d89fc1362c6a0900bdef4d
    

    if (K4A_RESULT_SUCCEEDED != k4a_device_start_cameras(device, &config))
    {
        printf("Failed to start device\n");
        return returnCode;
    }

    while (captureFrameCount-- > 0)
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

        printf("Capture");

        // Probe for a color image
        image = k4a_capture_get_color_image(capture);
        if (image)
        {
            printf(" | Color res:%4dx%4d stride:%5d ",
                   k4a_image_get_height_pixels(image),
                   k4a_image_get_width_pixels(image),
                   k4a_image_get_stride_bytes(image));

            // code for storing the buffer in an image file
            uint8_t* image_buffer = k4a_image_get_buffer(image);
            size_t image_buffer_size = k4a_image_get_size(image);
            const std::string file_name = "hello_rgb.jpeg";
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
            const std::string file_name = "hello_ir.jpeg";
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
            const std::string file_name = "hello_depth.jpeg";
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
