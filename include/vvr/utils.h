#ifndef VVR_UTILS_H
#define VVR_UTILS_H

#include "vvrframework_DLL.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

namespace vvr
{
    float vvrframework_API getSeconds();
    double vvrframework_API normalizeAngle(double angle);
    std::string vvrframework_API getExePath();
    std::string vvrframework_API getBasePath();
    bool vvrframework_API mkdir(const std::string &path);
    bool vvrframework_API fileExists(const std::string &filename);
    bool vvrframework_API dirExists(const std::string &dirname);
    void vvrframework_API split(const std::string &s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> vvrframework_API split(const std::string &s, char delim);
    std::string vvrframework_API zpn(int num, int len);
}

#endif
