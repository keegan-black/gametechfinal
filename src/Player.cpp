#include "Player.h"
#include "Structure.h"
#include "Zombie.h"
#include <KinematicCollision.hpp>
#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <InputEventMouseMotion.hpp>
#include <StaticBody.hpp>
#include <SceneTree.hpp>
#include <Viewport.hpp>
#include <GameController.h>
#include "GUI.h"

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
    placeholder = Node::cast_to<Spatial>(get_node("Placeholder"));

    //_update_GUI();
}

void Player::_process(float delta) {
    _update_placeholder();
}

void Player::_update_GUI() {
    GUI* gui = Node::cast_to<GUI>(get_node("/root/Spatial/GUI"));
    if (gui != nullptr) {
        gui->_set_ammo_label(ammo);
        gui->_set_materials_label(materials);
        gui->_set_player_health_bar(health);
    }
}

void Player::_take_damage(int damage) {
    health -= damage;
    _update_GUI();
}

void Player::_update_placeholder() {
    if (placeholder == nullptr) {
        return;
    }
    placeholder->hide();
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
                Vector3 location = _to_grid_coordinate(ray->get_collision_point());
                Basis basis = placeholder->get_global_transform().get_basis();
                placeholder->set_global_transform(Transform(basis,location));
                placeholder->show();
            } else {
                Vector3 location = _to_grid_coordinate(ray->get_collision_point());
                Basis basis = placeholder->get_global_transform().get_basis();
                placeholder->set_global_transform(Transform(basis,location));
                placeholder->show();
            }
        } else if (area != nullptr) {
            GridBlock* gridBlock = Node::cast_to<GridBlock>(area->get_parent());
            if (gridBlock != nullptr) {
                GridBlock::Direction face_direction = gridBlock->_face_at_global_point(ray->get_collision_point());
                Vector3 gridBlock_location = gridBlock->get_global_transform().get_origin();
                Vector3 build_location = gridBlock_location;
                if (face_direction == GridBlock::Direction::Top) {
                    build_location.y += 6;
                }
                Basis basis = placeholder->get_global_transform().get_basis();
                placeholder->set_global_transform(Transform(basis,build_location));
                placeholder->show();
            }
        }
    }

    ray->set_enabled(false);
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
    BuildType buildType = BuildType::None;

    if (input->is_key_pressed(84)) { // T
        if (!is_shooting) {
            is_shooting = true;
            action = Action::Shoot;
        }
    } else {
        is_shooting = false;
    }

    if (input->is_key_pressed(67)) {
        if (!is_hitting) {
            is_hitting = true;
            action = Action::Melee;
        }
    } else {
        is_hitting = false;
    }

    if (input->is_key_pressed(82)) { // R
        if (!is_building) {
            is_building = true;
            action = Action::Build;
            buildType = BuildType::Wall;
        }  
    } else if(input->is_key_pressed(70)) { // F
        if (!is_building) {
            is_building = true;
            action = Action::Build;
            buildType = BuildType::Ramp;
        }  
    } else if(input->is_key_pressed(69)) { // E
        if (!is_building) {
            is_building = true;
            action = Action::Build;
            buildType = BuildType::Floor;
        }  
    } else {
        is_building = false;
    }

    _perform_action(action, buildType);
    _move(front_direction, side_direction, move_action);
	
	
}

