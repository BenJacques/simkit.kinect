#include <json/value.h>
#include <json/json.h>
#include "settings.h"
#include <fstream>
#include "utils.h"

using namespace settings;



void Settings::create_and_save_default_settings_file(std::string file_path){
    Json::Value root;

    root["exposure"] = 0;
    root["color_resolution"] = 5;
    root["depth_mode"] = 3;
    root["frames_per_second"] = 2;
    Json::StreamWriterBuilder builder;
    const std::string json_file = Json::writeString(builder, root);
    std::ofstream file_object(file_path, std::ios::out| std::ios::binary);
    
    file_object.write(json_file.c_str(),json_file.length()); 
    file_object.flush();
    file_object.close();
}

bool Settings::load_settings(std::string file_path){
    Json::Value root;
    std::ifstream ifs;
    ifs.open(file_path);
    if (ifs.good()== false){
        ifs.close();
        // If the config file doesn't exist, then create and save it first
        printf("No settings file found. Creating default one.\n");
        create_and_save_default_settings_file(file_path);
    }
    ifs.open(file_path);
    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING   errs;
    if (!parseFromStream(builder, ifs, &root, &errs)) {
        printf("Error loading settings from file. %s", errs.c_str());
        return false;
    }
    exposure = root["exposure"].asInt();
    frames_per_second = root["frames_per_second"].asInt();
    depth_mode = root["depth_mode"].asInt();
    color_resolution = root["color_resolution"].asInt();
    return true;
}


bool Settings::create_data_capture_directories(const char *fileDirectory){

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