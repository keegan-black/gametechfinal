#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <Godot.hpp>
#include <Area.hpp>
#include "Tower.h"
#include "Structure.h"
#include "GridBlock.h"
#include "GUI.h"

namespace godot {

	class GameController : public Node {
		GODOT_CLASS(GameController, Node)

	private:

        using Graph = Dictionary;
        Graph structures;
        // Vector3 -> Gridblock

	public:
        static void _register_methods();

        Tower* playerTower = nullptr;
        Tower* zombieTower = nullptr;
        GUI* gui = nullptr;

        int level = 1;
        float timer = 0;
        int zombies_to_spawn = 0;
        

        GameController();
        ~GameController();

        void _init(); // our initializer called by Godot

        void _process(float delta);
        void _ready();

        void _game_start();
        void _round_complete();
        void _game_over();

        bool _add_structure(Structure::Type type, GridBlock::Direction direction, Vector3 location, bool is_ground = false);
        void _check_gridblock_and_delete(Vector3 location);
        void _spawn_zombie();

        void _tower_death_signal(Tower* tower);
        void _tower_health_signal(Tower* tower);

        private:
        bool _add_structure_in_gridblock(Structure::Type type, GridBlock::Direction direction, GridBlock* gridBlock);
        void _add_neighboring_grid_blocks_if_dont_exist(Vector3 location);

        bool _is_gridBlock_connected_to_floor_at(Vector3 location);

        bool _find_ground_dfs(Vector3 location, Dictionary& visited);


        GridBlock* _build_grid_block_at(Vector3 location, bool is_ground);
        Vector3 _to_grid_coordinate(Vector3 location);
        void remove_gridBlock_at(Vector3 location);
        };

}

#endif