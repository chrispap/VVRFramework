#ifndef VVR_UTILS_H
#define VVR_UTILS_H

#include "vvrframework_DLL.h"
#include <algorithm>
#include <iostream>
#include <typeinfo>
#include <sstream>
#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <tuple>

namespace vvr
{
    float
    VVRFramework_API get_seconds();

    double
    VVRFramework_API normalize_deg(double deg);

    std::string
    VVRFramework_API get_exe_path();

    std::string
    VVRFramework_API get_base_path();

    bool
    VVRFramework_API mkdir(const std::string &path);

    bool
    VVRFramework_API file_exists(const std::string &filename);

    bool
    VVRFramework_API dir_exists(const std::string &dirname);

    void
    VVRFramework_API split(const std::string &s, char delim, std::vector<std::string> &elems);

    std::vector<std::string>
    VVRFramework_API split(const std::string &s, char delim);

    std::string
    VVRFramework_API zpn(int num, int len);

    std::string
    VVRFramework_API read_file(const std::string &filename);

    inline char get_path_separator()
    {
#ifdef _WIN32
        return '\\';
#else
        return '/';
#endif
    }

#ifdef __GNUG__
    std::string
    VVRFramework_API demangle(const char* name);

    template <class T>
    std::string typestr(const T& t) { return demangle(typeid(t).name()); }
#else
    template <class T>
    std::string typestr(const T& t) { return typeid(t).name(); }
#endif

    template <typename T>
    class BackupAndRestore {
        T& valRef;
        T const oldVal;
    public:
        explicit BackupAndRestore(T& ref) : valRef(ref), oldVal(ref) {}
        explicit BackupAndRestore(T& ref, T newVal) : valRef(ref), oldVal(ref) { ref = newVal; }
        ~BackupAndRestore() { valRef = oldVal; }
    };

    template <class T>
    inline std::string make_str(const T& x)
    {
        std::stringstream ss;
        ss << x;
        return ss.str();
    }

    template<class T, 
    typename = typename std::enable_if<
        std::is_integral<T>::value || 
        std::is_floating_point<T>::value>::type>
    inline std::string make_str(const T& x)
    {
        return std::to_string(x);
    }

    template<>
    inline std::string make_str<std::string>(const std::string& x)
    {
        return x;
    }

    template <class V >
    std::string implode(V const& v, const char* h = "")
    {
        const size_t n = std::strlen(h);
        char delim = ',';
        if (n==1) delim = h[0];
        else if (n==3) delim = h[1];
        std::stringstream ss;
        if (n==3) ss << h[0];
        auto it = std::cbegin(v);
        const auto end = std::cend(v);
        if (it != end) ss << *(it++);
        while (it != end) ss << delim << *(it++);
        if (n==3) ss << h[2];
        return ss.str();
    }

    template <class Tuple, class F, size_t... Is>
    constexpr auto apply_impl(Tuple& t, F f, std::index_sequence<Is...>) {
        int dummy[] = {0, (f(std::get<Is>(t)), void(), 0)...};
        static_cast<void>(dummy);
    }

    template <class Tuple, class F>
    constexpr auto apply(Tuple& t, F f) {
        return apply_impl(t, f, std::make_index_sequence<std::tuple_size<Tuple>({})>{});
    }
}

#endif
