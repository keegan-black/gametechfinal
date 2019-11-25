#include "Structure.h"
#include <KinematicCollision.hpp>
#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <InputEventMouseMotion.hpp>
#include <StaticBody.hpp>
#include "GridBlock.h"

using namespace godot;
#define PI 3.14

void Structure::_register_methods() {
	register_method("_process", &Structure::_process);
	register_method("_ready", &Structure::_ready);
    register_method("_take_damage", &Structure::_take_damage);
}

Structure::Structure() {
}

Structure::~Structure() {
	// add your cleanup here
}


void Structure::_init() {
	
}

void Structure::_take_damage(float damage) {
    health -= damage;
    if (health < 0) {
        GridBlock* gridBlock = Node::cast_to<GridBlock>(this->get_parent());
        if (gridBlock != nullptr) {
            gridBlock->_clear_structure_pointer_if_exists(this);
        }
        free();
    }
}

void Structure::_ready(){
}

void Structure::_process(float delta) {

}
