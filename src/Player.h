#ifndef PLAYER_H
#define PLAYER_H

#include <Godot.hpp>
#include "GridBlock.h"
#include <Area.hpp>
#include <RandomNumberGenerator.hpp>
#include <CollisionShape.hpp>
#include <MeshInstance.hpp>
#include <core/Array.hpp>
#include <Input.hpp>
#include <InputEvent.hpp>
#include <KinematicBody.hpp>
#include <Camera.hpp>
#include <RayCast.hpp>

namespace godot {

	class Player : public Spatial {
		GODOT_CLASS(Player, Spatial)

	private:
		
		Input* input;
		
		Vector3 velocity;

		float camera_angle = 0; //Degrees
		float mouse_x_sensitivity = 0.3;
		float mouse_y_sensitivity = 0.3;
		float rotation = 0; //Radians

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

		enum class Action {
			None, Shoot, Build, Melee
		};

		enum class BuildType {
			None, Wall, Ramp, Floor
		};

		enum class Facing {
			None,
			Front, 	// -z
			Back,	// z
			Left,	// -x
			Right,	// x
			Up,	// y
			Down	// -y
		};

		KinematicBody* me;
		Camera* camera;
		RayCast* ray;
		Spatial* placeholder;

		float moveSpeed;
		float jumpForce;

		Vector3 gravity;
		Vector3 terminal_velocity = Vector3(10,10,10);

		bool is_shooting = false;
		bool is_hitting = false;
		bool is_building = false;

		int ammo = 20;
		int materials = 100;
		float health = 100;

		static void _register_methods();

		Player();
		~Player();

		void _init(); // our initializer called by Godot

		void _process(float delta);
		void _physics_process(float delta);
		void _input(InputEvent *event);
        void _move(FrontDirection front_direction, SideDirection side_direction, MovementAction action);
		void _perform_action(Action action, BuildType buildType);
		void _take_damage(float damage);
		void _ready();

	private:

		float _get_grid_rotation();
		float _get_grid_tilt();

		void _update_placeholder();

		Vector3 _get_nearest_neighbor_gridBlock_from(Vector3 location);
		Facing _get_facing_with_tilt();
		Facing _get_facing_no_tilt();

		void _shoot();
		void _melee();

		bool _build(BuildType buildType);

		void _update_GUI();
		
		void handle_gravity(Vector3& force, Vector3& gravity);
		void handle_movement(Vector3& force, FrontDirection front_direction, SideDirection side_direction);
		void handle_move_action(Vector3& force, MovementAction move_action);

		Vector3 _to_grid_coordinate(Vector3 location);
	};

}

#endif
