#ifndef VVR_MACROS_H
#define VVR_MACROS_H

#define vvr_echo(x) std::cout<<#x<<" = "<<x<<std::endl

#define vvr_msg(x) std::cout<<x<<std::endl

#define vvr_sstr(x) dynamic_cast< std::ostringstream & >(( std::ostringstream() << std::dec << x ) ).str()

#define VVR_DECL_SHARED_PTR(x) \
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
