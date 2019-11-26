#ifndef GRIDBLOCk_H
#define GRIDBLOCk_H

#include <Godot.hpp>
#include <Area.hpp>
#include <Structure.h>

namespace godot {

	class GridBlock : public Spatial {
		GODOT_CLASS(GridBlock, Spatial)

	private:

        Structure* floor = nullptr;
        Structure* ceiling = nullptr;
        Structure* front_wall = nullptr;
        Structure* back_wall = nullptr;
        Structure* left_wall = nullptr;
        Structure* right_wall = nullptr;

	public:
        Structure* ramp = nullptr;


        enum class Direction {
            Front, Back, Left, Right
        };

        static void _register_methods();

        GridBlock();
        ~GridBlock();

        void _init(); // our initializer called by Godot

        void _process(float delta);
        void _ready();

        bool _has_floor();
        bool _has_ceiling();
        bool _has_ramp();
        bool _has_wall(Direction type);

        bool _add_wall(Direction type);
        bool _add_ramp(Direction direction);
        bool _add_floor();
        bool _add_ceiling();

        void _clear_structure_pointers();
        void _clear_structure_pointer_if_exists(Structure* pointer);

	private:
        bool _has_left_wall();
        bool _has_right_wall();
        bool _has_back_wall();
        bool _has_front_wall();
	};

}

#endif