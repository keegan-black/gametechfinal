#include "Materials.h"
#include "Player.h"

using namespace godot;

void Materials::_register_methods() {
	register_method("_process", &Materials::_process);
    register_method("_body_entered", &Materials::_body_entered);
	register_method("_ready", &Materials::_ready);
}

Materials::Materials() {
}

Materials::~Materials() {
	// add your cleanup here
}

void Materials::_init() {
}

void Materials::_ready() {
    this->get_node("MeshInstance/Area")->connect("body_entered", this, "_body_entered");
}

void Materials::_body_entered(Node *other) {
    Player* player = Node::cast_to<Player>(other->get_parent());
    if (player != nullptr) {
        player->_pickup_materials(50);
        this->queue_free();
    }
}

void Materials::_process(float delta) {
}