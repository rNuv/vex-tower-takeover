#include "main.h"

int TOP_LEFT_DRIVE = 1;
int TOP_RIGHT_DRIVE = -2;
int BOTTOM_RIGHT_DRIVE = -3;
int BOTTOM_LEFT_DRIVE = 4;
int TWO_BAR = 5;
int RIGHT_ROLLER = 6;
int LEFT_ROLLER = 7;
int PUSHER = 8;

Controller masterController;

ControllerButton runAutoButton(ControllerDigital::X);
ControllerButton leftStrafe(ControllerDigital::left);
ControllerButton rightStrafe(ControllerDigital::right);
ControllerButton liftUpButton(ControllerDigital::L1);
ControllerButton liftDownButton(ControllerDigital::L2);
ControllerButton rollUpButton(ControllerDigital::R1);
ControllerButton rollDownButton(ControllerDigital::R2);
ControllerButton pushForwardButton(ControllerDigital::up);
ControllerButton pushBackwardButton(ControllerDigital::down);

Motor twoBar(TWO_BAR);
Motor rightRoller(RIGHT_ROLLER);
Motor leftRoller(LEFT_ROLLER);
Motor pusher(PUSHER);
/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed");
	} else {
		pros::lcd::clear_line(2);
	}
}

int getStrafeSpeed(ControllerButton left, ControllerButton right)
{
	if(left.isPressed())
	{
		return -100;
	}
	else if (right.isPressed())
	{
		return 100;
	}
	else
	{
		return 0;
	}
}

void lift(ControllerButton up, ControllerButton down)
{
  if(up.isPressed())
  {
    twoBar.move_velocity(100);
  }
  else if(down.isPressed())
  {
    twoBar.move_velocity(-100);
  }
  else{
    twoBar.move_velocity(0);
  }
}

void roll(ControllerButton up, ControllerButton down)
{
  if(up.isPressed())
  {
    leftRoller.move_velocity(100);
    rightRoller.move_velocity(-100);
  }
  else if(down.isPressed())
  {
    leftRoller.move_velocity(-100);
    rightRoller.move_velocity(100);
  }
  else
  {
    leftRoller.move_velocity(0);
    rightRoller.move_velocity(0);
  }
}

void push(ControllerButton forward, ControllerButton backward)
{
  if(forward.isPressed())
  {
    pusher.move_velocity(20);
  }
  else if(backward.isPressed())
  {
    pusher.move_velocity(-20);
  }
  else
  {
    pusher.move_velocity(0);
  }
}
/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello arnavv!");

	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

auto myChassis = ChassisControllerFactory::create(
	 1, -2, -3, 4,
	 AbstractMotor::gearset::green,
	 {4_in, 12.5_in}
);

void autonomous() {
	myChassis.moveDistance(24_in);
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */


auto xChassis = ChassisModelFactory::create(TOP_LEFT_DRIVE, TOP_RIGHT_DRIVE,
	BOTTOM_RIGHT_DRIVE, BOTTOM_LEFT_DRIVE, 100.0);

int strafeSpeed = 0;

void opcontrol() {
	while (true)
	{
		strafeSpeed = getStrafeSpeed(leftStrafe, rightStrafe);

		xChassis.xArcade(strafeSpeed,
		masterController.getAnalog(ControllerAnalog::leftY),
		masterController.getAnalog(ControllerAnalog::rightX));

		lift(liftUpButton, liftDownButton);
		roll(rollUpButton, rollDownButton);
		push(pushForwardButton, pushBackwardButton);

		if(runAutoButton.changedToPressed())
		{
			autonomous();
		}


		pros::delay(20);
	}
}
