#include "GameController.h"
#include <SceneTree.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>
#include <Viewport.hpp>
#include <Zombie.h>

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
	
}

void GameController::_process(float delta) {
	if (timer > 0) {
		timer -= delta;
	}

	if (zombies_to_spawn > 0 && timer <= 0) {
		timer = 2;
		zombies_to_spawn -= 1;
		_spawn_zombie();
	}

}

void GameController::_spawn_zombie() {

	if (zombieTower == nullptr || playerTower == nullptr) {
		Godot::print("Zombie Spawn Tower Nil");
		return;
	}

    ResourceLoader* resourceLoader = ResourceLoader::get_singleton();
    Ref<PackedScene> zombieScene = resourceLoader->load("res://Zombie.tscn");
    Zombie* zombie = Node::cast_to<Zombie>(zombieScene->instance());
    get_node("/root/Spatial")->add_child(zombie);
	zombie->set_global_transform(Transform(zombie->get_global_transform().get_basis(),zombieTower->get_global_transform().get_origin() + Vector3(-10,5,0)));
	zombie->_set_target(playerTower->get_global_transform().get_origin());

	//Move speed normally 5
	int randSpeed = rand()%(8-5 + 1) + 5;
	zombie->_set_move_speed(randSpeed);

}

void GameController::_tower_death_signal(Tower* tower) {
	if (tower == playerTower) {
		_game_over();
	} else if (tower == zombieTower) {
		_round_complete();
	}
}

void GameController::_tower_health_signal(Tower* tower) {
	Godot::print("tower health signal");
	if (tower == playerTower && gui != nullptr) {
		gui->_set_player_tower_health_bar(tower->health);
	} else if (tower == zombieTower && gui != nullptr) {
		gui->_set_zombie_tower_health_bar(tower->health);
	}
}

void GameController::_game_start() {
	playerTower = Node::cast_to<Tower>(get_node("/root/Spatial/PlayerTower"));
	zombieTower = Node::cast_to<Tower>(get_node("/root/Spatial/ZombieTower"));
	gui = Node::cast_to<GUI>(get_node("/root/Spatial/GUI"));

	timer = 2;
	zombies_to_spawn = 5;
}

void GameController::_round_complete() {
	level +=1;
	if (gui != nullptr) {
		gui->_set_round_label(level);
	}
	if (zombieTower != nullptr) {
		zombieTower->health = 100;
	}
	if (playerTower != nullptr) {
		playerTower->health = 100;
	}

	timer = 10;
	zombies_to_spawn = 5 + level * level;
}

void GameController::_game_over() {
	level = 0;
	playerTower = nullptr;
	zombieTower = nullptr;
	timer = 0;
	zombies_to_spawn = 0;
	gui = nullptr;
	get_tree()->change_scene("res://Menu.tscn");
}

bool GameController::_add_structure(Structure::Type type, GridBlock::Direction direction, Vector3 location,bool is_ground) {

	GridBlock* gridBlock = Object::cast_to<GridBlock>(structures[location]);
	if (gridBlock != nullptr) {
		return _add_structure_in_gridblock(type, direction, gridBlock);
	} else {
		//Build GridBlock
		GridBlock* new_gridBlock = _build_grid_block_at(location,is_ground);
		return _add_structure_in_gridblock(type, direction, new_gridBlock);
	}

	Godot::print("No GridBlock");

	return false;
}

void GameController::_check_gridblock_and_delete(Vector3 location) {
	Vector3 grid_location = _to_grid_coordinate(location);
	GridBlock* gridBlock = Object::cast_to<GridBlock>(structures[grid_location]);
	if (gridBlock != nullptr) {
		if (gridBlock->is_empty()) {

			structures.erase(grid_location);
			gridBlock->get_parent()->remove_child(gridBlock);
			//gridBlock->queue_free();
			
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(GRID_SIZE,0,0));
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(-GRID_SIZE,0,0));
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(GRID_SIZE,GRID_SIZE,0));
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(-GRID_SIZE,GRID_SIZE,0));
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(GRID_SIZE,-GRID_SIZE,0));
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(-GRID_SIZE,-GRID_SIZE,0));

			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(0,GRID_SIZE,0));
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(0,-GRID_SIZE,0));

			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(0,0,GRID_SIZE));
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(0,0,-GRID_SIZE));
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(0,GRID_SIZE,GRID_SIZE));
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(0,GRID_SIZE,-GRID_SIZE));
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(0,-GRID_SIZE,GRID_SIZE));
			_is_gridBlock_connected_to_floor_at(grid_location + Vector3(0,-GRID_SIZE,-GRID_SIZE));
		}
	}
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

GridBlock* GameController::_build_grid_block_at(Vector3 location, bool is_ground) {
	GridBlock* check_block = Object::cast_to<GridBlock>(structures[location]);
	if (check_block == nullptr) {
		Vector3 new_location = _to_grid_coordinate(location);
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
		new_gridBlock->is_on_ground = is_ground;
		structures[new_location] = new_gridBlock;

		return new_gridBlock;
	}
	return check_block;
}

