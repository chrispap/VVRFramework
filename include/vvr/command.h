#ifndef VVR_COMMAND_H
#define VVR_COMMAND_H

#include "vvrframework_DLL.h"
#include <vvr/macros.h>

namespace vvr
{

    struct Command
    {
        virtual ~Command() { }
        virtual void operator()() = 0;
    };

    template <typename RecT, typename RetT=void>
    class SimpleCommand : public Command
    {
        typedef RetT(RecT::*ActT)();
        RecT* _receiver;
        ActT _action;

    public:
        SimpleCommand(RecT* rec, ActT act)
            : _receiver{rec}
            , _action{act}
        { }

        RetT operator()()
        {
            (_receiver->*_action)();
        }
    };

    class MacroCommand : public Command
    {
        std::vector<Command*> _commands;

    public:
        ~MacroCommand() { for (auto cmd : _commands) delete cmd; }
        void add(Command* cmd) { _commands.push_back(cmd); }
        void operator()() { for (auto cmd : _commands) (*cmd)(); }
    };
}

#endif
