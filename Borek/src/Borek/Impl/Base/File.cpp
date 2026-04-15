// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/File.h"
#include "Include/Engine/Utils/PathHelpers.h"

namespace Borek {

File::File()
{
}

File::File(std::string_view path, FileType_ type)
{
        std::ios::openmode mode;

        switch (type) {
        case FileType_Read:
                mode = std::ios::in;
                break;
        case FileType_Write:
                mode = std::ios::out;
                break;
        case FileType_ReadWrite:
                mode = std::ios::in | std::ios::out;
                break;
        }

        m_Path = Utils::Path::FromRelative(path);
        m_File = std::fstream(m_Path, mode);
}

void
File::Reset()
{
        m_File.seekp(0);
}

std::fstream&
File::Get()
{
        return m_File;
}

const std::string&
File::GetPath()
{
        return m_Path;
}

}  // namespace Borek
