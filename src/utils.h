#ifndef UTILS_H
#define UTILS_H
bool readTemparatures(double &cpu_temp, double &gpu_temp);
int mkdirRecursive(const char *path);
int dirExists(const char *path);
#endif