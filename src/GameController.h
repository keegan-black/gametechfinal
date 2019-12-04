#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <Godot.hpp>
#include <Area.hpp>
#include "Tower.h"

namespace godot {

	class GameController : public Node {
		GODOT_CLASS(GameController, Node)

	private:
    

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

	private:
        
	};

}

#endif