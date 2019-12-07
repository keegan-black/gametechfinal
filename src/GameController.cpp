#include "GameController.h"
#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <Viewport.hpp>

using namespace godot;
#define PI 3.14
#define GRID_SIZE 6

void GameController::_register_methods() {
	register_method("_process", &GameController::_process);
	register_method("_ready", &GameController::_ready);
}

GameController::GameController() {
}

GameController::~GameController() {
	// add your cleanup here
}

void GameController::_init() {
}

void GameController::_ready(){
	playerTower = Node::cast_to<Tower>(get_node("/root/Spatial/PlayerTower"));
	zombieTower = Node::cast_to<Tower>(get_node("/root/Spatial/ZombieTower"));
}

void GameController::_process(float delta) {
}

bool GameController::_add_structure(Structure::Type type, GridBlock::Direction direction, Vector3 location) {

	GridBlock* gridBlock = Object::cast_to<GridBlock>(structures[location]);
	if (gridBlock != nullptr) {
		return _add_structure_in_gridblock(type, direction, gridBlock);
	} else {
		//Build GridBlock
		GridBlock* new_gridBlock = _build_grid_block_at(location);
		return _add_structure_in_gridblock(type, direction, new_gridBlock);
	}

	Godot::print("No GridBlock");

	return false;
}

bool GameController::_add_structure_in_gridblock(Structure::Type type, GridBlock::Direction direction, GridBlock* gridBlock) {
	bool built = false;
	switch (type)
	{
	case Structure::Type::Ramp:
		if(gridBlock->_add_ramp(direction)) {
			built = true;
		}
		break;
	case Structure::Type::Wall:
		if(gridBlock->_add_wall(direction)) {
			built = true;
		}
		break;
	case Structure::Type::Floor:
		if (gridBlock->_add_floor()) {
			built = true;
		}
		break;
	default:
		break;
	}

	if (built) {
		_add_neighboring_grid_blocks_if_dont_exist(gridBlock->get_global_transform().get_origin());
	}
	return built;
}

GridBlock* GameController::_build_grid_block_at(Vector3 location) {
	GridBlock* check_block = Object::cast_to<GridBlock>(structures[location]);
	if (check_block == nullptr) {
		Vector3 new_location = _to_grid_coordinate(location);
		Godot::print("Build New Grid Block at");
		Godot::print(new_location);
		ResourceLoader* resourceLoader = ResourceLoader::get_singleton();

		if (resourceLoader == nullptr) {
			Godot::print("[ERROR] Resource Loader Null");
			return check_block;
		}

		Ref<PackedScene> gridScene = resourceLoader->load("res://GridBlock.tscn");
		GridBlock* new_gridBlock = Node::cast_to<GridBlock>(gridScene->instance());

		if (new_gridBlock == nullptr) {
			Godot::print("[ERROR] Gridblock is null");
			return check_block;
		}

		get_tree()->get_root()->add_child(new_gridBlock);
		new_gridBlock->set_global_transform(Transform(new_gridBlock->get_global_transform().basis,new_location));
		structures[new_location] = new_gridBlock;

		return new_gridBlock;
	}
	return check_block;
}

void GameController::_add_neighboring_grid_blocks_if_dont_exist(Vector3 location) {
	Vector3 grid_location = _to_grid_coordinate(location);

	Vector3 left = grid_location + Vector3(-GRID_SIZE,0,0);
	Vector3 right = grid_location + Vector3(GRID_SIZE,0,0);
	Vector3 front = grid_location + Vector3(0,0,-GRID_SIZE);
	Vector3 back = grid_location + Vector3(0,0,GRID_SIZE);

	_build_grid_block_at(left);
	_build_grid_block_at(right);
	_build_grid_block_at(front);
	_build_grid_block_at(back);
}


Vector3 GameController::_to_grid_coordinate(Vector3 location) {
    Vector3 ret = Vector3(0,0,0);
    ret.x = ((int) location.x / GRID_SIZE) * GRID_SIZE + (location.x/abs(location.x) * GRID_SIZE/2);
    ret.y = ((int) location.y / GRID_SIZE) * GRID_SIZE + (location.y/abs(location.y) * GRID_SIZE/2);
    ret.z = ((int) location.z / GRID_SIZE) * GRID_SIZE + (location.z/abs(location.z) * GRID_SIZE/2);
    return ret;
}