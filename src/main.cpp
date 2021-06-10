#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <k4a/k4a.h>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <thread>
#include <string>
#include <string.h>
#include <errno.h>
#include <unistd.h>


#include "common_macros.h"
#include "date.h"
#include "utils.h"
#include "kinect.h"
#include "buttons.h"
#include "settings.h"

static const char* root_dir = "/media/FarmData01/Datasets/";
static kinect::Kinect kinect_device;
static buttons::Buttons button_mapping;
static settings::Settings curr_settings;

std::atomic <bool> temparature_logging_enabled (false);



void logTemparatures(const char *fileDirectory){
    temparature_logging_enabled = true;
    std::string temparature_file_name = fileDirectory;
    temparature_file_name += "/temparature_readings.csv";
    std::ofstream file_object(temparature_file_name);
    std::string header = "Timestamp,CPU Temp (C),GPU Temp (C),Kinect Temp (C)\n";
    file_object.write(header.c_str(), header.length());

    while (temparature_logging_enabled){
        double cpu_temp;
        double gpu_temp;
        double kinect_temp;
        bool nanoTempsRead = readTemparatures(cpu_temp, gpu_temp);
        bool kinectTempRead = kinect_device.GetRecentTemparature(kinect_temp);
        if (nanoTempsRead && kinectTempRead){
            std::string s = date::format("%T", std::chrono::system_clock::now());
            char *output;
            int len = asprintf(&output, "%s,%f,%f,%f\n",s.c_str(),cpu_temp,gpu_temp,kinect_temp);
            file_object.write(output,len);
            free(output);
            file_object.flush();
        }

        usleep(1*1000*1000); // Log every 60 seconds
    }
    file_object.flush();
    file_object.close();
}


int main(int argc, char* argv[]) {

    // Create the directory structure for saving data
    std::string s = date::format("%m_%d_%Y", std::chrono::system_clock::now());
    printf("Time: %s", s.c_str());

    if (dirExists(root_dir) ==false){
        printf("Error! No External Hard Drive detected.\nWould you like to store data locally?.");
        char* usr_input;
        std::cin >> usr_input;
        if (usr_input == "y" || usr_input == "Y"){
            root_dir = "~/Documents/Datasets/";
        }
        else{
            return -1;
        }
    }

    std::string base_dir = root_dir;
    base_dir += s;
    if (curr_settings.CreateDataCaptureDirectories(base_dir.c_str()) == false){
        return -1;
    }

    if (curr_settings.LoadSettings("config.json") == false){
        return -1;
    }

    int returnCode = 1;
    int captureFrameCount;
    if (argc < 2)
    {
        printf("%s FRAMECOUNT\n", argv[0]);
        printf("Capture FRAMECOUNT color and depth frames from the device using the separate get frame APIs\n");
        returnCode = 2;
        return returnCode;
    }

    captureFrameCount = atoi(argv[1]);
    printf("Capturing %d frames\n", captureFrameCount);

    // Connect to the Kinect 
    if (kinect_device.Connect(-1, curr_settings) == false){
        printf("Failed to connect to Kinect.");
        return 2;
    }

    // Try saving the calibration file if possible
    kinect_device.TrySaveCalibrationFile(base_dir.c_str());

    // Set up GPIO pins on the Nano
    button_mapping.SetupPins();


    std::thread kinect_stream_thread = kinect_device.RunThread(captureFrameCount);
    std::thread button_thread = button_mapping.PollInputsThread(); 
    std::thread log_temps_thread (logTemparatures, base_dir.c_str());
    printf("**THREADS STARTED****\n");
    kinect_stream_thread.join();
    printf("**KINECT THREAD DONE****\n");; 
    temparature_logging_enabled = false;
    button_mapping.Close();
    button_thread.join();
    printf("**BUTTON THREAD DONE****\n");
    log_temps_thread.join();
    printf("**LOG TEMPS THREAD DONE****\n");
    returnCode = 0;

    kinect_device.Stop();


    return returnCode;
}
