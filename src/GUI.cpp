#include "GUI.h"

using namespace godot;
#define PI 3.14

void GUI::_register_methods() {
	register_method("_process", &GUI::_process);
	register_method("_ready", &GUI::_ready);
}

GUI::GUI() {
}

GUI::~GUI() {
	// add your cleanup here
}

void GUI::_init() {
}

void GUI::_ready(){
	ammo_label = cast_to<Label>(get_node("MarginContainer/VBoxContainer/HBoxAmmo/AmmoLabel"));
	materials_label = cast_to<Label>(get_node("MarginContainer/VBoxContainer/HBoxMaterials/MaterialsLabel"));
	_set_materials_label(100);
	_set_ammo_label(20);
}

void GUI::_set_materials_label(int materials) {
	if (materials_label != nullptr) {
		materials_label->set_text(std::to_string(materials).c_str());
	}
}

void GUI::_set_ammo_label(int ammo) {
	if (ammo_label != nullptr) {
		ammo_label->set_text(std::to_string(ammo).c_str());
	}
}

void GUI::_process(float delta) {
}