void Player::_perform_action(Player::Action action, Player::BuildType buildType) {
    if (action == Action::Shoot && ammo > 0) {
        ammo -= 1;
        _update_GUI();
        _shoot();
    }
    if (action == Action::Melee) {
        _melee();
    }
    if (action == Action::Build && buildType != BuildType::None && materials > 5) {
        
        if (_build(buildType)) {
            materials -= 5;
            _update_GUI();
        }
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

float Player::_get_grid_tilt() {
    float angle = camera_angle;
    if (angle > -45 && angle < 45) {
        return 0;
    }
    if (angle < -45) {
        return -90;
    }
    if (angle > 45) {
        return 90;
    }
    return 0;
}

Player::Facing Player::_get_facing_with_tilt() {
    float rotation = _get_grid_rotation();
    float tilt = _get_grid_tilt();

    if (tilt != 0) {
        if (tilt == -90) { return Facing::Down;}
        if (tilt == 90) { return Facing::Up;}
        return Facing::Down;
    } else {
        return _get_facing_no_tilt();
    }
}
Player::Facing Player::_get_facing_no_tilt() {
    float rotation = _get_grid_rotation();
    float tilt = _get_grid_tilt();

    if (rotation == 0.0F) { return Facing::Front;}
    if (rotation == 90.0F) { return Facing::Left;}
    if (rotation == 180.0F) { return Facing::Back;}
    if (rotation == 270.0F) { return Facing::Right;}

    return Facing::Front;

}

bool Player::_build(Player::BuildType buildType) {
    GameController* gameController = Node::cast_to<GameController>(get_node("/root/GameController"));
    if (gameController == nullptr) {
        Godot::print("[Error] Game Controller Null");
        return false;
    }

    Structure::Type type = Structure::Type::Wall;
    switch (buildType) {
    case BuildType::Wall:
        type = Structure::Type::Wall;
        break;
    case BuildType::Floor:
        type = Structure::Type::Floor;
        break;
    case BuildType::Ramp:
        type = Structure::Type::Ramp;
        break;
    default:
        break;
    }

    Facing facing = _get_facing_no_tilt();
    GridBlock::Direction user_direction = GridBlock::Direction::Front;
    switch (facing)
    {
    case Facing::Front:
        user_direction = GridBlock::Direction::Front;
        break;
    case Facing::Back:
        user_direction = GridBlock::Direction::Back;
        break;
    case Facing::Left:
        user_direction = GridBlock::Direction::Left;
        break;
    case Facing::Right:
        user_direction = GridBlock::Direction::Right;
        break;
    default:
        break;
    }

    if (ray == nullptr) {
        Godot::print("Ray not found");
        return false;
    }

    ray->set_collide_with_areas(true);
    ray->set_collide_with_bodies(true);

    ray->set_enabled(true);

    ray->set_cast_to(Vector3(0,0,-10));
    ray->force_raycast_update();

    bool built = false;

    if (ray->is_colliding()) {
        Object* obj = ray->get_collider();
        if (obj == nullptr) {
            return false;
        }
        StaticBody* body = Object::cast_to<StaticBody>(obj);
        Area* area = Object::cast_to<Area>(obj);
        if (body != nullptr) {
            if (body->get_name() == "FloorStaticBody") {
                built = gameController->_add_structure(type,user_direction,ray->get_collision_point(),true);
            } else {
                Structure* structure = Node::cast_to<Structure>(body->get_parent()->get_parent());
                if (structure != nullptr) {
                    // _build_click_on_structure(buildType, ray->get_collision_point(), structure);
                    built = gameController->_add_structure(type,user_direction,ray->get_collision_point(),false);
                }
            }
        } else if (area != nullptr) {
            GridBlock* gridBlock = Node::cast_to<GridBlock>(area->get_parent());
            if (gridBlock != nullptr) {
                GridBlock::Direction face_direction = gridBlock->_face_at_global_point(ray->get_collision_point());
                Vector3 gridBlock_location = gridBlock->get_global_transform().get_origin();
                Vector3 build_location = gridBlock_location;
                if (face_direction == GridBlock::Direction::Top) {
                    build_location.y += 6;
                }
                built = gameController->_add_structure(type,user_direction,build_location,false);

            }
        }
    }

    ray->set_enabled(false);
    return built;
}

Vector3 Player::_get_nearest_neighbor_gridBlock_from(Vector3 location) {
    Facing facing = _get_facing_with_tilt();
    Vector3 next_door_block = location;
    switch (facing)
    {
    case Facing::Front:
        next_door_block.z += 3;
        break;
    case Facing::Back:
        next_door_block.z -= 3;
        break;
    case Facing::Left:
        next_door_block.x += 3;
        break;
    case Facing::Right:
        next_door_block.z -= 3;
        break;
    case Facing::Up:
        next_door_block.y -= 3;
        break;
    case Facing::Down:
        next_door_block.y += 3;
        break;
    default:
        break;
    }

    return next_door_block;
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
        KinematicBody* kin = Object::cast_to<KinematicBody>(obj);
        if (kin != nullptr) {
            Zombie* zombie = Node::cast_to<Zombie>(kin->get_parent());
            if (zombie != nullptr) {
                zombie->_take_damage(20);
            }
        }
    }

    ray->set_enabled(false);

}

void Player::_melee() {
    if (ray == nullptr) {
        Godot::print("Ray not found");
        return;
    }

    ray->set_collide_with_areas(false);
    ray->set_collide_with_bodies(true);

    ray->set_enabled(true);

    ray->set_cast_to(Vector3(0,0,-2));
    ray->force_raycast_update();

    if (ray->is_colliding()) {
        Object* obj = ray->get_collider();
        StaticBody* body = Object::cast_to<StaticBody>(obj);
        if (body != nullptr) {
            Structure* structure = Node::cast_to<Structure>(body->get_parent()->get_parent());
            if (structure != nullptr) {
                Vector3 location = structure->get_global_transform().get_origin();
                structure->_take_damage(100);
            }
        } 
        KinematicBody* kin = Object::cast_to<KinematicBody>(obj);
        if (kin != nullptr) {
            Zombie* zombie = Node::cast_to<Zombie>(kin->get_parent());
            if (zombie != nullptr) {
                zombie->_take_damage(20);
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

