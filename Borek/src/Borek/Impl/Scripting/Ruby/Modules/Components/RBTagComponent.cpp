// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>
#include <mruby/array.h>

#include <mrbcpp.h>

#include "Include/Components/TagComponent.h"
#include "Include/Base/Entity.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/Components/RBTagComponent.h"
//#include "Include/Scripting/Ruby/Modules/RBChangesExporter.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Initialize)
{
        mrb_value node = MRB_ARG1;
        Entity e(mrb_integer(MRB_GET_IV(node, "@entity_id")));
        auto& tags = e.GetComponent<TagComponent>();

        mrb_value arr = mrb_ary_new_capa(mrb, tags.size());
        for (int i = 0; Symbol& tag : tags) {
                mrb_ary_set(mrb, arr, i, mrb_symbol_value(MRB_SYM(tag.Str().c_str())));
                i++;
        }

        MRB_SET_IV(self, "@tags", arr);

        MRB_SET_IV(self, "@node", node);
        
        return self;
}

static MRB_FUNC(Notify)
{
        //ChangesExporter::AddSprite(mrb, MRB_GET_IV(self, "@node"));
        return MRB_NIL;
}

void RBTagComponent::Init(RubyEngine& engine)
{
        Module& borek = engine.GetBorekModule();

        tag_component_class = borek.define_class("SpriteComponent")
                .define_method("initialize", Initialize,
                               FuncArgs().Required(1))
                .define_method("notify", Notify);
}

Class RBTagComponent::tag_component_class;

}  // namespace RBModules
}  // namespace Borek
