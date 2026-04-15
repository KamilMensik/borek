// Copyright 2024-2025 <kamilekmensik@gmail.com>


#include "Include/Scripting/Ruby/Modules/Nodes/RBArea.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBCamera.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBParticleEmmiter.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBTilemap.h"
#include <mrbcpp.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/boxing_word.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/value.h>

#include "Include/Components/TagComponent.h"
#include "Include/Base/Entity.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBNode.h"
#include "Include/Scripting/Ruby/Modules/Components/RBTransform.h"
#include "Include/Scripting/Ruby/Modules/Components/RBTagComponent.h"
#include "Include/Scripting/Ruby/Modules/RBChangesExporter.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBDynamicBody.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBText2D.h"
#include "Include/Base/Application.h"
#include "Include/Debug/Log.h"
#include "Include/Engine/EntityInitializer.h"
#include "Include/Events/EntityEvents.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBAnimatedSprite.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBSound.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBSprite.h"
#include "Include/Scripting/Ruby/Modules/RBDatatypes.h"
#include "Include/Scripting/Ruby/Modules/RBNDatatypes.h"

#define GET_TRANSFORM_SAFE                                                     \
mrb_value transform = MRB_GET_IV(self, "@_transform");                         \
if (mrb_nil_p(transform)) {                                                    \
        transform = mrb_class_new_instance(mrb, 1, &self,                      \
                                           RBTransform::transform_class);      \
        MRB_SET_IV(self, "@_transform", transform);                            \
}

#define GET_TAGS_SAFE                                                          \
mrb_value tags = MRB_GET_IV(self, "@_tags");                                   \
if (mrb_nil_p(tags)) {                                                         \
        tags = mrb_class_new_instance(mrb, 1, &self,                           \
                                      RBTagComponent::tag_component_class);    \
        MRB_SET_IV(self, "@_tags", tags);                                      \
}


namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static MRB_FUNC(Position)
{
        GET_TRANSFORM_SAFE;

        return MRB_GET_IV(transform, "@position");
}

static MRB_FUNC(Scale)
{
        GET_TRANSFORM_SAFE;

        return MRB_GET_IV(transform, "@scale");
}

static MRB_FUNC(Rotation)
{
        GET_TRANSFORM_SAFE;

        return MRB_GET_IV(transform, "@rotation");
}

static MRB_FUNC(SetPosition)
{
        GET_TRANSFORM_SAFE;

        mrb_value position = MRB_GET_IV(transform, "@position");
        auto data = MRB_ISTRUCT_VAL(position, glm::vec2);
        mrb_value arg = MRB_ARG1;
        if (mrb_type(arg) != MRB_TT_ISTRUCT) {
                BOREK_LOG_ERROR("Trying to set size to something other than Vec2");
                return MRB_NIL;
        }
        auto stru = RCAST<RIStruct*>(arg.w);
        if (stru->c != RBDatatypes::vec2_class &&
            stru->c != RBNDatatypes::vec2_class) {
                BOREK_LOG_ERROR("Trying to set size to something other than Vec2");
                return MRB_NIL;
        }

        *data = *RCAST<glm::vec2*>(stru->inline_data);
        ChangesExporter::AddTransform(mrb, self);
        return MRB_NIL;
}

static MRB_FUNC(SetScale)
{
        GET_TRANSFORM_SAFE;

        mrb_value scale = MRB_GET_IV(transform, "@scale");
        auto data = MRB_ISTRUCT_VAL(scale, glm::vec2);
        mrb_value arg = MRB_ARG1;
        if (mrb_type(arg) != MRB_TT_ISTRUCT) {
                BOREK_LOG_ERROR("Trying to set size to something other than Vec2");
                return MRB_NIL;
        }
        auto stru = RCAST<RIStruct*>(arg.w);
        if (stru->c != RBDatatypes::vec2_class &&
            stru->c != RBNDatatypes::vec2_class) {
                BOREK_LOG_ERROR("Trying to set size to something other than Vec2");
                return MRB_NIL;
        }

        *data = *RCAST<glm::vec2*>(stru->inline_data);
        ChangesExporter::AddTransform(mrb, self);
        return MRB_NIL;
}

static MRB_FUNC(SetRotation)
{
        GET_TRANSFORM_SAFE;

        MRB_SET_IV(transform, "@rotation", MRB_ARG1);
        ChangesExporter::AddTransform(mrb, self);
        return MRB_NIL;
}

static MRB_FUNC(Name)
{
        Entity e(mrb_integer(MRB_GET_IV(self, "@entity_id")));
        return mrb_str_new_cstr(mrb, e.GetName());
}

static MRB_FUNC(Children)
{
        return mrb_hash_values(mrb, MRB_GET_IV(self, "@children"));
}

static MRB_FUNC(FindChild)
{
        return mrb_hash_get(mrb, MRB_GET_IV(self, "@children"), MRB_ARG1);
}

static MRB_FUNC(Parent)
{
        return MRB_GET_IV(self, "@parent");
}

static MRB_FUNC(SetParent)
{
        mrb_value eid = MRB_GET_IV(self, "@entity_id");
        Entity e(mrb_integer(eid));
        mrb_value parent = MRB_GET_IV(self, "@parent");
        mrb_value parent_children = Children(mrb, parent);
        mrb_value new_parent = MRB_FUNCALL(MRB_ARG1, "node");
        Entity npe = mrb_integer(MRB_GET_IV(new_parent, "@entity_id"));

        mrb_hash_delete_key(mrb, parent_children, eid);
        MRB_SET_IV(self, "@parent", new_parent);
        mrb_hash_set(mrb, Children(mrb, new_parent), eid, self);
        Application::GetScene()->GetTree().EntityAppendChild(e, npe);
        return MRB_NIL;
}

