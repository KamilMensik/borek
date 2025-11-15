// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <filesystem>

namespace Borek {

class ResourceAssetifier {
public:
        static void
        AssetifyFolder(const std::filesystem::path& path);

        static void
        AssetifyFile(const std::filesystem::path& path);

        static void
        AssetifyImage(const std::filesystem::path& path);

        static void
        AssetifyScript(const std::filesystem::path& path);

        static void
        AssetifySound(const std::filesystem::path& path);

        static void
        AssetifyFont(const std::filesystem::path& path);
};

}  // namespace Borek
