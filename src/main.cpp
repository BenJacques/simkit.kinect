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
#include "utils.h"
#include "kinect.h"

static const char* root_dir = "/media/FarmData01/Datasets/";


bool create_data_capture_directories(const char *fileDirectory, DATA_DIRS_T &data_dirs){

    // create the directories for storing the captures
    std::string base_dir = fileDirectory;
    base_dir += "/camera_1";
    data_dirs.depthFileDirectory = base_dir;
    data_dirs.depthFileDirectory += "/Depth/";
    data_dirs.colorFileDirectory = base_dir;
    data_dirs.colorFileDirectory  += "/Color/";
    data_dirs.irFileDirectory = base_dir;
    data_dirs.irFileDirectory += "/Infrared/";
    
    int nError = 0;
    printf("Creating depth directory: %s", data_dirs.depthFileDirectory.c_str());
    nError = mkdir_recursive(data_dirs.depthFileDirectory.c_str());
    if (nError != 0)   
    {
        printf("Depth directory (%s) creation failed. Exiting...", data_dirs.depthFileDirectory.c_str());
        return false;
    }
    
        printf("Creating color directory: %s", data_dirs.colorFileDirectory.c_str());
    nError = mkdir_recursive(data_dirs.colorFileDirectory.c_str());
    if (nError != 0)
    {
        printf("Color directory creation failed. Exiting...");
        return false;
    }
    
        printf("Creating infrared directory: %s", data_dirs.irFileDirectory.c_str());
    nError = mkdir_recursive(data_dirs.irFileDirectory.c_str());
    if (nError != 0)
    {
        printf("Infrared directory creation failed. Exiting...");
        return false;
    }
    
    return true;
}



int main(int argc, char* argv[]) {


    std::string s = date::format("%m_%d_%Y", std::chrono::system_clock::now());
    printf("Time: %s", s.c_str());
    std::string base_dir = root_dir;
    base_dir += s;
    DATA_DIRS_T data_dirs;
    if (create_data_capture_directories(base_dir.c_str(), data_dirs) == false){
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
    kinect::Kinect kinect_device = kinect::Kinect();

    if (kinect_device.Connect(-1) == false){
        printf("Failed to connect to Kinect.");
        return 2;
    }
    kinect_device.TrySaveCalibrationFile(base_dir.c_str());

    kinect_device.Run(captureFrameCount, data_dirs);


    returnCode = 0;

    //kinect_device.Stop();

    return returnCode;
}
