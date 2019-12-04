#include <Godot.hpp>
#include "Player.h"
#include "Structure.h"
#include "GridBlock.h"
#include "Zombie.h"
#include "GameController.h"

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
    godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
    godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
    godot::Godot::nativescript_init(handle);

   godot::register_class<godot::Player>();
   godot::register_class<godot::Structure>();
   godot::register_class<godot::GridBlock>();
   godot::register_class<godot::Zombie>();
   godot::register_class<godot::GameController>();
}
