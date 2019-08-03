#include "main.h"
#include "variables.h"
using namespace okapi;

int FRONT_LEFT = 4;
int FRONT_RIGHT = 2;
int BACK_LEFT = 1;
int BACK_RIGHT = 3;
int TWO_BAR = 5;

pros::Motor FrontRight (FRONT_RIGHT);
pros::Motor FrontLeft (FRONT_LEFT);
pros::Motor BackLeft (BACK_LEFT);
pros::Motor BackRight (BACK_RIGHT);
pros::Motor TwoBar (TWO_BAR);

int RightX;
int RightY;
int LeftY;
int LeftX;
bool liftUpButton;
bool liftDownButton;
int thresh = 20;

void opcontrol() {

	pros::Controller controller (CONTROLLER_MASTER);

	while(true){

    RightX = controller.get_analog(ANALOG_RIGHT_X);
		RightY = controller.get_analog(ANALOG_RIGHT_Y);
		LeftY = controller.get_analog(ANALOG_LEFT_Y);
		LeftX = controller.get_analog(ANALOG_LEFT_X);

		bool rollUpButton = controller.get_digital(DIGITAL_R1);
		bool rollDownButton = controller.get_digital(DIGITAL_R2);
    bool liftUpButton = controller.get_digital(DIGITAL_L1);
    bool liftDownButton = controller.get_digital(DIGITAL_L2);

		if((abs(RightX) > 0) || (abs(RightY) > 0) || (abs(LeftY) > 0) || (abs(LeftX) > 0)){
			if(abs(RightX) < thresh) {
				RightX = 0;
			}
			if(abs(RightY) < thresh) {
				RightY = 0;
			}
			if(abs(LeftY) < thresh) {
				LeftY = 0;
			}
			if(abs(LeftX) < thresh) {
				LeftX = 0;
			}

			if(abs(LeftY) > abs(RightY)) {
				RightY = 0;
			}
			else {
				LeftY = 0;
			}

			if(abs(LeftY) > 80){
				RightX *= .707106781186548;
			}
		}

		drive(RightX, RightY, LeftY, LeftX);
    lift(liftUpButton, liftDownButton);
		roll(rollUpButton, rollDownButton);

		pros::Task::delay(10);
	}
}
