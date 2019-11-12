#ifndef PLAYER_H
#define PLAYER_H

#include <Godot.hpp>
#include <Area.hpp>
#include <RandomNumberGenerator.hpp>
#include <CollisionShape.hpp>
#include <MeshInstance.hpp>
#include <core/Array.hpp>
#include <Input.hpp>
#include <InputEvent.hpp>
#include <KinematicBody.hpp>
#include <Camera.hpp>

namespace godot {

	class Player : public Area {
		GODOT_CLASS(Player, Area)

	private:
		
		Input* input;
		
		Vector3 velocity;

		float camera_angle = 0;
		float mouse_x_sensitivity = 0.3;
		float mouse_y_sensitivity = 0.3;
		float rotation = 0;

		float max_walk_angle = 0.75;


	public:

        enum class FrontDirection {
            None, Forward, Backward
        };

        enum class SideDirection {
            None, Left, Right
        };

        enum class MovementAction {
            None, Jump
        };

		KinematicBody* me;
		Camera* camera;

		float moveSpeed;
		float jumpForce;

		Vector3 gravity;
		Vector3 terminal_velocity = Vector3(10,10,10);

		static void _register_methods();

		Player();
		~Player();

		void _init(); // our initializer called by Godot

		void _process(float delta);
		void _physics_process(float delta);
		void _input(InputEvent *event);
        void _move(FrontDirection front_direction, SideDirection side_direction, MovementAction action);
		void _ready();

	private:

		void handle_gravity(Vector3& force, Vector3& gravity);
		void handle_movement(Vector3& force, FrontDirection front_direction, SideDirection side_direction);
		void handle_move_action(Vector3& force, MovementAction move_action);
	};

}

#endif
