// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

namespace Borek {

class ICommand {
public:
        virtual ~ICommand() = default;

        virtual void
        Execute() { Redo(); };

        virtual void
        Undo() = 0;

        virtual void
        Redo() = 0;
};

}  // namespace Borek
