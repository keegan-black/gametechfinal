#include "Player.h"
#include "Structure.h"
#include <KinematicCollision.hpp>
#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <InputEventMouseMotion.hpp>
#include <StaticBody.hpp>
#include <SceneTree.hpp>
#include <Viewport.hpp>

using namespace godot;
#define PI 3.14
#define GRID_SIZE 6

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
	moveSpeed = 10;
	jumpForce = 100;
	velocity = Vector3(0,0,0);
	gravity = Vector3(0,-5,0);
    terminal_velocity = Vector3(moveSpeed,jumpForce,moveSpeed);
}

void Player::_ready(){
    input = Input::get_singleton();
    input->set_mouse_mode(Input::MOUSE_MODE_HIDDEN);
    me = Object::cast_to<KinematicBody>(get_node("KinematicBody"));
    camera = Object::cast_to<Camera>(get_node("KinematicBody/Camera"));
    ray = Object::cast_to<RayCast>(get_node("KinematicBody/Camera/RayCast"));
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

    FrontDirection front_direction = FrontDirection::None;

    if (input == nullptr) {
        Godot::print("No Input Found");
        return;
    }

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
    if (input->is_key_pressed(84)) { // T
        if (!is_shooting) {
            is_shooting = true;
            action = Action::Shoot;
        }
    } else {
        is_shooting = false;
    }

    if (input->is_key_pressed(82)) { // R
        if (!is_building) {
            is_building = true;
            action = Action::Build_Wall;
        }  
    } else if(input->is_key_pressed(70)) { // F
        if (!is_building) {
            is_building = true;
            action = Action::Build_Ramp;
        }  
    } else if(input->is_key_pressed(69)) { // E
        if (!is_building) {
            is_building = true;
            action = Action::Build_Floor;
        }  
    } else {
        is_building = false;
    }

    _perform_action(action);
    _move(front_direction, side_direction, move_action);
	
	
}

void Player::_perform_action(Player::Action action) {
    if (action == Action::Shoot) {
        _shoot();
    }
    if (action == Action::Build_Wall || action == Action::Build_Ramp || action == Action::Build_Floor) {
        _build(action);
    }
}

float Player::_get_grid_rotation() {
    float rot_degrees = rotation * 180/PI;
    int current_rotation = ((int) rot_degrees) % 360;
    if (current_rotation < 0) {
        current_rotation += 360;
    }

    if (current_rotation >= 0 && current_rotation < 45) {
        return 0;
    } else if (current_rotation >= 45 && current_rotation < 135) {
        return 90;
    } else if (current_rotation >= 135 && current_rotation < 225) {
        return 180;
    } else if (current_rotation >= 225 && current_rotation < 315) {
        return 270;
    }
    return 0;
}

void Player::_build(Player::Action action) {
    if (ray == nullptr) {
        Godot::print("Ray not found");
        return;
    }

    ray->set_collide_with_areas(true);
    ray->set_collide_with_bodies(true);

    ray->set_enabled(true);

    ray->set_cast_to(Vector3(0,0,-10));
    ray->force_raycast_update();

    if (ray->is_colliding()) {
        Object* obj = ray->get_collider();
        if (obj == nullptr) {
            return;
        }
        StaticBody* body = Object::cast_to<StaticBody>(obj);
        Area* area = Object::cast_to<Area>(obj);
        if (body != nullptr) {
            if (body->get_name() == "FloorStaticBody") {
                _create_grid_block_at(ray->get_collision_point(), action);
            }
        } else if (area != nullptr) {
            GridBlock* gridBlock = Node::cast_to<GridBlock>(area->get_parent());
            if (gridBlock != nullptr) {
                _build_in_grid_block(gridBlock, action, ray->get_collision_point());
            }
        }
    }

    ray->set_enabled(false);
}

void Player::_create_grid_block_at(Vector3 floor_location, Action action) {
    Vector3 new_location = _to_grid_coordinate(floor_location);
    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();

    if (resourceLoader == nullptr) {
        Godot::print("[ERROR] Resource Loader Null");
        return;
    }

    Ref<PackedScene> gridScene = resourceLoader->load("res://GridBlock.tscn");
    GridBlock* gridBlock = Node::cast_to<GridBlock>(gridScene->instance());

    if (gridBlock == nullptr) {
        Godot::print("[ERROR] Gridblock is null");
        return;
    }

    get_tree()->get_root()->add_child(gridBlock);
    gridBlock->set_global_transform(Transform(gridBlock->get_global_transform().basis,new_location));
    _build_in_grid_block(gridBlock, action, Vector3(0,0,0));
}

void Player::_build_in_grid_block(GridBlock* gridBlock, Action action, Vector3 collision_location) {

    GridBlock::Direction direction = GridBlock::Direction::Front;
    float rot = _get_grid_rotation();
    if (rot == 0.0f) { direction = GridBlock::Direction::Front;}
    if (rot == 90.0f) { direction = GridBlock::Direction::Left;}
    if (rot == 180.0f) { direction = GridBlock::Direction::Back;}
    if (rot == 270.0f) { direction = GridBlock::Direction::Right;}

    gridBlock->_face_at_global_point(collision_location);

    switch (action)
    {
    case Action::Build_Ramp:
        gridBlock->_add_ramp(direction);
        break;
    case Action::Build_Wall:
        gridBlock->_add_wall(direction);
        break;
    case Action::Build_Floor:
        gridBlock->_add_floor();
        break;
    default:
        break;
    }
}

void Player::_shoot() {
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
            Structure* structure = Node::cast_to<Structure>(body->get_parent()->get_parent());
            if (structure != nullptr) {
                structure->_take_damage(20);
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

    velocity = me->move_and_slide(velocity, Vector3(0,1,0),true);
    
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

Vector3 Player::_to_grid_coordinate(Vector3 location) {
    Vector3 ret = Vector3(0,0,0);
    ret.x = ((int) location.x / GRID_SIZE) * GRID_SIZE + (location.x/abs(location.x) * GRID_SIZE/2);
    ret.y = ((int) location.y / GRID_SIZE) * GRID_SIZE + (location.y/abs(location.y) * GRID_SIZE/2);
    ret.z = ((int) location.z / GRID_SIZE) * GRID_SIZE + (location.z/abs(location.z) * GRID_SIZE/2);
    return ret;
}