void GameController::_add_neighboring_grid_blocks_if_dont_exist(Vector3 location) {
	Vector3 grid_location = _to_grid_coordinate(location);
	GridBlock* block = Object::cast_to<GridBlock>(structures[grid_location]);
	if (block != nullptr && !block->is_empty()) {
		Vector3 left = grid_location + Vector3(-GRID_SIZE,0,0);
		Vector3 right = grid_location + Vector3(GRID_SIZE,0,0);
		Vector3 front = grid_location + Vector3(0,0,-GRID_SIZE);
		Vector3 back = grid_location + Vector3(0,0,GRID_SIZE);

		_build_grid_block_at(left, block->is_on_ground);
		_build_grid_block_at(right, block->is_on_ground);
		_build_grid_block_at(front, block->is_on_ground);
		_build_grid_block_at(back, block->is_on_ground);
	}
}

bool GameController::_is_gridBlock_connected_to_floor_at(Vector3 location) {
	Dictionary visited;
	bool on_ground = _find_ground_dfs(location,visited);
	if (!on_ground) {
		Array arr = Array(visited.keys());
		for (int i = 0; i < arr.size(); i++) {
			Vector3 location = arr[i];
			Vector3 front_location = location + Vector3(0,0,-GRID_SIZE);
			Vector3 back_location = location + Vector3(0,0,GRID_SIZE);
			Vector3 right_location = location + Vector3(-GRID_SIZE,0,0);
			Vector3 left_location = location + Vector3(GRID_SIZE,0,0);

			GridBlock * gridblock = Object::cast_to<GridBlock>(structures[arr[i]]);
			GridBlock* front = Object::cast_to<GridBlock>(structures[front_location]);
			GridBlock* back = Object::cast_to<GridBlock>(structures[back_location]);
			GridBlock* right = Object::cast_to<GridBlock>(structures[right_location]);
			GridBlock* left = Object::cast_to<GridBlock>(structures[left_location]);

			bool found_neighbor = false;
			found_neighbor = found_neighbor || (front != nullptr && !visited.has(front_location) && !front->is_empty());
			found_neighbor = found_neighbor || (back != nullptr && !visited.has(back_location) && !back->is_empty());
			found_neighbor = found_neighbor || (right != nullptr && !visited.has(right_location) && !right->is_empty());
			found_neighbor = found_neighbor || (left != nullptr && !visited.has(left_location) && !left->is_empty());
			
			if (found_neighbor && gridblock != nullptr) {
				if (gridblock->is_empty()) {
					remove_gridBlock_at(arr[i]);
					_build_grid_block_at(location,gridblock->is_on_ground);
				}
			} else {
				remove_gridBlock_at(arr[i]);
			}
    	}

		// Array all_grid_blocks = Array(structures.keys());
		// for (int i = 0; i < all_grid_blocks.size(); i++) {
		// 	_add_neighboring_grid_blocks_if_dont_exist(all_grid_blocks[i]);
		// }
	}
	return on_ground;
}

bool GameController::_find_ground_dfs(Vector3 location, Dictionary& visited) { 
	GridBlock* block = Object::cast_to<GridBlock>(structures[location]);
	//bool* has_been_visited = Object::cast_to<bool>(visited[location]);
	bool has_been_visited = static_cast<bool>(visited[location]);
	if (block == nullptr || has_been_visited) {
		return false;
	}

	if(block->is_empty()) {
		return false;
	}

	if (block->is_on_ground) {
		return true;
	}

	visited[location] = true;

	bool found = false;

	found = found || _find_ground_dfs(location + Vector3(GRID_SIZE,0,0), visited);
	found = found || _find_ground_dfs(location + Vector3(-GRID_SIZE,0,0), visited);
	found = found || _find_ground_dfs(location + Vector3(GRID_SIZE,GRID_SIZE,0), visited);
	found = found || _find_ground_dfs(location + Vector3(-GRID_SIZE,GRID_SIZE,0), visited);
	found = found || _find_ground_dfs(location + Vector3(GRID_SIZE,-GRID_SIZE,0), visited);
	found = found || _find_ground_dfs(location + Vector3(-GRID_SIZE,-GRID_SIZE,0), visited);

	found = found || _find_ground_dfs(location + Vector3(0,GRID_SIZE,0), visited);
	found = found || _find_ground_dfs(location + Vector3(0,-GRID_SIZE,0), visited);

	found = found || _find_ground_dfs(location + Vector3(0,0,GRID_SIZE), visited);
	found = found || _find_ground_dfs(location + Vector3(0,0,-GRID_SIZE), visited);
	found = found || _find_ground_dfs(location + Vector3(0,GRID_SIZE,GRID_SIZE), visited);
	found = found || _find_ground_dfs(location + Vector3(0,GRID_SIZE,-GRID_SIZE), visited);
	found = found || _find_ground_dfs(location + Vector3(0,-GRID_SIZE,GRID_SIZE), visited);
	found = found || _find_ground_dfs(location + Vector3(0,-GRID_SIZE,-GRID_SIZE), visited);

	return found;
}

void GameController::remove_gridBlock_at(Vector3 location) {
	GridBlock* block = Object::cast_to<GridBlock>(structures[location]);
	structures.erase(location);
	if (block != nullptr) {
		block->queue_free();
	}
	
}


Vector3 GameController::_to_grid_coordinate(Vector3 location) {
    Vector3 ret = Vector3(0,0,0);
    ret.x = ((int) location.x / GRID_SIZE) * GRID_SIZE + (location.x/abs(location.x) * GRID_SIZE/2);
    ret.y = ((int) location.y / GRID_SIZE) * GRID_SIZE + (location.y/abs(location.y) * GRID_SIZE/2);
    ret.z = ((int) location.z / GRID_SIZE) * GRID_SIZE + (location.z/abs(location.z) * GRID_SIZE/2);
    return ret;
}