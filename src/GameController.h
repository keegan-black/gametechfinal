#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <Godot.hpp>
#include <Area.hpp>

namespace godot {

	class GameController : public Node {
		GODOT_CLASS(GameController, Node)

	private:
    
    static void _register_methods();

	public:

        Vector3 playerTower;
        Vector3 zombieTower;

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