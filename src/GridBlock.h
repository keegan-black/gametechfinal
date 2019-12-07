#ifndef GRIDBLOCk_H
#define GRIDBLOCk_H

#include <Godot.hpp>
#include <Area.hpp>
#include <Structure.h>

namespace godot {

	class GridBlock : public Spatial {
		GODOT_CLASS(GridBlock, Spatial)

	private:

	public:
        Structure* ramp = nullptr;


        enum class Direction {
            Front, Back, Left, Right, Top, Bottom, None
        };

        static void _register_methods();

        GridBlock();
        ~GridBlock();

        void _init(); // our initializer called by Godot

        void _process(float delta);
        void _ready();

        bool _has_ramp();

        Structure* _has_floor();
        Structure* _has_ceiling();
        Structure* _has_left_wall();
        Structure* _has_right_wall();
        Structure* _has_back_wall();
        Structure* _has_front_wall();

        bool _has_wall(Direction type);

        bool _add_wall(Direction type);
        bool _add_ramp(Direction direction);
        bool _add_floor();
        bool _add_ceiling();
        void _check_neighbors();

        bool _has_neighbor_block(Direction direction);

        Direction _face_at_global_point(Vector3 location);

	private:
        
	};

}

#endif