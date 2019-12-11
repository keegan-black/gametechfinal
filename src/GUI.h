#ifndef GUI_H
#define GUI_H

#include <Godot.hpp>
#include <Area.hpp>
#include <Control.hpp>
#include <Label.hpp>

namespace godot {

	class GUI : public Control {
		GODOT_CLASS(GUI, Control)

	public:

        Label* ammo_label;
        Label* materials_label;
    
    static void _register_methods();
        

        GUI();
        ~GUI();

        void _init(); // our initializer called by Godot

        void _set_materials_label(int materials);
        void _set_ammo_label(int materials);

        void _process(float delta);
        void _ready();

	private:
        
	};

}

#endif