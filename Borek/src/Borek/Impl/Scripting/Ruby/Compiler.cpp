// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Utils/PathHelpers.h"
#include <cstdio>

#include <mruby.h>
#include <mruby/irep.h>
#include <mruby/compile.h>
#include <mruby/dump.h>
#include <mruby/proc.h>
#include <mruby/string.h>

#include <mrbcpp.h>

#include "Include/Scripting/Ruby/Compiler.h"

namespace Borek {

namespace fs = std::filesystem;

Compiler::Compiler(mrbcpp::VM& vm)
{
        m_VM = &vm;
        m_Context = mrb_ccontext_new(*m_VM);
        m_Context->no_optimize = false;
        m_Context->dump_result = false;
}

Compiler::~Compiler()
{
        mrb_ccontext_free(*m_VM, m_Context);
}

void Compiler::Compile(const fs::path& in, const fs::path& out)
{
        FILE* fin = fopen(in.c_str(), "r");
        FILE* fout = fopen(out.c_str(), "wb");

        std::string rel_path = Utils::Path::ToRelative(in);
        mrb_ccontext_filename(*m_VM, m_Context, rel_path.c_str());
        auto parser_state = mrb_parse_file(*m_VM, fin, m_Context);
        RProc* res = mrb_generate_code(*m_VM, parser_state);

        mrb_dump_irep_binary(*m_VM, res->body.irep, MRB_DUMP_DEBUG_INFO, fout);

        fclose(fin);
        fclose(fout);
}

}  // namespace Borek
