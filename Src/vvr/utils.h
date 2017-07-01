#ifndef VVR_UTILS_H
#define VVR_UTILS_H

#include "vvrscenedll.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

#define echo(x) std::cout<<#x<<" = "<<x<<std::endl
#define msg(x) std::cout<<x<<std::endl
#define SQUARE(x) ((x)*(x))
#define SSTR(x) dynamic_cast< std::ostringstream & >(( std::ostringstream() << std::dec << x ) ).str()
#define DECL_SHARED_PTR(x) \
    template<typename ...T>\
    static inline std::shared_ptr<x> Make(T&&... t) {\
        struct make_shared_enabler : public x {\
            make_shared_enabler(T&&... t) : x(std::forward<T>(t)...) {}\
        };\
        return std::make_shared<make_shared_enabler>(std::forward<T>(t)...);\
    }\
    typedef std::shared_ptr<x> Ptr;\
    typedef std::shared_ptr<const x> ConstPtr;

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

#endif // VVR_UTILS_H
