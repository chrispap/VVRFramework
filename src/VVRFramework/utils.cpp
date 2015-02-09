#include "utils.h"

#include <ctime>
#include <iostream>

#ifdef _WIN32
#	include <Windows.h>
#else
#	include <sys/time.h>
#	include <stdio.h>
#	include <unistd.h>
#endif
#ifdef __APPLE__
#   include <mach-o/dyld.h>
#endif

using namespace std;

double vvr::getSeconds()
{
#ifdef _WIN32
    return (double) GetTickCount() / 1000.0;
#else
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double) t.tv_sec + (double) t.tv_usec/1000000.0;
#endif
}

double vvr::elapsed(double startTime)
{
    return 1.e3* ((double) (clock() - startTime)) / CLOCKS_PER_SEC;
}

string vvr::getExePath()
{
#ifdef __linux__
    std::string path = "";
    pid_t pid = getpid();
    char buf[20] = {0};
    sprintf(buf,"%d",pid);
    std::string _link = "/proc/";
    _link.append( buf );
    _link.append( "/exe");
    char proc[512];
    int ch = readlink(_link.c_str(),proc,512);
    if (ch != -1) {
        proc[ch] = 0;
        path = proc;
        std::string::size_type t = path.find_last_of("/");
        path = path.substr(0,t);
    }
    return path + "/";
#endif

#ifdef _WIN32
    HMODULE hModule = GetModuleHandleW(NULL);
    WCHAR wPath[MAX_PATH];
    GetModuleFileNameW(hModule, wPath, MAX_PATH);
    char cPath[MAX_PATH];
    char DefChar = ' ';
    WideCharToMultiByte(CP_ACP, 0, wPath, -1, cPath, 260, &DefChar, NULL);
    string sPath(cPath);
    return sPath.substr(0, sPath.find_last_of("\\/")).append("\\");
#endif

#ifdef __APPLE__
    char cPath[1024];
    uint32_t size = sizeof(cPath);
    if (_NSGetExecutablePath(cPath, &size) == 0) {
        string sPath(cPath);
        return sPath.substr(0, sPath.find_last_of("/")).append("/");
    } else
        return "./";
#endif
}

string vvr::getBasePath()
{
#ifdef __APPLE__
    return getExePath() + "../../";
#else
    return getExePath() + "../../";
#endif
}

double vvr::normalizeAngle(double angle)
{
    while (angle < 0)
        angle += 360;
    while (angle > 360)
        angle -= 360;

    return angle;
}
