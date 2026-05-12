// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Utils/StringHelpers.h"
#include "Include/Scripting/Ruby/Modules/RBIter.h"
#include <filesystem>
#include <mrbcpp.h>
#include <mruby/value.h>
#include <mruby/istruct.h>
#include <mruby/boxing_word.h>
#include <mruby/data.h>
#include <mruby/class.h>
#include <mruby/proc.h>
#include <mruby/string.h>

#include "Include/Scripting/Ruby/Modules/RBFile.h"
#include "Include/Core.h"
#include "Include/Base/File.h"
#include <string_view>

//TODO? def atime
//def ctime
//def mtime

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static void
file_free(mrb_state* mrb, void* asset)
{
        File* file = RCAST<File*>(asset);
        delete file;
}

static const mrb_data_type
file_data_type {
        .struct_name = "File",
        .dfree = file_free,
};

static MRB_FUNC(Open)
{
        const char* path;
        mrb_int psize;
        const char* open_type = "r";

        mrb_get_args(mrb, "s|z", &path, &psize, &open_type);

        FileType_ type;
        switch (Hash(open_type)) {
        case Hash("r"):
                type = FileType_Read;
                break;
        case Hash("w"):
                type = FileType_Write;
                break;
        case Hash("rw"):
                type = FileType_ReadWrite;
                break;
        default:
                mrb_raise(mrb, E_ARGUMENT_ERROR, "mode can be 'r', 'w' or 'rw'");
                return MRB_NIL;
        }

        File* file = new Borek::File(std::string_view(path, psize), type);
        return mrb_obj_value(mrb_data_object_alloc(mrb, RBFile::file_class, file, &file_data_type));
}

static MRB_FUNC(Close)
{
        File* f = RCAST<File*>(DATA_PTR(self));
        f->~File();
        DATA_PTR(self) = nullptr;
        return MRB_NIL;
}

static MRB_FUNC(ForEach)
{
        mrb_value params[3] = { MRB_NIL, mrb_str_new_static(mrb, "r", 1), MRB_NIL };
        mrb_get_args(mrb, "S|&", &params[0], &params[2]);
        mrb_value file = mrb_funcall_argv(mrb, self,
                                          mrb_intern_static(mrb, "open", 4),
                                          2, params);

        params[1] = params[2];
        return mrb_funcall_argv(mrb, file, mrb_intern_static(mrb, "each", 4), 2, params);
}

static MRB_FUNC(Initialize)
{
        const char* path;
        mrb_int psize;
        const char* open_type = "r";

        mrb_get_args(mrb, "s|z", &path, &psize, &open_type);

        FileType_ type;
        switch (Hash(open_type)) {
        case Hash("r"):
                type = FileType_Read;
                break;
        case Hash("w"):
                type = FileType_Write;
                break;
        case Hash("rw"):
                type = FileType_ReadWrite;
                break;
        default:
                mrb_raise(mrb, E_ARGUMENT_ERROR, "mode can be 'r', 'w' or 'rw'");
                return MRB_NIL;
        }

        File* file = new Borek::File(std::string_view(path, psize), type);
        mrb_data_init(self, file, &file_data_type);
        return self;
}

static MRB_FUNC(EachLine)
{
        mrb_value block = MRB_NIL;
        mrb_get_args(mrb, "|&", &block);

        File* f = RCAST<File*>(DATA_PTR(self));
        f->Reset();
        std::fstream& fstr = f->Get();
        std::string line;

        if (mrb_nil_p(block)) {
                mrb_value params[2] = { self };
                params[1] = mrb_symbol_value(mrb_intern_cstr(mrb, "each"));
                return mrb_class_new_instance(mrb, 2, params, RBIter::iter_class);
        }

        while (std::getline(fstr, line)) {
                mrb_yield(mrb, block, mrb_str_new_cstr(mrb, line.c_str()));
        };

        return MRB_NIL;
}

static MRB_FUNC(Write)
{
        mrb_value string = mrb_obj_as_string(mrb, MRB_ARG1);

        File* f = RCAST<File*>(DATA_PTR(self));
        f->Get() << std::string_view(RSTRING_PTR(string), RSTRING_LEN(string));

        return self;
}

