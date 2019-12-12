#include "Ammo.h"
#include "Player.h"

using namespace godot;

void Ammo::_register_methods() {
	register_method("_process", &Ammo::_process);
    register_method("_body_entered", &Ammo::_body_entered);
	register_method("_ready", &Ammo::_ready);
}

Ammo::Ammo() {
}

Ammo::~Ammo() {
	// add your cleanup here
}

void Ammo::_init() {
}

void Ammo::_ready() {
    this->get_node("MeshInstance/Area")->connect("body_entered", this, "_body_entered");
}

void Ammo::_body_entered(Node *other) {
    Player* player = Node::cast_to<Player>(other->get_parent());
    if (player != nullptr) {
        player->_pickup_ammo(10);
        this->queue_free();
    }
}

void Ammo::_process(float delta) {
}