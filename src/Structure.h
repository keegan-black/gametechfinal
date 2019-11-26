#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <Godot.hpp>
#include <Area.hpp>

namespace godot {

	class Structure : public Spatial {
		GODOT_CLASS(Structure, Spatial)

	private:
		
		float health = 100;

	public:

        static void _register_methods();

		Structure();
		~Structure();

		void _init(); // our initializer called by Godot

		void _process(float delta);
        void _take_damage(float damage);
		void _ready();

	private:
	};

}

#endif