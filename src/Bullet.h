#ifndef BULLET_H
#define BULLET_H

#include <Godot.hpp>
#include <Area.hpp>

namespace godot {

	class Bullet : public Spatial {
		GODOT_CLASS(Bullet, Spatial)

	private:
    
	public:
    
    static void _register_methods();

        float lifetime = 0.1;
        Vector3 direction;
        int speed = 400;

        Bullet();
        ~Bullet();

        void _init(); // our initializer called by Godot
        void _set_direction_towards(Vector3 location);
        void _process(float delta);
        void _ready();

	private:
        
	};

}

#endif