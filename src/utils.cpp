#include <fstream>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

bool readTemparatures(double &cpu_temp, double &gpu_temp){
    char cpu[6];
    char gpu[6];
    
    // This uses the linux virtual device system to read temperature from different thermal zones
    // To figure out which zones we want to monitor, you can use this command to get a description of each
    // cat /sys/devices/virtual/thermal/thermal_zone*/type
    // CPU is thermal zone 1

    // GPU is thermal zone 2
    FILE* fcputemp = fopen("/sys/devices/virtual/thermal/thermal_zone1/temp", "r");
    FILE* fgputemp = fopen("/sys/devices/virtual/thermal/thermal_zone2/temp","r");
    if (!fcputemp || !fgputemp ) {
        return false;
    }
    
    cpu_temp = (double)atoi(fgets(cpu, 6, fcputemp))/1000;
    gpu_temp = (double)atoi(fgets(gpu, 6, fgputemp))/1000;
    
    printf("\rCpu : %.2fC, Gpu : %.2fC.", cpu_temp, gpu_temp);

    
    fclose(fcputemp);
    fclose(fgputemp);
    return true;
}

int mkdirRecursive(const char *path)
{
    // Adapted from http://stackoverflow.com/a/2336245/119527 
    const size_t len = strlen(path);
    char _path[PATH_MAX];
    char *p; 

    errno = 0;

    // Copy string so its mutable 
    if (len > sizeof(_path)-1) {
        errno = ENAMETOOLONG;
        return -1; 
    }   
    strcpy(_path, path);

    // Iterate the string 
    for (p = _path + 1; *p; p++) {
        if (*p == '/') {
            // Temporarily truncate 
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

int dirExists(const char *path)
{
    struct stat info;

    if(stat( path, &info ) != 0)
        return 0;
    else if(info.st_mode & S_IFDIR)
        return 1;
    else
        return 0;
}