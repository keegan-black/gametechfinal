#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <Godot.hpp>
#include <Area.hpp>
#include "Tower.h"
#include "Structure.h"
#include "GridBlock.h"

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

        int level = 0;
        

        GameController();
        ~GameController();

        void _init(); // our initializer called by Godot

        void _process(float delta);
        void _ready();

        bool _add_structure(Structure::Type type, GridBlock::Direction direction, Vector3 location);

        private:
        bool _add_structure_in_gridblock(Structure::Type type, GridBlock::Direction direction, GridBlock* gridBlock);
        void _add_neighboring_grid_blocks_if_dont_exist(Vector3 location);
        GridBlock* _build_grid_block_at(Vector3 location);
        Vector3 _to_grid_coordinate(Vector3 location);
        };

}

#endif