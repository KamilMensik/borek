// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <stack>

#include "Commands/ICommand.h"
#include "Include/Core.h"

namespace Borek {

/*
 * Class for Stavitel editor commands handling.
 * Supports undo/redoing
 */
class CommandHandler {
public:
        void
        Undo();

        void
        Redo();

        // Executes given command and adds it to the undo stack
        void
        AddCommand(Uniq<ICommand> cmd);

        // Returns true if changed. Sets changed state to false.
        bool
        Changed();

private:
        std::stack<Uniq<ICommand>> m_UndoStack;
        std::stack<Uniq<ICommand>> m_RedoStack;
        bool m_Changed;
};

}  // namespace Borek