static MRB_FUNC(Destroy)
{
        Entity e(mrb_integer(MRB_GET_IV(self, "@entity_id")));
        Application::SendEvent<DestroyEntityEvent>(e);
        return MRB_NIL;
}

static MRB_FUNC(ToS)
{
        return Name(mrb, self);
}

static MRB_FUNC(Tags)
{
        GET_TAGS_SAFE;
        return MRB_GET_IV(tags, "@tags");
}

static MRB_FUNC(AddTag)
{
        mrb_sym tag;
        mrb_get_args(mrb, "n", &tag);

        Entity e(mrb_integer(MRB_GET_IV(self, "@entity_id")));
        auto& tc = e.GetComponent<TagComponent>();
        tc.AddTag(mrb_sym_name(mrb, tag));

        MRB_SET_IV(self, "@_tags", MRB_NIL);
        return MRB_NIL;
}

static MRB_FUNC(New)
{
        return RBNode::NewNode(mrb, RBNode::node_class, NodeType::Node);
}

static MRB_FUNC(Node)
{
        return self;
}

static MRB_FUNC(MNode)
{
        return MRB_GET_IV(self, "@_node");
}

NODE_MODULE_METHOD(Position);
NODE_MODULE_METHOD(Scale);
NODE_MODULE_METHOD(Rotation);
NODE_MODULE_METHOD(SetPosition);
NODE_MODULE_METHOD(SetScale);
NODE_MODULE_METHOD(SetRotation);
NODE_MODULE_METHOD(Name);
NODE_MODULE_METHOD(Children);
NODE_MODULE_METHOD(FindChild);
NODE_MODULE_METHOD(Parent);
NODE_MODULE_METHOD(SetParent);
NODE_MODULE_METHOD(Destroy);
NODE_MODULE_METHOD(ToS);
NODE_MODULE_METHOD(Tags);
NODE_MODULE_METHOD(AddTag);

mrb_value
RBNode::Initialize(mrb_state *mrb, mrb_value self)
{
        MRB_SET_IV(self, "@entity_id", MRB_ARG1);
        MRB_SET_IV(self, "@children", mrb_hash_new(mrb));
        return self;
}


void
RBNode::Init(RubyEngine &engine)
{
        auto& vm = engine.GetRubyVM();
        node_class = vm.define_class("NodeC")
                .define_method("initialize", Initialize, FuncArgs().Required(1))
                .define_method("position", Position)
                .define_method("scale", Scale)
                .define_method("rotation", Rotation)
                .define_method("position=", SetPosition, FuncArgs().Required(1))
                .define_method("scale=", SetScale, FuncArgs().Required(1))
                .define_method("rotation=", SetRotation, FuncArgs().Required(1))
                .define_method("name", Name)
                .define_method("children", Children)
                .define_method("find_child", FindChild, FuncArgs().Required(1))
                .define_method("parent", Parent)
                .define_method("parent=", SetParent, FuncArgs().Required(1))
                .define_method("destroy", Destroy)
                .define_method("to_s", ToS)
                .define_method("tags", Tags)
                .define_method("add_tag", AddTag, FuncArgs().Required(1))
                .define_private_method("node", Node);

        node_module = vm.define_module("Node")
                .define_method("position", MPosition)
                .define_method("scale", MScale)
                .define_method("rotation", MRotation)
                .define_method("position=", MSetPosition, FuncArgs().Required(1))
                .define_method("scale=", MSetScale, FuncArgs().Required(1))
                .define_method("rotation=", MSetRotation, FuncArgs().Required(1))
                .define_method("name", MName)
                .define_method("children", MChildren)
                .define_method("find_child", MFindChild, FuncArgs().Required(1))
                .define_method("parent", MParent)
                .define_method("parent=", MSetParent, FuncArgs().Required(1))
                .define_method("destroy", MDestroy)
                .define_method("to_s", MToS)
                .define_method("tags", MTags)
                .define_method("add_tag", MAddTag, FuncArgs().Required(1))
                .define_method("node", MNode)
                .define_module_method("new", New);

        RBDynamicBody::Init(engine);
        RBSprite::Init(engine);
        RBSound::Init(engine);
        RBAnimatedSprite::Init(engine);
        RBText2D::Init(engine);
        RBParticleEmmiter::Init(engine);
        RBTilemap::Init(engine);
        RBCamera::Init(engine);
        RBArea::Init(engine);
}

mrb_value
RBNode::NewNode(mrb_state* mrb, mrbcpp::Class& cls, NodeType type)
{
        auto scene = Application::GetScene();
        SceneTree& tree = scene->GetTree();
        Entity e = scene->NewEntity(Symbol(), type);
        tree.EntityPrependChild(e, tree.GetRootEntity());
        EntityInitializer::InitializeEntity(e);

        mrb_value eid = mrb_int_value(mrb, e.GetId());
        mrb_value instance = mrb_class_new_instance(mrb, 1, &eid, cls);
        MRB_SET_IV(instance, "@parent", { tree.GetRootEntity().GetRubyNode() });
        return instance;
}

Module RBNode::node_module;
Class RBNode::node_class;

}  // namespace RBModules
}  // namespace Borek
