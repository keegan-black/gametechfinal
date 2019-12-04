#include "Zombie.h"
#include "GameController.h"

using namespace godot;

void Zombie::_register_methods() {
	register_method("_process", &Zombie::_process);
	register_method("_physics_process", &Zombie::_physics_process);
	register_method("_ready", &Zombie::_ready);
    register_method("_take_damage", &Zombie::_take_damage);
}

Zombie::Zombie() {
}

Zombie::~Zombie() {
	// add your cleanup here
}

void Zombie::_take_damage(float damage) {
    health -= damage;
    if (health < 0) {
        this->queue_free();
    }
}

void Zombie::_init() {
	moveSpeed = 5;
    target = Vector3(0,0,0);
	velocity = Vector3(0,0,0);
	gravity = Vector3(0,-5,0);
    terminal_velocity = Vector3(moveSpeed,moveSpeed,moveSpeed);
}

void Zombie::_ready(){
    me = Object::cast_to<KinematicBody>(get_node("KinematicBody"));
}

void Zombie::_physics_process(float delta) {
    _move();
}

void Zombie::_move() {
    Vector3 forceVector = Vector3(0,0,0);
    Vector3 curr_gravity = gravity;

    handle_movement(forceVector);
    handle_gravity(forceVector, curr_gravity);

    //velocity += forceVector.rotated(Vector3(0,1,0),current_rotation);
    velocity += forceVector;
    // velocity += forceVector;
    if (velocity.x > terminal_velocity.x) { velocity.x = terminal_velocity.x;}
    if (velocity.y > terminal_velocity.y) { velocity.y = terminal_velocity.y;}
    if (velocity.z > terminal_velocity.z) { velocity.z = terminal_velocity.z;}

    if (velocity.x < -1 * terminal_velocity.x) { velocity.x = -1 * terminal_velocity.x;}
    if (velocity.y < -1 * terminal_velocity.y) { velocity.y = -1 * terminal_velocity.y;}
    if (velocity.z < -1 * terminal_velocity.z) { velocity.z = -1 * terminal_velocity.z;}

    velocity = me->move_and_slide(velocity, Vector3(0,1,0),true);
    
    velocity = velocity.linear_interpolate(Vector3(0,0,0),.2);
}

void Zombie::handle_gravity(Vector3& force, Vector3& gravity) {
    force += gravity;
}

void Zombie::handle_movement(Vector3& force) {

    Vector3 current_location = me->get_global_transform().get_origin();

    if (abs(current_location.distance_to(Vector3(target.x,current_location.y,target.z))) < 2) {
        return;
    }

    Vector3 move_direction = (target-current_location).normalized();

    force += move_direction * Vector3(moveSpeed,0,moveSpeed);
}

void Zombie::_process(float delta) {
    GameController* gameController = Node::cast_to<GameController>(get_node("/root/GameController"));
    if (gameController != nullptr && gameController->playerTower != nullptr) {
        target = gameController->playerTower->get_global_transform().get_origin();
    }
}

void Zombie::_set_target(Vector3 target) {
    this->target = target;
}