#include "main.h"

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

using namespace okapi;

int FRONT_LEFT = 1;
int FRONT_RIGHT = -2;
int BACK_LEFT = -3;
int BACK_RIGHT = 4;

auto myChassis = ChassisControllerFactory::create(
  FRONT_LEFT, FRONT_RIGHT, BACK_LEFT, BACK_RIGHT,
  AbstractMotor::gearset::green,
  {4_in, 11_in}
);

void opcontrol() {

	Controller controller;
	ControllerButton runAutoButton(ControllerDigital::X);

	while(true){
		myChassis.arcade(controller.getAnalog(ControllerAnalog::leftY), controller.getAnalog(ControllerAnalog::rightX));

		if(runAutoButton.changedToPressed()){
			for(int i = 0; i < 4; i++){
				myChassis.moveDistance(12_in);
				myChassis.turnAngle(90_deg);
			}
		}

		pros::Task::delay(10);
	}

}
