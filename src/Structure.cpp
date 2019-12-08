#include "Structure.h"
#include <KinematicCollision.hpp>
#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <InputEventMouseMotion.hpp>
#include <StaticBody.hpp>
#include "GridBlock.h"
#include "GameController.h"

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
        GridBlock* block = Node::cast_to<GridBlock>(get_parent());
        Vector3 location = block->get_global_transform().get_origin();
        GameController* gameController = Node::cast_to<GameController>(get_node("/root/GameController"));
        
        if (block != nullptr && type == Type::Ramp) {
            block->ramp = nullptr;
        }
        get_parent()->remove_child(this);

        if (gameController != nullptr) {
            gameController->_check_gridblock_and_delete(location);
        }
        //this->queue_free();
    }
}

void Structure::_set_type(Type type) {
    this->type = type;
}

void Structure::_set_orientation(Orientation orientation) {
    this->orientation = orientation;
}

Structure::Orientation Structure::_get_orientation() {
    return this->orientation;
}

Structure::Type Structure::_get_type() {
    return this->type;
}

void Structure::_ready(){
}

void Structure::_process(float delta) {

}
