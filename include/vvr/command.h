#ifndef VVR_COMMAND_H
#define VVR_COMMAND_H

#include "vvrframework_DLL.h"
#include "macros.h"
#include <unordered_map>

namespace vvr
{
    struct Cmd
    {
        virtual ~Cmd() { }
        virtual void operator()() = 0;
    };

    template <typename RecT, typename RetT=void>
    class SimpleCmd : public Cmd
    {
        typedef RetT(RecT::*ActT)();
        RecT* _receiver;
        ActT _action;

    public:
        SimpleCmd(RecT* rec, ActT act)
            : _receiver{rec}
            , _action{act}
        { }

        void operator()() override { (_receiver->*_action)(); }
    };

    class MacroCmd : public Cmd
    {
        std::vector<Cmd*> _commands;

    public:
        ~MacroCmd() { clear(); }
        void add(Cmd* cmd) { _commands.push_back(cmd); }
        void clear() { for (auto cmd : _commands) delete cmd; _commands.clear(); }
        void operator()() { for (auto cmd : _commands) (*cmd)(); }
    };

    typedef std::unordered_map<char, vvr::MacroCmd> KeyMap;
}

#endif
