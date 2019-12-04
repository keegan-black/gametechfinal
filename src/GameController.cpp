#include "GameController.h"

using namespace godot;
#define PI 3.14

void GameController::_register_methods() {
	register_method("_process", &GameController::_process);
	register_method("_ready", &GameController::_ready);
}

GameController::GameController() {
}

GameController::~GameController() {
	// add your cleanup here
}

void GameController::_init() {
}

void GameController::_ready(){
}

void GameController::_process(float delta) {
}