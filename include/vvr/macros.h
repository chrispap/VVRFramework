#ifndef VVR_MACROS_H
#define VVR_MACROS_H

#define vvr_echo(x) std::cout<<#x<<" = "<<x<<std::endl

#define vvr_msg(x) std::cout<<x<<std::endl

#define vvr_sstr(x) dynamic_cast< std::ostringstream & >(( std::ostringstream() << std::dec << x ) ).str()

#define vvr_square(x) ((x)*(x))

#define vvr_flag(x) (1<<(x))

#define vvr_flag_on(v,f) (v & vvr_flag(f))

#define vvr_flag_toggle(v,c,f) case c: v ^= vvr_flag(f); std::cout \
    << #f << " = " << (vvr_flag_on(v,f) ? "ON" : "OFF") \
    << std::endl; break

#define vvr_decl_shared_ptr(x) \
    template<typename ...T>\
    static inline std::shared_ptr<x> Make(T&&... t) {\
        struct make_shared_enabler : public x {\
            make_shared_enabler(T&&... t) : x(std::forward<T>(t)...) {}\
        };\
        return std::make_shared<make_shared_enabler>(std::forward<T>(t)...);\
    }\
    typedef std::shared_ptr<x> Ptr;\
    typedef std::shared_ptr<const x> ConstPtr;

#endif
