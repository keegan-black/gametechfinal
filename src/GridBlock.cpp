#include "GridBlock.h"
#include <KinematicCollision.hpp>
#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <InputEventMouseMotion.hpp>
#include <StaticBody.hpp>
#include <RayCast.hpp>

using namespace godot;
#define PI 3.14

void GridBlock::_register_methods() {
	register_method("_process", &GridBlock::_process);
	register_method("_ready", &GridBlock::_ready);
}

GridBlock::GridBlock() {
}

GridBlock::~GridBlock() {
	// add your cleanup here
}


void GridBlock::_init() {
	ramp = nullptr;
}

void GridBlock::_ready(){
    ramp = nullptr;
}

void GridBlock::_check_neighbors() {

    Godot::print("Checking Neighbors");

    Structure* n_front_wall = _has_front_wall();
    Structure* n_back_wall = _has_back_wall();
    Structure* n_left_wall = _has_left_wall();
    Structure* n_right_wall = _has_right_wall();
    Structure* n_ceiling = _has_ceiling();
    Structure* n_floor = _has_floor();

    if (n_front_wall != nullptr) {
        Godot::print("Found front wall");
    }
    if (n_back_wall != nullptr) {
        Godot::print("Found back wall");
    }
    if (n_left_wall != nullptr) {
        Godot::print("Found left wall");
    }
    if (n_right_wall != nullptr) {
        Godot::print("Found right wall");
    }
    if (n_ceiling != nullptr) {
        Godot::print("Found ceiling");
    }
    if (n_floor != nullptr) {
        Godot::print("Found floor");
    }
}

bool GridBlock::_add_wall(Direction type) {
    if (_has_wall(type)) { 
        Godot::print("GridBlock: Wall Already Exists");
        return false;}

    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    Ref<PackedScene> WallScene = resourceLoader->load("res://Wall.tscn");
    godot::Structure* wall = Node::cast_to<Structure>(WallScene->instance());
    wall->_set_type(Structure::Type::Wall);
    this->add_child(wall);
    float rot = 0;
    switch (type)
    {
    case Direction::Front:
        rot = 0;
        break;
    case Direction::Back:
        rot = 180 * PI/180;
        break;
    case Direction::Left:
        rot = 90 * PI/180;
        break;
    case Direction::Right:
        rot = 270 * PI/180;
        break;
    
    default:
        break;
    }
    wall->rotate(Vector3(0,1,0),rot);
    wall->set_translation(Vector3(0,0,0));
    return true;

}
bool GridBlock::_add_ramp(Direction direction) {
    if (_has_ramp()) { 
        Godot::print("Ramp Already Exists");
        return false; }

    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    Ref<PackedScene> rampScene = resourceLoader->load("res://Ramp.tscn");
    Structure* ramp = Node::cast_to<Structure>(rampScene->instance());
    ramp->_set_type(Structure::Type::Ramp);
    this->ramp = ramp;

    this->add_child(ramp);
    float rot = 0;
    switch (direction)
    {
    case Direction::Front:
        rot = 0;
        break;
    case Direction::Back:
        rot = 180 * PI/180;
        break;
    case Direction::Left:
        rot = 90 * PI/180;
        break;
    case Direction::Right:
        rot = 270 * PI/180;
        break;
    
    default:
        break;
    }

    ramp->rotate(Vector3(0,1,0),rot);
    ramp->set_translation(Vector3(0,0,0));

    return true;

}
bool GridBlock::_add_floor() {
    if (_has_floor() != nullptr) { 
        Godot::print("GridBlock: Floor Already Exists");
        return false; }

    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    Ref<PackedScene> floorScene = resourceLoader->load("res://Floor.tscn");
    Structure* floor = Node::cast_to<Structure>(floorScene->instance());
    floor->_set_type(Structure::Type::Floor);
    this->add_child(floor);
    floor->set_translation(Vector3(0,0,0));
    return true;
}
bool GridBlock::_add_ceiling() {
    if (_has_ceiling() != nullptr) { 
        Godot::print("GridBlock: Ceiling Already Exists");
        return false; }

    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    Ref<PackedScene> ceilingScene = resourceLoader->load("res://Ceiling.tscn");
    Structure* ceiling = Node::cast_to<Structure>(ceilingScene->instance());
    ceiling->_set_type(Structure::Type::Floor);
    this->add_child(ceiling);
    ceiling->set_translation(Vector3(0,0,0));
    return true;

}

