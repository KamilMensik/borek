// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <filesystem>

#include "Include/Engine/Assets/IAsset.h"

namespace Borek {

struct ScriptAsset : public IAsset {
        ~ScriptAsset() override;

        void
        Serialize(const std::filesystem::path &path) override;

        void
        Deserialize(const std::filesystem::path &path) override;

        void
        Load() override;

        void
        Unload() override;

        uint32_t
        GetType() const override;

        std::string class_name;
        uint64_t ruby_class;
        uint8_t* code = nullptr;
        uint32_t code_size;
        uint32_t flags = 0;
};


}  // namespace Borek
