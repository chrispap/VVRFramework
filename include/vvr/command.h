#ifndef VVR_COMMAND_H
#define VVR_COMMAND_H

#include "vvrframework_DLL.h"

namespace vvr 
{

    struct Command
    {
        virtual ~Command() { }
        virtual void operator()() = 0;
    };

    template <typename Receiver, typename Ret=void>
    class SimpleCommand : public Command
    {
        typedef Ret(Receiver::*Action)();
        Receiver* _receiver;
        Action _action;

    public:
        SimpleCommand(Receiver* rec, Action action)
            : _receiver{rec}
            , _action{action}
        { }

        Ret operator()()
        {
            (_receiver->*_action)();
        }
    };

    class MacroCommand
    {
        std::vector<Command*> _commands;

    public:
        void add(Command* cmd) { _commands.push_back(cmd); }
        void operator()() { for (auto cmd : _commands) (*cmd)(); }
    };
}

#endif
