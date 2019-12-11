#include "Tower.h"
#include "GameController.h"

using namespace godot;
#define PI 3.14

void Tower::_register_methods() {
	register_method("_process", &Tower::_process);
	register_method("_ready", &Tower::_ready);
}

Tower::Tower() {
}

Tower::~Tower() {
	// add your cleanup here
}

void Tower::_init() {
}

void Tower::_ready(){
	GameController* gameController = Node::cast_to<GameController>(get_node("/root/GameController"));
	if (gameController != nullptr) {
		gameController->_tower_health_signal(this);
	}
}

void Tower::_take_damage(float damage) {
	health -= damage;
	Godot::print(std::to_string(health).c_str());
	GameController* gameController = Node::cast_to<GameController>(get_node("/root/GameController"));
	if (gameController != nullptr) {
		gameController->_tower_health_signal(this);
		
		if (health <= 0) {
			gameController->_tower_death_signal(this);
		}	
	}

	
}

void Tower::_process(float delta) {
}