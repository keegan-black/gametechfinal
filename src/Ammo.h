#ifndef AMMO_H
#define AMMO_H

#include <Godot.hpp>
#include <Area.hpp>

namespace godot {

	class Ammo : public Area {
		GODOT_CLASS(Ammo, Area)

	private:
    
	public:
    
    static void _register_methods();

        Ammo();
        ~Ammo();

        void _init(); // our initializer called by Godot

        void _process(float delta);
        void _body_entered(Node *other);
        void _ready();

	private:
        
	};

}

#endif