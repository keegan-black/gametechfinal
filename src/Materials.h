#ifndef MATERIALS_H
#define MATERIALS_H

#include <Godot.hpp>
#include <Area.hpp>

namespace godot {

	class Materials : public Area {
		GODOT_CLASS(Materials, Area)

	private:
    
	public:
    
    static void _register_methods();
    
        Materials();
        ~Materials();

        void _init(); // our initializer called by Godot

        void _process(float delta);
        void _body_entered(Node *other);
        void _ready();

	private:
        
	};

}

#endif