#include "Health.h"
#include "Player.h"

using namespace godot;

void Health::_register_methods() {
	register_method("_process", &Health::_process);
    register_method("_body_entered", &Health::_body_entered);
	register_method("_ready", &Health::_ready);
}

Health::Health() {
}

Health::~Health() {
	// add your cleanup here
}

void Health::_init() {
}

void Health::_ready() {
    this->get_node("MeshInstance/Area")->connect("body_entered", this, "_body_entered");
}

void Health::_body_entered(Node *other) {
    Player* player = Node::cast_to<Player>(other->get_parent());
    if (player != nullptr) {
        player->_pickup_health(10);
        this->queue_free();
    }
}

void Health::_process(float delta) {
}