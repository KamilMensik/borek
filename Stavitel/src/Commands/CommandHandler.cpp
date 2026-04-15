// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <utility>

#include "Commands/CommandHandler.h"
#include "Commands/ICommand.h"
#include "Include/Core.h"

namespace Borek {

void
CommandHandler::Undo()
{
        if (m_UndoStack.empty())
                return;

        m_UndoStack.top()->Undo();
        m_RedoStack.push(std::move(m_UndoStack.top()));
        m_UndoStack.pop();
        m_Changed = true;
}

void
CommandHandler::Redo()
{
        if (m_RedoStack.empty())
                return;

        m_RedoStack.top()->Redo();
        m_UndoStack.push(std::move(m_RedoStack.top()));
        m_RedoStack.pop();
        m_Changed = true;
}

void
CommandHandler::AddCommand(Uniq<ICommand> cmd)
{
        while (!m_RedoStack.empty())
                m_RedoStack.pop();

        cmd->Execute();
        m_UndoStack.push(std::move(cmd));
        m_Changed = true;
}

bool
CommandHandler::Changed()
{
        return std::exchange(m_Changed, false);
}

}  // namespace Borek