Structure* GridBlock::_has_floor() {

    RayCast* ray = Object::cast_to<RayCast>(get_node("RayCast"));
    if (ray == nullptr) {
        Godot::print("Couldn't find ray node");
        return nullptr;
    }

    Godot::print("Checking Floor");

    ray->set_collide_with_areas(false);
    ray->set_collide_with_bodies(true);

    Structure* structure = nullptr;

    ray->set_enabled(true);

    ray->set_cast_to(Vector3(0,-3,0));
    ray->force_raycast_update();
    if (ray->is_colliding()) {
        Object* obj = ray->get_collider();
        StaticBody* body = Object::cast_to<StaticBody>(obj);
        if (body != nullptr) {
            Node* parent = body->get_parent();
            if (parent != nullptr) {
                structure = Node::cast_to<Structure>(parent->get_parent());
                if (structure != nullptr && structure->_get_type() != Structure::Type::Floor) {
                    structure = nullptr;
                }
            }
        }
    }

    ray->set_enabled(false);

    return structure;

}
Structure* GridBlock::_has_ceiling() {
    RayCast* ray = Object::cast_to<RayCast>(get_node("RayCast"));
    if (ray == nullptr) {
        Godot::print("Couldn't find ray node");
        return nullptr;
    }

    Godot::print("Checking Ceiling");

    ray->set_collide_with_areas(false);
    ray->set_collide_with_bodies(true);

    Structure* structure = nullptr;

    ray->set_enabled(true);

    ray->set_cast_to(Vector3(0,3,0));
    ray->force_raycast_update();
    if (ray->is_colliding()) {
        Object* obj = ray->get_collider();
        StaticBody* body = Object::cast_to<StaticBody>(obj);
        if (body != nullptr) {
            Node* parent = body->get_parent();
            if (parent != nullptr) {
                structure = Node::cast_to<Structure>(parent->get_parent());
                if (structure != nullptr && structure->_get_type() != Structure::Type::Floor) {
                    structure = nullptr;
                }
            }
        }
    }

    ray->set_enabled(false);

    return structure;
}
bool GridBlock::_has_ramp() {
    return ramp != nullptr;
}
bool GridBlock::_has_wall(Direction type) {
    switch (type)
    {
    case Direction::Front:
        if (_has_front_wall() != nullptr) { return true; }
        break;
    case Direction::Back:
        if (_has_back_wall() != nullptr) { return true; }
        break;
    case Direction::Left:
        if (_has_left_wall() != nullptr) { return true; }
        break;
    case Direction::Right:
        if (_has_right_wall() != nullptr) { return true; }
        break;
    
    default:
        return false;
        break;
    }
    return false;
    
}
Structure* GridBlock::_has_left_wall() {
    RayCast* ray = Object::cast_to<RayCast>(get_node("RayCast"));
    if (ray == nullptr) {
        Godot::print("Couldn't find ray node");
        return nullptr;
    }

    Godot::print("Checking Left Wall");

    ray->set_collide_with_areas(false);
    ray->set_collide_with_bodies(true);

    Structure* structure = nullptr;

    ray->set_enabled(true);

    ray->set_cast_to(Vector3(-3,0,0));
    ray->force_raycast_update();
    if (ray->is_colliding()) {
        Object* obj = ray->get_collider();
        StaticBody* body = Object::cast_to<StaticBody>(obj);
        if (body != nullptr) {
            Node* parent = body->get_parent();
            if (parent != nullptr) {
                structure = Node::cast_to<Structure>(parent->get_parent());
                if (structure != nullptr && structure->_get_type() != Structure::Type::Wall) {
                    structure = nullptr;
                }
            }
        }
    }

    ray->set_enabled(false);

    return structure;
}
Structure* GridBlock::_has_right_wall() {
    RayCast* ray = Object::cast_to<RayCast>(get_node("RayCast"));
    if (ray == nullptr) {
        Godot::print("Couldn't find ray node");
        return nullptr;
    }

    Godot::print("Checking Right Wall");

    ray->set_collide_with_areas(false);
    ray->set_collide_with_bodies(true);

    Structure* structure = nullptr;

    ray->set_enabled(true);

    ray->set_cast_to(Vector3(3,0,0));
    ray->force_raycast_update();
    if (ray->is_colliding()) {
        Object* obj = ray->get_collider();
        StaticBody* body = Object::cast_to<StaticBody>(obj);
        if (body != nullptr) {
            Node* parent = body->get_parent();
            if (parent != nullptr) {
                structure = Node::cast_to<Structure>(parent->get_parent());
                if (structure != nullptr && structure->_get_type() != Structure::Type::Wall) {
                    structure = nullptr;
                }
            }
        }
    }

    ray->set_enabled(false);

    return structure;
}
Structure* GridBlock::_has_back_wall() {
    RayCast* ray = Object::cast_to<RayCast>(get_node("RayCast"));
    if (ray == nullptr) {
        Godot::print("Couldn't find ray node");
        return nullptr;
    }

    Godot::print("Checking Back Wall");

    ray->set_collide_with_areas(false);
    ray->set_collide_with_bodies(true);

    Structure* structure = nullptr;

    ray->set_enabled(true);

    ray->set_cast_to(Vector3(0,0,3));
    ray->force_raycast_update();
    if (ray->is_colliding()) {
        Object* obj = ray->get_collider();
        StaticBody* body = Object::cast_to<StaticBody>(obj);
        if (body != nullptr) {
            Node* parent = body->get_parent();
            if (parent != nullptr) {
                structure = Node::cast_to<Structure>(parent->get_parent());
                if (structure != nullptr && structure->_get_type() != Structure::Type::Wall) {
                    structure = nullptr;
                }
            }
        }
    }

    ray->set_enabled(false);

    return structure;
}
Structure* GridBlock::_has_front_wall() {
    RayCast* ray = Object::cast_to<RayCast>(get_node("RayCast"));
    if (ray == nullptr) {
        Godot::print("Couldn't find ray node");
        return nullptr;
    }

    Godot::print("Checking Front Wall");

    ray->set_collide_with_areas(false);
    ray->set_collide_with_bodies(true);

    Structure* structure = nullptr;

    ray->set_enabled(true);

    ray->set_cast_to(Vector3(0,0,-3));
    ray->force_raycast_update();
    if (ray->is_colliding()) {
        Object* obj = ray->get_collider();
        StaticBody* body = Object::cast_to<StaticBody>(obj);
        if (body != nullptr) {
            Node* parent = body->get_parent();
            if (parent != nullptr) {
                structure = Node::cast_to<Structure>(parent->get_parent());
                if (structure != nullptr && structure->_get_type() != Structure::Type::Wall) {
                    structure = nullptr;
                }
            }
        }
    }

    ray->set_enabled(false);

    return structure;
}

