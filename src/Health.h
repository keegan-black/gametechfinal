#ifndef HEALTH_H
#define HEALTH_H

#include <Godot.hpp>
#include <Area.hpp>

namespace godot {

	class Health : public Area {
		GODOT_CLASS(Health, Area)

	private:
    
	public:
    
    static void _register_methods();

        Health();
        ~Health();

        void _init(); // our initializer called by Godot

        void _process(float delta);
        void _body_entered(Node *other);
        void _ready();

	private:
        
	};

}

#endif