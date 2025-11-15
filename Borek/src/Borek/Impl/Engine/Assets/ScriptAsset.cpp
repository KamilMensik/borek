// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>
#include <cstring>
#include <fstream>

#include <mruby.h>
#include <mruby/boxing_word.h>
#include <mruby/class.h>
#include <mruby/irep.h>
#include <mruby/hash.h>
#include <mrbcpp.h>

#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/IAsset.h"
#include "Include/Engine/Assets/ScriptAsset.h"
#include "Include/Base/Application.h"

namespace fs = std::filesystem;

namespace Borek {

static void
tokenize_class_name(const std::string& class_name, std::vector<std::string>& out)
{
        size_t start = 0;
        size_t end = class_name.find(':');

        while (end != std::string::npos) {
                out.push_back(class_name.substr(start, end - start));
                start = end + 1;
                end = class_name.find(':', start);
        }
        out.push_back(class_name.substr(start, class_name.size() - start));
}

static RClass*
get_class(const std::vector<std::string>& tokens)
{
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        RClass* res = nullptr;

        for (int i = 0; i < tokens.size(); i++) {
                const char* name = tokens[i].c_str();
                if (i != tokens.size() - 1) {
                        if (!res) {
                                if (!mrb_class_defined(mrb, name))
                                        return nullptr;

                                res = mrb_module_get(mrb, name);
                        } else {
                                if (!mrb_class_defined_under(mrb, res, name))
                                        return nullptr;

                                res = mrb_module_get_under(mrb, res, name);
                        }
                } else {
                        if (!res) {
                                if (!mrb_class_defined(mrb, name))
                                        return nullptr;

                                res = mrb_class_get(mrb, name);
                        } else {
                                if (!mrb_class_defined_under(mrb, res, name))
                                        return nullptr;

                                res = mrb_class_get_under(mrb, res, name);
                        }

                }
        }

        return res;
}

ScriptAsset::~ScriptAsset()
{
        if (code)
                delete[] code;
}

void
ScriptAsset::Serialize(const fs::path &path)
{
        std::ofstream asset(path, std::ios::binary);

        const uint32_t cname_size = class_name.size();
        asset.write(RCAST<const char*>(&cname_size), sizeof(uint32_t))
             .write(class_name.data(), cname_size)
             .write(RCAST<const char*>(&flags), sizeof(uint32_t))
             .write(RCAST<const char*>(&code_size), sizeof(uint32_t))
             .write(RCAST<const char*>(code), code_size);
}

void
ScriptAsset::Deserialize(const fs::path &path)
{
        std::ifstream asset(path, std::ios::binary);
        if (code)
                delete[] code;

        uint32_t cname_size;
        asset.read(RCAST<char*>(&cname_size), sizeof(uint32_t));
        class_name.resize(cname_size + 1);

        asset.read(class_name.data(), cname_size)
             .read(RCAST<char*>(&flags), sizeof(uint32_t))
             .read(RCAST<char*>(&code_size), sizeof(uint32_t));

        code = new uint8_t[code_size];
        asset.read(RCAST<char*>(code), code_size);
}

void
ScriptAsset::Load()
{
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        std::vector<std::string> tokens;
        tokenize_class_name(class_name, tokens);
        RClass* res = get_class(tokens);

        int ai = mrb_gc_arena_save(mrb);
        if (res) {
                mrb_value cls = { RCAST<uintptr_t>(res) };
                if (MRB_IV_DEFINED(cls, "@_export"))
                        mrb_hash_clear(mrb, MRB_GET_IV(cls, "@_export"));
        }

        mrb_load_irep_buf(mrb, code, code_size);
        if (mrb->exc)
                mrb_print_error(mrb);

        mrb_gc_arena_restore((mrb_state*)Application::GetRubyEngine().GetRubyVM(), ai);

        if (!res)
                res = get_class(tokens);

        if (res == nullptr) {
                BOREK_ENGINE_ERROR("Couldnt find class {}", class_name);
        }

        ruby_class = RCAST<uint64_t>(res);
}

void
ScriptAsset::Unload()
{
}

uint32_t
ScriptAsset::GetType() const
{
        return AssetType_Script;
}


}  // namespace Borek
