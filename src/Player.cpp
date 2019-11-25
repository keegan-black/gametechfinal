#include "Player.h"
#include "Structure.h"
#include <KinematicCollision.hpp>
#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <InputEventMouseMotion.hpp>
#include <StaticBody.hpp>

using namespace godot;
#define PI 3.14

void Player::_register_methods() {
	register_method("_process", &Player::_process);
	register_method("_physics_process", &Player::_physics_process);
	register_method("_ready", &Player::_ready);
    register_method("_input", &Player::_input);
}

Player::Player() {
}

Player::~Player() {
	// add your cleanup here
}


void Player::_init() {
	input = Input::get_singleton();
	moveSpeed = 10;
	jumpForce = 100;
	velocity = Vector3(0,0,0);
	gravity = Vector3(0,-5,0);
    terminal_velocity = Vector3(moveSpeed,jumpForce,moveSpeed);
}

void Player::_ready(){
    input->set_mouse_mode(Input::MOUSE_MODE_HIDDEN);
}

void Player::_process(float delta) {

}

void Player::_input(InputEvent* event) {
    InputEventMouseMotion *m = cast_to<InputEventMouseMotion>(event);
    if (m != nullptr) {
        float mouse_rotation = -m->get_relative().x * mouse_x_sensitivity * PI/180.0;
        if (me != nullptr) {
            me->rotate_y(mouse_rotation);
            rotation += mouse_rotation;
        }

        float pan = -m->get_relative().y * mouse_y_sensitivity;
        if (camera_angle + pan < 90 && camera_angle + pan > -90 && camera != nullptr) {
            camera->rotate_x(pan * PI/180.0);
            camera_angle += pan;
        }
    }
}

void Player::_physics_process(float delta) {
    me = Object::cast_to<KinematicBody>(get_node("KinematicBody"));
    camera = Object::cast_to<Camera>(get_node("KinematicBody/Camera"));
    ray = Object::cast_to<RayCast>(get_node("KinematicBody/Camera/RayCast"));
    FrontDirection front_direction = FrontDirection::None;
    if (input->is_key_pressed(87)) {
        front_direction = FrontDirection::Forward;
    }
    if (input->is_key_pressed(83)) {
        front_direction = FrontDirection::Backward;
    }
    SideDirection side_direction = SideDirection::None;
    if (input->is_key_pressed(65)) {
        side_direction = SideDirection::Left;
    }
    if (input->is_key_pressed(68)) {
        side_direction = SideDirection::Right;
    }

    MovementAction move_action = MovementAction::None;
    if (input->is_key_pressed(32)) {
        move_action = MovementAction::Jump;
    }

    Action action = Action::None;
    if (input->is_key_pressed(70)) {
        if (!is_shooting) {
            is_shooting = true;
            action = Action::Shoot;
        }
    } else {
        is_shooting = false;
    }

    _perform_action(action);
    _move(front_direction, side_direction, move_action);
	
	
}

void Player::_perform_action(Player::Action action) {
    if (action == Action::Shoot) {
        _shoot();
    }
}

void Player::_shoot() {
    Godot::print("Shooting!");
    if (ray == nullptr) {
        Godot::print("Ray not found");
        return;
    }

    ray->set_collide_with_areas(false);
    ray->set_collide_with_bodies(true);

    ray->set_enabled(true);

    ray->set_cast_to(Vector3(0,0,-20));
    ray->force_raycast_update();

    if (ray->is_colliding()) {
        Object* obj = ray->get_collider();
        StaticBody* body = Object::cast_to<StaticBody>(obj);
        if (body != nullptr) {
            Structure* structure = Node::cast_to<Structure>(body->get_parent());
            if (structure != nullptr) {
                Godot::print("Wall Hit");
                structure->_take_damage(20);
            } else {
                Godot::print("Non Structure Hit");
            }
        } 
    }

    ray->set_enabled(false);

}

void Player::_move(Player::FrontDirection front_direction, Player::SideDirection side_direction, Player::MovementAction move_action) {
    // Compute force vector based on current state

    Vector3 forceVector = Vector3(0,0,0);
    Vector3 curr_gravity = gravity;

    handle_movement(forceVector, front_direction,side_direction);
    handle_move_action(forceVector, move_action);
    handle_gravity(forceVector, curr_gravity);

    //velocity += forceVector.rotated(Vector3(0,1,0),current_rotation);
    velocity += forceVector.rotated(Vector3(0,1,0),rotation);
    // velocity += forceVector;
    if (velocity.x > terminal_velocity.x) { velocity.x = terminal_velocity.x;}
    if (velocity.y > terminal_velocity.y) { velocity.y = terminal_velocity.y;}
    if (velocity.z > terminal_velocity.z) { velocity.z = terminal_velocity.z;}

    if (velocity.x < -1 * terminal_velocity.x) { velocity.x = -1 * terminal_velocity.x;}
    if (velocity.y < -1 * terminal_velocity.y) { velocity.y = -1 * terminal_velocity.y;}
    if (velocity.z < -1 * terminal_velocity.z) { velocity.z = -1 * terminal_velocity.z;}

    velocity = me->move_and_slide(velocity, Vector3(0,1,0));
    
    velocity = velocity.linear_interpolate(Vector3(0,0,0),.2);
}

void Player::handle_movement(Vector3& force, Player::FrontDirection front_direction, Player::SideDirection side_direction) {
	switch (front_direction)
    {
    case FrontDirection::Forward:
        force.z -= moveSpeed;
        break;
    case FrontDirection::Backward:
        force.z += moveSpeed;
        break;
    default:
        break;
    }

    switch (side_direction)
    {
    case SideDirection::Left:
        force.x -= moveSpeed;
        break;
    case SideDirection::Right:
        force.x += moveSpeed;
        break;
    default:
        break;
    }
}

void Player::handle_move_action(Vector3& force, Player::MovementAction move_action) {
    if(me->is_on_floor() && move_action == MovementAction::Jump) {
		force.y += jumpForce;
	}
}

void Player::handle_gravity(Vector3& force, Vector3& curr_gravity) {
    force += curr_gravity;
}

