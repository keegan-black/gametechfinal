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

		enum class Type {
			Wall, Ramp, Floor
		};

		enum class Orientation {
			Front, Back, Left, Right
		};

        static void _register_methods();

		Structure();
		~Structure();

		void _init(); // our initializer called by Godot

		void _process(float delta);
        void _take_damage(float damage);
		void _ready();
		void _set_type(Type type);
		void _set_orientation(Orientation orientation);
		Type _get_type();
		Orientation _get_orientation();

	private:
		Type type = Type::Wall;
		Orientation orientation = Orientation::Front;

	};

}

#endif