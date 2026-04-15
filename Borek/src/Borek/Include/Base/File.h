// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <fstream>
#include <string_view>

namespace Borek {

enum FileType_ {
        FileType_Read,
        FileType_Write,
        FileType_ReadWrite,
};

class File {
public:
        File();
        File(std::string_view path, FileType_ type);

        void
        Reset();

        std::fstream&
        Get();

        const std::string&
        GetPath();

private:
        std::string m_Path;
        std::fstream m_File;
};

}  // namespace Borek
