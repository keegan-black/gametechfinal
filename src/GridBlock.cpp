#include "GridBlock.h"
#include <KinematicCollision.hpp>
#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <InputEventMouseMotion.hpp>
#include <StaticBody.hpp>

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
    ceiling = nullptr;
    left_wall = nullptr;
    right_wall = nullptr;
    back_wall = nullptr;
    front_wall = nullptr;
    floor = nullptr;
}

void GridBlock::_ready(){
}

bool GridBlock::_add_wall(Direction type) {
    if (_has_wall(type)) { return false;}

    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    Ref<PackedScene> WallScene = resourceLoader->load("res://Wall.tscn");
    godot::Structure* wall = Node::cast_to<Structure>(WallScene->instance());

    this->add_child(wall);
    float rot = 0;
    switch (type)
    {
    case Direction::Front:
        rot = 0;
        front_wall = wall;
        break;
    case Direction::Back:
        rot = 180 * PI/180;
        back_wall = wall;
        break;
    case Direction::Left:
        rot = 90 * PI/180;
        left_wall = wall;
        break;
    case Direction::Right:
        rot = 270 * PI/180;
        right_wall = wall;
        break;
    
    default:
        break;
    }
    wall->rotate(Vector3(0,1,0),rot);
    wall->set_translation(Vector3(0,0,0));
    Godot::print(wall->get_global_transform());
    return true;

}
bool GridBlock::_add_ramp(Direction direction) {
    if (_has_ramp()) { 
        Godot::print("Ramp Already Exists");
        return false; }

    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    Ref<PackedScene> rampScene = resourceLoader->load("res://Ramp.tscn");
    Structure* ramp = Node::cast_to<Structure>(rampScene->instance());

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

    Godot::print(ramp->get_global_transform());
    return true;

}
bool GridBlock::_add_floor() {
    if (_has_floor()) { return false; }

    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    Ref<PackedScene> floorScene = resourceLoader->load("res://Floor.tscn");
    Structure* floor = Node::cast_to<Structure>(floorScene->instance());

    this->floor = floor;
    this->add_child(floor);
    floor->set_translation(Vector3(0,0,0));
    return true;
}
bool GridBlock::_add_ceiling() {
    if (_has_ceiling()) { return false; }

    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    Ref<PackedScene> ceilingScene = resourceLoader->load("res://Ceiling.tscn");
    Structure* ceiling = Node::cast_to<Structure>(ceilingScene->instance());

    this->ceiling = ceiling;
    this->add_child(ceiling);
    ceiling->set_translation(Vector3(0,0,0));
    return true;

}

bool GridBlock::_has_floor() {
    return floor != nullptr;
}
bool GridBlock::_has_ceiling() {
    return ceiling != nullptr;
}
bool GridBlock::_has_ramp() {
    return ramp != nullptr;
}
bool GridBlock::_has_wall(Direction type) {
    switch (type)
    {
    case Direction::Front:
        if (_has_front_wall()) { return false; }
        break;
    case Direction::Back:
        if (_has_back_wall()) { return false; }
        break;
    case Direction::Left:
        if (_has_left_wall()) { return false; }
        break;
    case Direction::Right:
        if (_has_right_wall()) { return false; }
        break;
    
    default:
        return false;
        break;
    }
    return false;
}
bool GridBlock::_has_left_wall() {
    return left_wall != nullptr;
}
bool GridBlock::_has_right_wall() {
    return right_wall != nullptr;
}
bool GridBlock::_has_back_wall() {
    return back_wall != nullptr;
}
bool GridBlock::_has_front_wall() {
    return front_wall != nullptr;
}

void GridBlock::_clear_structure_pointer_if_exists(Structure* pointer) {
    if (ramp == pointer) {
        ramp = nullptr;
    } else if (ceiling == pointer) {
        ceiling = nullptr;
    } else if (left_wall == pointer) {
        left_wall = nullptr;
    } else if (right_wall == pointer) {
        right_wall = nullptr;
    } else if (back_wall == pointer) {
        back_wall = nullptr;
    } else if (front_wall == pointer) {
        front_wall = nullptr;
    } else if (floor == pointer) {
        floor = nullptr;
    }
}

void GridBlock::_clear_structure_pointers() {
    ramp = nullptr;
    ceiling = nullptr;
    left_wall = nullptr;
    right_wall = nullptr;
    back_wall = nullptr;
    front_wall = nullptr;
    floor = nullptr;
}

void GridBlock::_process(float delta) {
}