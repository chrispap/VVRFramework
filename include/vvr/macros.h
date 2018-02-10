#ifndef VVR_MACROS_H
#define VVR_MACROS_H

/*----[VVR MACRO]-----------------------------------------------------------------------*/
#define vvr_echo(x) std::cout<<#x<<" = "<<x<<std::endl

/*----[VVR MACRO]-----------------------------------------------------------------------*/
#define vvr_msg(x) std::cout<<x<<std::endl

/*----[VVR MACRO]-----------------------------------------------------------------------*/
#define vvr_square(x) ((x)*(x))

/*----[VVR MACRO]-----------------------------------------------------------------------*/
#define vvr_flag(x) (1<<(x))

/*----[VVR MACRO]-----------------------------------------------------------------------*/
#define vvr_flag_test(v,f) (v & vvr_flag(f))

/*----[VVR MACRO]-----------------------------------------------------------------------*/
#define vvr_setmemb(x) this->x = x

/*----[VVR MULTILINE MACRO]-------------------------------------------------------------*/
#define vvr_sstr(x) dynamic_cast<std::ostringstream&>                                   \
    ((std::ostringstream()<<std::dec<<x)).str()

/*----[VVR MULTILINE MACRO]-------------------------------------------------------------*/
#define vvr_flag_toggle(v,c,f) case c: v ^= vvr_flag(f); std::cout                      \
    << #f << " = " << (vvr_flag_test(v,f) ? "ON" : "OFF")                                 \
    << std::endl; break                                                                 \

/*----[VVR MULTILINE MACRO]-------------------------------------------------------------*/
#define vvr_decl_shared_ptr(x)                                                          \
    template<typename ...T>                                                             \
    static inline std::shared_ptr<x> Make(T&&... t) {                                   \
        struct make_shared_enabler : public x {                                         \
            make_shared_enabler(T&&... t) : x(std::forward<T>(t)...) {}                 \
        };                                                                              \
        return std::make_shared<make_shared_enabler>(std::forward<T>(t)...);            \
    }                                                                                   \
    typedef std::shared_ptr<x> Ptr;                                                     \
    typedef std::shared_ptr<const x> ConstPtr;                                          \

/*----[VVR MULTILINE MACRO]-------------------------------------------------------------*/
#define vvr_decl_shape(Name, Base, filled)                                              \
    void setGeom(const Base& gmb)                                                       \
    {                                                                                   \
        static_cast<Base&>(*this) = gmb;                                                \
    }                                                                                   \
    Name(const vvr::Colour& col=vvr::Colour())                                          \
        : Shape(col, filled)                                                            \
    {                                                                                   \
        setup();                                                                        \
    }                                                                                   \
    Name(const Base& gmb, const vvr::Colour& col=vvr::Colour())                         \
        : Shape(col, filled)                                                            \
        , Base(gmb)                                                                     \
    {                                                                                   \
        setup();                                                                        \
    }                                                                                   \
    typedef Base GeomBase;                                                              \
    vvr_decl_shared_ptr(Name)                                                           \

/*----[VVR MULTILINE MACRO]-------------------------------------------------------------*/
#define vvr_invoke_main_with_scene(SceneClassName)                                      \
int main(int argc, char* argv[])                                                        \
{                                                                                       \
    try                                                                                 \
    {                                                                                   \
        return vvr::mainLoop(argc, argv, new SceneClassName);                           \
    }                                                                                   \
    catch (std::string exc)                                                             \
    {                                                                                   \
        std::cerr << exc << std::endl;                                                  \
    }                                                                                   \
    return 1;                                                                           \
}                                                                                       \

#endif
