// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

namespace Borek {

enum class GameState {
        kStopped,
        kPlaying,
};

struct EditorState {
        static GameState game_state;
};

}  // namespace Borek
