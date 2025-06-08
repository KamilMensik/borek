# BOREK ECS  
  
ECS system by Kamil Mensik (kamilekmensik@gmail.com)  
Made for Borek game engine. (https://github.com/KamilMensik/borek)  
Copyright 2024-2025 <kamilekmensik@gmail.com>  


## ECS::World  
> World is storing all ECS data.  
> Cleans itself upon deletion.  

---
==> Creating a world:  
```  
// Creates a new ECS world  
ECS::World world;
```
---


## ECS::Entity
---
> Entities are composed from components.
> They itself do not store any data. (Entity is just an uint32_t)

---
==> Creating entities

```
// Creating an empty entity  
ECS::Entity entity = world.entity();  
  
// Creating entity with components already registered  
// Passing raw component list is supported, but not recommended.  
// This is exponentially slower than using the second variant.  
ECS::Entity entity = world.entity({ Sprite, Transform });  
  
// Creating an entity using a predetermined archetype.  
ECS::Entity entity = world.entity(PlayerArchetype);  
```
---
==> Removing an entity:  
```  
 Removes the entity and its components.  
world.remove(entity);  
```
---
==> Manipulating the components of entity
  
```
// Setting components: (overriding them)  
// There are two ways to do it:  
// The slow variant. That should not be used.  
world.set_components(entity, { componentA, componentB});  
  
// The exponentially faster variant, that is much preffered.  
world.set_components(entity, PlayerArchetype);  
  
// Adding a single component:  
// This is O(1), Slower if world needs to generate new Archetype.  
world.add_component(entity, componentA);  
```
---
==> Removing components from entity  
  
```
// Removes a single component.  
// This is O(1), Slower if world needs to generate new Archetype.  
world.remove_component(entity, componentA);  
```
---
==> Working with components  
  
```
// Checking whether entity has a component
world.has_component(entity, transform);

// Getting a component
world.get_component(entity, transform) => returns void* to component

// Automatically casting component to specific structure
// This way you can manipulate components without explicit set methods
world.get_component<Transform>(entity, transform) => returns Transform&
```
---

## ECS::Component  
---
> Used to store data of entity.  
> Creating a component doesnt automatically use memory.  
> Memory is used only when it is a part of Archetype (Entity)  
  
---
==> Creating a component
```
// Defining a struct for the component layout  
struct Position {  
    int x, int y;  
};  
  
// Returns a new component that represents the Position struct;  
ECS::Component component = world.component<Position>();  
  
// Create components with implicitly specified information  
// about the data that is being stored inside the component.  
world.component(data_size, data_alignment);  
```
  
******************************WARNING**********************************  
```
// p1 and p2 are completely different components.  
// They just have the same memory layout.  
// If you wish to reuse components, you need to store them.  
  
ECS::Component p1 = world.component<Position>;  
ECS::Component p2 = world.component<Position>;  
(p1 == p2) => returns false  
```

******************************WARNING**********************************  

---
==> Removing components
  
```
// You can remove components from entities.  
// But removing components completely is not supported.  
```
---
  
  
## ECS::Archetype  
---
> Archetype is a group of entities with the same component layout.  
> Components are position independent  
> `({ Transform, HP} == { HP, Transform}) => true`  
---
==> Creating a new archetype  
  
```
// Creates a new / returns an existing archetype for given components  
// Really slow operation. If you want to reuse it. Save it somewhere.  
ECS::Archetype enemy = world.get_archetype({ HP, Transform, Sprite });  
```
