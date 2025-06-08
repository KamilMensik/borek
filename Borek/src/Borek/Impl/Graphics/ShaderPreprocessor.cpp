// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <fstream>
#include <regex>
#include <unordered_map>

#include "Include/Graphics/ShaderPreprocessor.h"
#include "Include/Debug/Assert.h"

static const std::string preprocessor_command_regex = "#(\\w+)\\s*([\\w\\s_]*)";
static const std::regex reg(preprocessor_command_regex);

namespace Borek {
namespace Graphics {

using Cmd = ShaderPreprocessor::Command;

static const std::unordered_map<std::string, Cmd> commands = {
        { "vertex", Cmd::kVertex }, { "fragment", Cmd::kFragment },
        { "name", Cmd::kName },
};

ShaderOptions::ShaderOptions()
{
        name = "";
}

bool ShaderPreprocessor::HandleCommand(const std::smatch& match)
{
        auto cmd = commands.find(match[1]);
        if (cmd == commands.end())
                return false;

        switch(cmd->second) {
        case Cmd::kFragment:
                s_CurrentOutput = 1;
                break;
        case Cmd::kVertex:
                s_CurrentOutput = 2;
                break;
        case Cmd::kName:
                s_Options.name = match[2];
                break;
        }

        return true;
}

void ShaderPreprocessor::Preprocess(const std::string& path,
                                    std::stringstream& vertex_shader,
                                    std::stringstream& fragment_shader)
{
        std::ifstream file(path);
        std::smatch match;
        std::string line;
        s_Options = ShaderOptions();

        while(std::getline(file, line)) {
                if (file.eof()) break;
                if (line == "") continue;
                if (std::regex_match(line, match, reg) && HandleCommand(match))
                        continue;

                BOREK_ENGINE_ASSERT(s_CurrentOutput != 0,
                                    "Shader type not set!");

                if (s_CurrentOutput == 2)
                        vertex_shader << line << "\n";
                else
                        fragment_shader << line << "\n";
        }

        s_CurrentOutput = 0;
        if (s_Options.name == "") {
                s_Options.name = path.substr(path.find_last_of("/") + 1);
        }
}

ShaderOptions ShaderPreprocessor::s_Options;

int ShaderPreprocessor::s_CurrentOutput;

}  // namespace Graphics
}  // namespace Borek