GridBlock::Direction GridBlock::_face_at_global_point(Vector3 location) {
    Vector3 curr_location = this->get_global_transform().get_origin();
    Vector3 relative_location = location - curr_location;
    // Godot::print("Current Location");
    // Godot::print(curr_location);
    // Godot::print("Relative Location");
    // Godot::print(relative_location);

    int x,y,z;
    x = (int) relative_location.x;
    y = (int) relative_location.y;
    z = (int) relative_location.z;

    std::cout << x <<  " " << y << " " << z << std::endl;

    if (z >= 3 && abs(x) < 3 && abs(y) < 3) {
        Godot::print("Direction::Back");
        return Direction::Back;
    }
    if (z <= 3 && abs(x) < 3 && abs(y) < 3) {
        Godot::print("Direction::Front");
        return Direction::Front;
    }
    if (x >= 3 && abs(z) < 3 && abs(y) < 3) {
        Godot::print("Direction::Right");
        return Direction::Right;
    }
    if (x <= 3 && abs(z) < 3 && abs(y) < 3) {
        Godot::print("Direction::Left");
        return Direction::Left;
    }
    if (y >= 3 && abs(z) < 3 && abs(x) < 3) {
        Godot::print("Direction::Top");
        return Direction::Top;
    }
    if (y <= 3 && abs(z) < 3 && abs(x) < 3) {
        Godot::print("Direction::Bottom");
        return Direction::Bottom;
    }


    return Direction::Front;

}

void GridBlock::_process(float delta) {
}