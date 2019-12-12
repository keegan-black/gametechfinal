#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <Godot.hpp>
#include <Area.hpp>
#include <KinematicBody.hpp>
#include <RayCast.hpp>
#include "Player.h"
#include "GameController.h"

namespace godot {

	class Zombie : public Spatial {
		GODOT_CLASS(Zombie, Spatial)

	private:
        KinematicBody* me;
		RayCast* ray;
		Player* player;
		GameController* gameController;

		Vector3 gravity;
		Vector3 terminal_velocity = Vector3(10,10,10);
        Vector3 velocity;

        float moveSpeed;

		float health = 100;
		float attack_timer = 0;

        Vector3 target;

	public:

        static void _register_methods();

		Zombie();
		~Zombie();

		void _init(); // our initializer called by Godot

		void _process(float delta);
		void _physics_process(float delta);
        void _take_damage(float damage);
		void _ready();
		void _set_move_speed(float speed);

        void _set_target(Vector3 target);
        void _move();

	private:

        void handle_gravity(Vector3& force, Vector3& gravity);
		void handle_movement(Vector3& force);
		Vector3 _to_grid_coordinate(Vector3 location);

	};

}

#endif