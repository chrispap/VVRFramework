#ifndef VVR_UTILS_H
#define VVR_UTILS_H

#include "vvrframework_DLL.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include <typeinfo>
#include <tuple>
#include <utility>

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

#ifdef __GNUG__
    std::string vvrframework_API demangle(const char* name);

    template <class T>
    std::string typestr(const T& t) { return demangle(typeid(t).name()); }
#else
    template <class T>
    std::string typestr(const T& t) { return typeid(t).name(); }
#endif

    template <class Tuple, class F, size_t... Is>
    constexpr auto apply_impl(Tuple& t, F f, std::index_sequence<Is...>) {
        int dummy[] = {0, (f(std::get<Is>(t)), void(), 0)...};
        static_cast<void>(dummy);
    }

    template <class Tuple, class F>
    constexpr auto apply(Tuple& t, F f) {
        return apply_impl(t, f, std::make_index_sequence<std::tuple_size<Tuple>{}>{});
    }
}

#endif
