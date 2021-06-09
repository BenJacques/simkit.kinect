#ifndef SETTINGS_H
#define SETTINGS_H
#include <string>




namespace settings
{

struct DATA_DIRS_T {
    std::string depthFileDirectory;
    std::string colorFileDirectory;
    std::string irFileDirectory;
};

class Settings
{


public:
    int exposure;
    int frames_per_second;
    int depth_mode;
    int color_resolution;
    DATA_DIRS_T data_dirs;

    bool load_settings(std::string file_path);
    bool create_data_capture_directories(const char *fileDirectory);
private:
    void create_and_save_default_settings_file(std::string file_path);

};
} // end namespace Settings

#endif