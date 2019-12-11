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
	round_label = cast_to<Label>(get_node("Round/VBoxContainer/RoundLabel"));
	ammo_label = cast_to<Label>(get_node("PlayerItems/VBoxContainer/HBoxAmmo/AmmoLabel"));
	materials_label = cast_to<Label>(get_node("PlayerItems/VBoxContainer/HBoxMaterials/MaterialsLabel"));
	player_health_bar = cast_to<ProgressBar>(get_node("PlayerHealth/VBoxContainer/PlayerHealthBar"));
	zombie_tower_health_bar = cast_to<ProgressBar>(get_node("TowerHealth/VBoxContainer/ZombieTowerHealthBar"));
	player_tower_health_bar = cast_to<ProgressBar>(get_node("TowerHealth/VBoxContainer/PlayerTowerHealthBar"));
	_set_materials_label(100);
	_set_ammo_label(20);
	_set_zombie_tower_health_bar(100);
	_set_player_tower_health_bar(100);
	_set_player_health_bar(100);
	_set_round_label(1);
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

void GUI::_set_zombie_tower_health_bar(float health) {
	if (zombie_tower_health_bar != nullptr) {
		zombie_tower_health_bar->set_value(health);
	}
}
void GUI::_set_player_tower_health_bar(float health) {
	if (player_tower_health_bar != nullptr) {
		player_tower_health_bar->set_value(health);
	}
}
void GUI::_set_player_health_bar(float health) {
	if (player_health_bar != nullptr) {
		player_health_bar->set_value(health);
	}
}

void GUI::_set_round_label(int round) {
	if (round_label != nullptr) {
		round_label->set_text(std::to_string(round).c_str());
	}
}

void GUI::_process(float delta) {
}