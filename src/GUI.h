#ifndef GUI_H
#define GUI_H

#include <Godot.hpp>
#include <Area.hpp>
#include <Control.hpp>
#include <Label.hpp>
#include <ProgressBar.hpp>

namespace godot {

	class GUI : public Control {
		GODOT_CLASS(GUI, Control)

	public:

        Label* round_label;
        Label* ammo_label;
        Label* materials_label;
        ProgressBar* player_health_bar;
        ProgressBar* player_tower_health_bar;
        ProgressBar* zombie_tower_health_bar;
    
    static void _register_methods();
        

        GUI();
        ~GUI();

        void _init(); // our initializer called by Godot

        void _set_materials_label(int materials);
        void _set_ammo_label(int materials);
        void _set_zombie_tower_health_bar(int health);
        void _set_player_tower_health_bar(int health);
        void _set_player_health_bar(int health);
        void _set_round_label(int round);

        void _process(float delta);
        void _ready();

	private:
        
	};

}

#endif