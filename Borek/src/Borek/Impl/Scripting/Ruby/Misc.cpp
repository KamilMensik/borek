// Copyright 2024-2025 <kamilekmensik@gmail.com>
// Because mruby doesnt want to allow some of its internal methods exposed :/

#include <iostream>
#include <sstream>
#include <string>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/debug.h>
#include <mruby/error.h>
#include <mruby/class.h>
#include <mruby/internal.h>

#include "Include/Scripting/Ruby/Misc.h"
#include "Include/Base/Application.h"

namespace Borek {

static mrb_value
mrb_exc_mesg_get_(mrb_state *mrb, struct RException *exc)
{
  if (exc->mesg == NULL) return mrb_nil_value();
  return mrb_obj_value(exc->mesg);
}

static mrb_bool
class_ptr_p(mrb_value obj)
{
  switch (mrb_type(obj)) {
  case MRB_TT_CLASS:
  case MRB_TT_SCLASS:
  case MRB_TT_MODULE:
    return TRUE;
  default:
    return FALSE;
  }
}

static mrb_value
class_name_str(mrb_state *mrb, struct RClass* c)
{
  mrb_value path = mrb_class_path(mrb, c);
  if (mrb_nil_p(path)) {
    path = c->tt == MRB_TT_MODULE ? mrb_str_new_lit(mrb, "#<Module:") :
                                    mrb_str_new_lit(mrb, "#<Class:");
    mrb_str_cat_str(mrb, path, mrb_ptr_to_str(mrb, c));
    mrb_str_cat_lit(mrb, path, ">");
  }
  return path;
}

static mrb_value
mrb_mod_to_s_(mrb_state *mrb, mrb_value klass)
{
  if (mrb_sclass_p(klass)) {
    mrb_value v = mrb_iv_get(mrb, klass, MRB_SYM("__attached__"));
    mrb_value str = mrb_str_new_lit(mrb, "#<Class:");

    if (class_ptr_p(v)) {
      mrb_str_cat_str(mrb, str, mrb_inspect(mrb, v));
    }
    else {
      mrb_str_cat_str(mrb, str, mrb_any_to_s(mrb, v));
    }
    return mrb_str_cat_lit(mrb, str, ">");
  }
  else {
    return class_name_str(mrb, mrb_class_ptr(klass));
  }
}

static mrb_value
mrb_exc_get_output_(mrb_state *mrb, struct RObject *exc)
{
  mrb_value cname = mrb_mod_to_s_(mrb, mrb_obj_value(mrb_class_real(exc->c)));
  mrb_value mesg = mrb_exc_mesg_get_(mrb, (struct RException*)exc); /* string or nil */
  return (mrb_nil_p(mesg)||RSTRING_LEN(mesg)==0) ? cname : mrb_format(mrb, "%v (%v)", mesg, cname);
}

static mrb_value
decode_location(mrb_state *mrb, const struct mrb_backtrace_location *entry)
{
  mrb_value btline;
  int32_t lineno;
  const char *filename;

  if (!entry->irep || !mrb_debug_get_position(mrb, entry->irep, entry->idx, &lineno, &filename)) {
    btline = mrb_str_new_lit(mrb, "(unknown):0");
  }
  else if (lineno != -1) {//debug info was available
    btline = mrb_format(mrb, "%s:%d", filename, (int)lineno);
  }
  else { //all that was left was the stack frame
    btline = mrb_format(mrb, "%s:0", filename);
  }
  if (entry->method_id != 0) {
    mrb_str_cat_lit(mrb, btline, ":in ");
    mrb_str_cat_cstr(mrb, btline, mrb_sym_name(mrb, entry->method_id));
  }
  return btline;
}

std::string get_mrb_exc_text()
{
        std::stringstream res;

        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        RObject* exc = mrb->exc;
        RBasic* ptr = ((RException*)mrb->exc)->backtrace;

        struct RArray *ary = NULL;
        struct RBacktrace *bt = NULL;
        mrb_int n = 0;

        if (ptr) {
                if (ptr->tt == MRB_TT_ARRAY) {
                        ary = (struct RArray*)ptr;
                        n = ARY_LEN(ary);
                } else {
                        bt = (struct RBacktrace*)ptr;
                        n = (mrb_int)bt->len;
                }
        }

        if (n != 0) {
                mrb_value btline;

                res << "trace (most recent call last):\n";
                for (mrb_int i=n-2; i>0; i--) {
                        if (ary) btline = ARY_PTR(ary)[i];
                        else btline = decode_location(mrb, &bt->locations[i]);
                        if (mrb_string_p(btline)) {
                                res << "[" << i << "] ";
                                res << RSTRING_PTR(btline) << "\n";
                        }
                }

                if (ary) btline = ARY_PTR(ary)[0];
                else btline = decode_location(mrb, &bt->locations[0]);
                if (mrb_string_p(btline)) {
                        res << "[0] " << RSTRING_PTR(btline) << ": ";
                }
        } else {
                res << "(unknown):0: ";
        }

        if (exc == mrb->nomem_err) {
                res << "Out of memory (NoMemoryError)\n";
        } else {
                mrb_value output = mrb_exc_get_output_(mrb, exc);
                res << RSTRING_PTR(output) << '\n';
        }

        return res.str();
}

}  // namespace Borek
