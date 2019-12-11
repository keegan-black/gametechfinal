#include "Menu.h"
#include "GameController.h"
#include "SceneTree.hpp"

using namespace godot;

void Menu::_register_methods() {
	register_method("_ready", &Menu::_ready);
    register_method("_start_game", &Menu::_start_game);
}

Menu::Menu() {
}

Menu::~Menu() {
	// add your cleanup here
}

void Menu::_init() {
}

void Menu::_ready() {

    this->get_node("VBoxContainer/VBoxContainer/Button")->connect("pressed", this, "_start_game");
}

void Menu::_start_game() {
    get_tree()->change_scene("res://Main.tscn");

}