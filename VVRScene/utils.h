#ifndef UTILS_H
#define UTILS_H

#include "vvrscenedll.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using namespace std;

#define echo(x) cout<<#x<<" = "<<x<<endl
#define msg(x) cout<<x<<endl
#define SQUARE(x) ((x)*(x))
#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#define VAR_CLASS_DEFS(x)        \
typedef boost::shared_ptr< x > Ptr;      \
typedef boost::shared_ptr<const x > ConstPtr;   \
static inline boost::shared_ptr<x> Make()    \
{              \
 return boost::make_shared<x>();      \
}              \
template<typename T0>         \
static inline boost::shared_ptr<x> Make(const T0& t0)   \
{              \
 return boost::make_shared<x>(t0);     \
}              \
template<typename T0, typename T1>      \
static inline boost::shared_ptr<x> Make(const T0& t0, const T1& t1) \
{              \
 return boost::make_shared<x>(t0, t1);    \
}              \
template<typename T0, typename T1, typename T2>   \
static inline boost::shared_ptr<x> Make(const T0& t0, const T1& t1, const T2& t2) \
{              \
return boost::make_shared<x>(t0, t1, t2);    \
}              \
template<typename T0, typename T1, typename T2, typename T3> \
static inline boost::shared_ptr<x> Make(const T0& t0, const T1& t1, const T2& t2, const T3& t3)   \
{              \
return boost::make_shared<x>(t0, t1, t2, t3);   \
}              \
template<typename T0, typename T1, typename T2, typename T3, typename T4> \
static inline boost::shared_ptr<x> Make(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4)   \
{              \
return boost::make_shared<x>(t0, t1, t2, t3, t4);  \
}              \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
static inline boost::shared_ptr<x> Make(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)   \
{              \
return boost::make_shared<x>(t0, t1, t2, t3, t4, t5); \
}              \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
static inline boost::shared_ptr<x> Make(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)   \
{              \
return boost::make_shared<x>(t0, t1, t2, t3, t4, t5, t6);\
}              \

namespace vvr {

    float           VVRScene_API getSeconds();
    double          VVRScene_API normalizeAngle(double angle);
    string          VVRScene_API getExePath();
    string          VVRScene_API getBasePath();
    bool            VVRScene_API mkdir(const std::string &path);
    void            VVRScene_API split(const string &s, char delim, vector<string> &elems);
    vector<string>  VVRScene_API split(const string &s, char delim);

}

#endif