static MRB_FUNC(Inspect)
{
        File* f = RCAST<File*>(DATA_PTR(self));
        return mrb_format(mrb, "#<File:%s>", f->GetPath().c_str());
}

static MRB_FUNC(SIsDirectory)
{
        const char* path;
        mrb_int path_len;

        mrb_get_args(mrb, "s", &path, &path_len);

        return MRB_BOOL(std::filesystem::is_directory(std::string_view(path, path_len)));
}

static MRB_FUNC(IsDirectory)
{
        File* f = RCAST<File*>(DATA_PTR(self));
        return MRB_BOOL(std::filesystem::is_directory(f->GetPath()));
}

static MRB_FUNC(SExists)
{
        const char* path;
        mrb_int path_len;

        mrb_get_args(mrb, "s", &path, &path_len);

        return MRB_BOOL(std::filesystem::exists(std::string_view(path, path_len)));
}

static MRB_FUNC(Exists)
{
        File* f = RCAST<File*>(DATA_PTR(self));
        return MRB_BOOL(std::filesystem::exists(f->GetPath()));
}

static MRB_FUNC(SIsFile)
{
        const char* path;
        mrb_int path_len;

        mrb_get_args(mrb, "s", &path, &path_len);

        return MRB_BOOL(std::filesystem::is_regular_file(std::string_view(path, path_len)));
}

static MRB_FUNC(IsFile)
{
        File* f = RCAST<File*>(DATA_PTR(self));
        return MRB_BOOL(std::filesystem::is_regular_file(f->GetPath()));
}

static MRB_FUNC(SSize)
{
        const char* path;
        mrb_int path_len;

        mrb_get_args(mrb, "s", &path, &path_len);

        return MRB_NUM(std::filesystem::file_size(std::string_view(path, path_len)));
}

static MRB_FUNC(Size)
{
        File* f = RCAST<File*>(DATA_PTR(self));
        return MRB_NUM(std::filesystem::file_size(f->GetPath()));
}

static MRB_FUNC(SExtension)
{
        const char* path;
        mrb_int path_len;

        mrb_get_args(mrb, "s", &path, &path_len);
        std::string_view pth(path, path_len);
        auto it = pth.find_last_of('.');
        if (it == 0) return mrb_str_new_static(mrb, "", 0);

        return MRB_STRING(pth.substr(it).data());
}

static MRB_FUNC(Extension)
{
        File* f = RCAST<File*>(DATA_PTR(self));

        std::string_view pth(f->GetPath());
        auto it = pth.find_last_of('.');
        if (it == 0) return mrb_str_new_static(mrb, "", 0);

        return MRB_STRING(pth.substr(it).data());
}

void
RBFile::Init(class RubyEngine& engine)
{
        auto& vm = engine.GetRubyVM();

        file_class = vm.define_class("BFile", MRB_TT_DATA)
                .define_class_method("open", Open, FuncArgs().Required(1).Optional(1))
                .define_class_method("foreach", ForEach, FuncArgs().Required(1).Block())
                .define_class_method("directory?", SIsDirectory, FuncArgs().Required(1))
                .define_class_method("exists?", SExists, FuncArgs().Required(1))
                .define_class_method("exist?", SExists, FuncArgs().Required(1))
                .define_class_method("file?", SIsFile, FuncArgs().Required(1))
                .define_class_method("size", SSize, FuncArgs().Required(1))
                .define_class_method("extname", SExtension, FuncArgs().Required(1))
                .define_method("directory?", IsDirectory)
                .define_method("exists?", Exists)
                .define_method("exist?", Exists)
                .define_method("file?", IsFile)
                .define_method("size", Size)
                .define_method("extname", Extension)
                .define_method("initialize", Initialize, FuncArgs().Required(1).Optional(1))
                .define_method("inspect", Inspect)
                .define_method("each", EachLine, FuncArgs().Block())
                .define_method("each_line", EachLine, FuncArgs().Block())
                .define_method("write", Write, FuncArgs().Required(1))
                .define_method("close", Close)
                .define_method("<<", Write, FuncArgs().Required(1));
}

Class RBFile::file_class;

}  // namespace RBModules
}  // namespace Borek
