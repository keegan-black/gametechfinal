#include "Tower.h"

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
}

void Tower::_process(float delta) {
}