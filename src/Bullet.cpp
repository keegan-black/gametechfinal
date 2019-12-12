#include "Bullet.h"

using namespace godot;

void Bullet::_register_methods() {
	register_method("_process", &Bullet::_process);
	register_method("_ready", &Bullet::_ready);
}

Bullet::Bullet() {
}

Bullet::~Bullet() {
	// add your cleanup here
}

void Bullet::_init() {
}

void Bullet::_ready() {
}

void Bullet::_set_direction_towards(Vector3 location) {
    direction = location.normalized();
}

void Bullet::_process(float delta) {
    lifetime -= delta;
    if (lifetime < 0) {
        this->queue_free();
    } else {
        Vector3 current_location = get_global_transform().get_origin();
        Basis b = get_global_transform().get_basis();
        set_global_transform(Transform(b,current_location + (direction*speed*delta)));
    }
}