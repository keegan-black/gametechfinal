#ifndef MENU_H
#define MENU_H

#include <Godot.hpp>
#include <Area.hpp>
#include <Control.hpp>

namespace godot {

	class Menu : public Control {
		GODOT_CLASS(Menu, Control)

	private:

	public:
    
    static void _register_methods();

        Menu();
        ~Menu();

        void _init(); // our initializer called by Godot

        void _start_game();
        void _ready();

	private:
        
	};

}

#endif