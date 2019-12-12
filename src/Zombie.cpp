#include "Zombie.h"
#include "Structure.h"
#include "StaticBody.hpp"
#include "Health.h"
#include "Materials.h"
#include "Ammo.h"

#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <Viewport.hpp>

using namespace godot;
#define GRID_SIZE 6

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
        // Drop Table
        ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
        Node* node = nullptr;
        int dropItem = rand()%(1-10 + 1) + 1;
        if (dropItem > 7) {
            Ref<PackedScene> healthScene = resourceLoader->load("res://HealthItem.tscn");
            node = healthScene->instance();
            Health* health = Node::cast_to<Health>(node);
            if (health != nullptr) {
                get_node("/root/Spatial")->add_child(node);
                health->set_global_transform(Transform(health->get_global_transform().get_basis(), me->get_global_transform().get_origin()));
            }
        } else if (dropItem > 5) {
            Ref<PackedScene> ammoScene = resourceLoader->load("res://AmmoItem.tscn");
            node = ammoScene->instance();
            Ammo* ammo = Node::cast_to<Ammo>(node);
            if (ammo != nullptr) {
                get_node("/root/Spatial")->add_child(node);
                ammo->set_global_transform(Transform(ammo->get_global_transform().get_basis(), me->get_global_transform().get_origin()));
            }

        } else  if (dropItem > 3) {
            Ref<PackedScene> materialScene = resourceLoader->load("res://MaterialsItem.tscn");
            node = materialScene->instance();
            Materials* material = Node::cast_to<Materials>(node);
            if (material != nullptr) {
                get_node("/root/Spatial")->add_child(node);
                material->set_global_transform(Transform(material->get_global_transform().get_basis(), me->get_global_transform().get_origin()));
            }
        }

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
    ray = Object::cast_to<RayCast>(get_node("KinematicBody/RayCast"));
    player = Object::cast_to<Player>(get_node("/root/Spatial/Player"));
    gameController = Object::cast_to<GameController>(get_node("/root/GameController"));
}

void Zombie::_set_move_speed(float speed) {
    moveSpeed = speed;
    terminal_velocity = Vector3(moveSpeed,moveSpeed,moveSpeed);
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

    if (abs(current_location.distance_to(Vector3(target.x,current_location.y,target.z))) < 1) {
        return;
    }

    Vector3 move_direction = (target-current_location).normalized();

    force += move_direction * Vector3(moveSpeed,0,moveSpeed);
}

void Zombie::_process(float delta) {

    if (player == nullptr) {
        goto tower_location;
    }

    if (player != nullptr) {
        Vector3 player_location = player->me->get_global_transform().get_origin();
        Vector3 location = me->get_global_transform().get_origin();
        if (location.distance_to(player_location) < 25) {
            _set_target(player_location);
            goto location_set;
        }
    }
    
    tower_location:
    if (gameController != nullptr && gameController->playerTower != nullptr) {
        target = gameController->playerTower->get_global_transform().get_origin();
    } else {
        Godot::print("No location for zombie to go");
    }
    location_set:

    if (ray == nullptr) {
        Godot::print("Ray not found");
        return;
    }

    ray->set_collide_with_areas(false);
    ray->set_collide_with_bodies(true);

    ray->set_enabled(true);

    Vector3 current_location = me->get_global_transform().get_origin();
    ray->set_cast_to((target-current_location).normalized() * 3);
    ray->force_raycast_update();

    if (ray->is_colliding()) {
        Object* obj = ray->get_collider();
        StaticBody* body = Object::cast_to<StaticBody>(obj);
        KinematicBody* kin = Object::cast_to<KinematicBody>(obj);
        if (body != nullptr) {
            Structure* structure = Node::cast_to<Structure>(body->get_parent()->get_parent());
            if (structure != nullptr) {
                structure->_take_damage(5);
            } else {
                Tower* tower = Node::cast_to<Tower>(body->get_parent()->get_parent());
                if (tower != nullptr) {
                    tower->_take_damage(.05);
                }
            }
            
        } else if (kin != nullptr) {
            Player* local_player = Node::cast_to<Player>(kin->get_parent());
            if (local_player != nullptr) {
                local_player->_take_damage(0.1);
            }
        }
    }

    ray->set_enabled(false);
}

void Zombie::_set_target(Vector3 target) {
    this->target = target;
}

Vector3 Zombie::_to_grid_coordinate(Vector3 location) {
    Vector3 ret = Vector3(0,0,0);
    ret.x = ((int) location.x / GRID_SIZE) * GRID_SIZE + (location.x/abs(location.x) * GRID_SIZE/2);
    ret.y = ((int) location.y / GRID_SIZE) * GRID_SIZE + (location.y/abs(location.y) * GRID_SIZE/2);
    ret.z = ((int) location.z / GRID_SIZE) * GRID_SIZE + (location.z/abs(location.z) * GRID_SIZE/2);
    return ret;
}