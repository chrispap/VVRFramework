#include <vvr/utils.h>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <streambuf>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>

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
#ifdef __GNUG__
#   include <cstdlib>
#   include <cxxabi.h>
#endif

using namespace std;

float vvr::get_seconds()
{
    static quint64 msec_base = 0;
    quint64 msec = QDateTime::currentMSecsSinceEpoch();
    if (msec_base==0) msec_base=msec;
    return (float) (msec-msec_base) / 1000.0;
}

string vvr::get_exe_path()
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

string vvr::get_base_path()
{
    return __BASE_DIR__;
}

double vvr::normalize_deg(double deg)
{
    while (deg < 0)
        deg += 360;
    while (deg > 360)
        deg -= 360;

    return deg;
}

void vvr::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (item.back() == '\r') {
            item.erase(item.back());
        }
        elems.push_back(item);
    }
}

std::vector<std::string> vvr::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

bool vvr::mkdir(const std::string &path)
{
    QDir dir(QString::fromStdString(string(path)));
    return dir.mkpath(".");
}

bool vvr::file_exists(const std::string &path)
{
    QFileInfo checkFile(QString::fromStdString(path));
    return (checkFile.exists() && checkFile.isFile());
}

bool vvr::dir_exists(const std::string &path)
{
    QFileInfo checkFile(QString::fromStdString(path));
    return (checkFile.exists() && checkFile.isDir());
}

std::string vvr::zpn(int num, int len)
{
    std::ostringstream ss;
    ss << std::setw(len) << std::setfill('0') << num;
    return ss.str();
}

std::string vvr::read_file(const std::string &filename)
{
    auto arg  = std::ifstream(filename);
    auto arg1 = std::istreambuf_iterator<char>(arg);
    const auto arg2 = std::istreambuf_iterator<char>();
    return std::string(arg1, arg2);
}

#ifdef __GNUG__
std::string VVRFramework_API demangle(const char* name)
{
    int status = -123;
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status), std::free
    };
    return (status == 0) ? res.get() : name;
}
#endif
