#ifndef TOWER_H
#define TOWER_H

#include <Godot.hpp>
#include <Area.hpp>

namespace godot {

	class Tower : public Spatial {
		GODOT_CLASS(Tower, Spatial)

	private:
    
	public:
    
    static void _register_methods();

        enum class Type {
            Player, Enemy
        };

        Type type = Type::Enemy;

        float health = 100;
        

        Tower();
        ~Tower();

        void _init(); // our initializer called by Godot

        void _process(float delta);
        void _take_damage(float damage);
        void _ready();

	private:
        
	};

}

#endif