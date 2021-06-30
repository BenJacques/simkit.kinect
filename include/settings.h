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

    bool LoadSettings(std::string file_path);
    bool CreateDataCaptureDirectories(const char *fileDirectory);
private:
    void createAndSaveDefaultSettingsFile(std::string file_path);

};
} // end namespace Settings

#endif