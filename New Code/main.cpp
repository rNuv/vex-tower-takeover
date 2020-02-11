#include "main.h"

const int TOP_LEFT_DRIVE = 2;
const int TOP_RIGHT_DRIVE = -4;
const int BOTTOM_RIGHT_DRIVE = -3;
const int BOTTOM_LEFT_DRIVE = 1;
const int TWO_BAR = 14;
const int RIGHT_ROLLER = -12;
const int LEFT_ROLLER = 20;
const int PUSHER = 5;
const int IMU_PORT = 16;
//DONT USE PORT 11

const int NUM_HEIGHTS = 3;
int goalHeight = 0;
const int height1 = 0;
const int height2 = -1700;
const int height3 = -2100;
const int push1 = 0;
const int push2 = 1650;
const int push3 = 1650;
const int heights[NUM_HEIGHTS] = {height1, height2, height3};
const int pushes[NUM_HEIGHTS] = {push1, push2, push3};

const int ROLLER_SPEED = 100;
const int SLOW_ROLLER_SPEED = 25;
const int PUSHER_SPEED = 22;
const int PUSHER_BACK_SPEED = 35;

bool liftIsUp = false;

Controller controller;

ControllerButton slowBackButton(ControllerDigital::right);
ControllerButton slowForwardButton(ControllerDigital::left);
ControllerButton upLift(ControllerDigital::X);
ControllerButton downLift(ControllerDigital::B);
ControllerButton runAutoButton(ControllerDigital::A);
ControllerButton liftUpButton(ControllerDigital::L1);
ControllerButton liftDownButton(ControllerDigital::L2);
ControllerButton rollOutButton(ControllerDigital::R1);
ControllerButton rollInButton(ControllerDigital::R2);
ControllerButton pushForwardButton(ControllerDigital::up);
ControllerButton pushBackwardButton(ControllerDigital::down);

Motor twoBar(TWO_BAR);
Motor rightRoller(RIGHT_ROLLER);
Motor leftRoller(LEFT_ROLLER);
Motor pusher(PUSHER);
Motor topLeft(TOP_LEFT_DRIVE);
Motor topRight(TOP_RIGHT_DRIVE);
Motor backLeft(BOTTOM_LEFT_DRIVE);
Motor backRight(BOTTOM_RIGHT_DRIVE);

std::map<std::string, Motor> ALL_MOTORS
{{"top_left", topLeft}, {"top_right", topRight},
{"down_left", backLeft}, {"down_right", backRight},
{"pusher",pusher}, {"two_bar",twoBar},
{"rollerL",leftRoller}, {"rollerR",rightRoller}};


auto drive = ChassisControllerBuilder()
		.withMotors({TOP_LEFT_DRIVE, BOTTOM_LEFT_DRIVE}, {TOP_RIGHT_DRIVE, BOTTOM_RIGHT_DRIVE})
		// .withGains(
    //     {0.002, 0.00, 0.001}, // Distance controller gains
    //     {0.002, 0.00, 0.001}, // Turn controller gains
    //     {0.002, 0.00, 0.001}  // Angle controller gains (helps drive straight)
    // )
		.withDimensions(AbstractMotor::gearset::green, {{4_in, 14_in}, 900})
		.build();

pros::Imu imu_sensor(IMU_PORT);

auto rollerController = AsyncVelControllerBuilder()
												.withMotor({RIGHT_ROLLER, LEFT_ROLLER})
												.build();

auto pushController = AsyncPosControllerBuilder()
											.withMotor(PUSHER)
											.build();

auto liftController = AsyncPosControllerBuilder()
											.withMotor(TWO_BAR)
											.build();

auto slowDrive = AsyncMotionProfileControllerBuilder()
								.withLimits({0.1, 0.3, 5.0})
								.withOutput(drive)
								.buildMotionProfileController();

auto medDrive = AsyncMotionProfileControllerBuilder()
													.withLimits({0.3, 0.3, 5.0})
											    .withOutput(drive)
											    .buildMotionProfileController();

auto fastDrive = AsyncMotionProfileControllerBuilder()
													.withLimits({0.6, 0.4, 5.0})
													.withOutput(drive)
													.buildMotionProfileController();



void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello R_N#V11");

	medDrive->generatePath({
	  {0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
	  {3.05_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
	  "OutsideForward" // Profile name
	);

	fastDrive->generatePath({
		{0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
		{2.6_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
		"OutsideBack" // Profile name
	);

	medDrive->generatePath({
		{0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
		{1.67_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
		"InsideForward" // Profile name
	);

	medDrive->generatePath({
		{0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
		{2.0_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
		"InsideRight" // Profile name
	);

	medDrive->generatePath({
		{0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
		{0.9_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
		"InsideBack" // Profile name
	);

}

void pusherControl() {
		pusher.setGearing(AbstractMotor::gearset::red);

    if (pushBackwardButton.isPressed() && pusher.getTargetVelocity() != 40) {
        pusher.moveVelocity(-100);
        if (pusher.getPosition() < 0) {
            pusher.tarePosition();
        }
    } else if (pushForwardButton.isPressed()) {
        if (pusher.getPosition() < 1450) {
            pusher.moveVelocity(100);
        }
				else if(pusher.getPosition() > 3800){
						pusher.moveVelocity(0);
				}else {
						int x = pusher.getPosition();
            int vel = (int)(((0.00003121) * x * x) - (0.1935 * x) + 315.0);
            if (vel < 20) {
                vel = 20;
            }
            pusher.moveVelocity(vel);
        }
    }
    if (pushForwardButton.changedToReleased() ||
		pushBackwardButton.changedToReleased()) {
        pusher.moveVelocity(0);
				pusher.setBrakeMode(AbstractMotor::brakeMode::hold);
    }
}

void rollerControl(){

	leftRoller.setGearing(AbstractMotor::gearset::red);
	rightRoller.setGearing(AbstractMotor::gearset::red);

	if(!liftIsUp){
		if(rollOutButton.isPressed()){
			leftRoller.moveVelocity(-ROLLER_SPEED);
			rightRoller.moveVelocity(-ROLLER_SPEED);
		}
		else if(rollInButton.isPressed()){
			leftRoller.moveVelocity(ROLLER_SPEED);
			rightRoller.moveVelocity(ROLLER_SPEED);
		}
		else{
			leftRoller.moveVelocity(0);
			rightRoller.moveVelocity(0);
			leftRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
			rightRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
		}
	}

	if(liftIsUp){
		if(rollOutButton.isPressed()){
			leftRoller.moveVelocity(-ROLLER_SPEED);
			rightRoller.moveVelocity(-ROLLER_SPEED);
		}
		else if(rollInButton.isPressed()){
			leftRoller.moveVelocity(-SLOW_ROLLER_SPEED);
			rightRoller.moveVelocity(-SLOW_ROLLER_SPEED);
		}
		else{
			leftRoller.moveVelocity(0);
			rightRoller.moveVelocity(0);
			leftRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
			rightRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
		}
	}
}

void gyroTurn(int angle){

	while(imu_sensor.get_heading() <= fabs(angle)){
		topRight.moveVelocity(-50);
		topLeft.moveVelocity(50);
		backLeft.moveVelocity(50);
		backRight.moveVelocity(-50);
	}
	topRight.moveVelocity(0);
	topLeft.moveVelocity(0);
	backLeft.moveVelocity(0);
	backRight.moveVelocity(0);
}

void twoBarControl(){
	if(liftUpButton.changedToPressed() && goalHeight < NUM_HEIGHTS - 1){
		goalHeight++;
		liftController->setTarget(heights[goalHeight]);
		pushController->setTarget(pushes[goalHeight]);
		liftIsUp = true;
	}
	else if (liftDownButton.changedToPressed() && goalHeight > 0) {
		goalHeight--;
		if(goalHeight == 0){
			pushController->setTarget(1800);
			liftController->setTarget(heights[goalHeight]);
			pushController->waitUntilSettled();
			liftIsUp = false;
		}
		liftController->setTarget(heights[goalHeight]);
		pushController->setTarget(pushes[goalHeight]);
	}

	if(upLift.isPressed()){
		twoBar.moveVelocity(-100);
	}
	else if(downLift.isPressed()){
		twoBar.moveVelocity(100);
	}
	else if(upLift.changedToReleased() || downLift.changedToReleased()){
		twoBar.moveVelocity(0);
		twoBar.setBrakeMode(AbstractMotor::brakeMode::hold);
	}
}

void goSlow(){
	if(slowForwardButton.isPressed()){
		topRight.moveVelocity(30);
		topLeft.moveVelocity(30);
		backLeft.moveVelocity(30);
		backRight.moveVelocity(30);
	}
	else if(slowBackButton.isPressed()){
		topRight.moveVelocity(-30);
		topLeft.moveVelocity(-30);
		backLeft.moveVelocity(-30);
		backRight.moveVelocity(-30);
	}
	else if(slowForwardButton.changedToReleased() || slowBackButton.changedToReleased()){
		topRight.moveVelocity(0);
		topLeft.moveVelocity(0);
		backLeft.moveVelocity(0);
		backRight.moveVelocity(0);
	}
}

void disabled() {}

void pidTurn(double input) {
		double angle = input * 5.02;

		imu_sensor.reset();

		double TARGET = imu_sensor.get_heading() + angle;
		double HALFWAY = imu_sensor.get_heading() + angle / 4;
		double currentValue = imu_sensor.get_heading();
		double currentError = TARGET - currentValue;
		double previousError = 0;
		double accel = true;

		double kP = 1.000;
		double kI = 0.000;
		double kD = 5.000;
		double kDr = 0.000;

		double maxRate = 16;

		while (fabs(currentError) > 10) {
			if (angle > 0 && currentValue > HALFWAY) {
				accel = false;
			} else if (angle < 0 && currentValue < HALFWAY) {
				accel = false;
			}

			double p = kP * currentError;
			double i = kI;
			double d = kD * (currentError - previousError);

			double command = p + i + d;

			if (fabs(command) > maxRate) {
				if (command > 0) {
					command = maxRate;
				} else {
					command = -maxRate;
				}
			}

			double left = command;
			double right = -(command);

			topLeft.moveVelocity(left);
			topRight.moveVelocity(right);
			backLeft.moveVelocity(left);
			backRight.moveVelocity(right);

			pros::delay(20);

			if (accel) {
				if (maxRate < 150) {
					maxRate += 10;
				}
			}

			currentValue = imu_sensor.get_heading();
			previousError = currentError;
			currentError = TARGET - currentValue;
		}

		topLeft.moveVelocity(0);
		topRight.moveVelocity(0);
		backLeft.moveVelocity(0);
		backRight.moveVelocity(0);

}

void deploy(){
	rightRoller.moveVelocity(-600);
	leftRoller.moveVelocity(-600);
	pros::delay(2000);
	rightRoller.moveVelocity(0);
	leftRoller.moveVelocity(0);
	pros::delay(200);
}

void moveBack(int speed, int time){
	topRight.moveVelocity(-speed);
	topLeft.moveVelocity(-speed);
	backLeft.moveVelocity(-speed);
	backRight.moveVelocity(-speed);
	pros::delay(time);
	topRight.moveVelocity(0);
	topLeft.moveVelocity(0);
	backLeft.moveVelocity(0);
	backRight.moveVelocity(0);
}

void competition_initialize() {}

void updateTempWarning(){
  std::string motorName = "";
	int i = 0;
  for(auto motor: ALL_MOTORS) {
    int temp = motor.second.getTemperature();
		motorName = motor.first;
		pros::lcd::set_text(i, motorName + std::to_string(temp));
		i++;
  }
}

void stackAndBack(){
	rightRoller.moveVelocity(-20);
	leftRoller.moveVelocity(-20);
	pros::delay(1000);
	rightRoller.moveVelocity(0);
	leftRoller.moveVelocity(0);
	pushController->setTarget(3700);
	pushController->waitUntilSettled();
	pushController->setTarget(0);
	pushController->waitUntilSettled();
	rightRoller.moveVelocity(-100);
	leftRoller.moveVelocity(-100);
	moveBack(100, 1500);
}

void blueOut(){
	deploy();
	rightRoller.moveVelocity(600);
	leftRoller.moveVelocity(600);
	medDrive->setTarget("OutsideForward");
	medDrive->waitUntilSettled();
	rightRoller.moveVelocity(0);
	leftRoller.moveVelocity(0);
	drive->turnAngle(-177_deg);
	rightRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
	leftRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
	pros::delay(200);
	fastDrive->setTarget("OutsideBack");
	fastDrive->waitUntilSettled();

	stackAndBack();
}

void blueIn3pt(){
}

void blueIn2pt(){
	deploy();
	medDrive->setTarget("InsideForward"); // go forward to cube
	rollerController->setTarget(170); // start rolling inwards
	medDrive->waitUntilSettled(); // wait until stopped
	drive->turnAngle(90_deg); // turn to right
	medDrive->setTarget("InsideRight"); // go to third cube
	medDrive->waitUntilSettled();
	drive->turnAngle(45_deg); // turn to goal zone
	medDrive->setTarget("InsideBack"); // go to goal zone
	rollerController->setTarget(0); // stop rollers
	medDrive->waitUntilSettled();
	// rollerController->setTarget(-70);
	// pushController->setTarget(1500);
	// pushController->waitUntilSettled();
	// moveBack(40, 1500);
	rightRoller.moveVelocity(-600);
	leftRoller.moveVelocity(-600);
	pros::delay(400);
	moveBack(50, 500);
}

void redOut(){
}

void redIn2pt(){
	deploy();
	medDrive->setTarget("InsideForward"); // go forward to cube
	rollerController->setTarget(170); // start rolling inwards
	medDrive->waitUntilSettled(); // wait until stopped
	drive->turnAngle(-90_deg); // turn to right
	medDrive->setTarget("InsideRight"); // go to third cube
	medDrive->waitUntilSettled();
	drive->turnAngle(-47_deg); // turn to goal zone
	medDrive->setTarget("InsideBack"); // go to goal zone
	rollerController->setTarget(0); // stop rollers
	medDrive->waitUntilSettled();
	// rollerController->setTarget(-70);
	// pushController->setTarget(1500);
	// pushController->waitUntilSettled();
	// moveBack(40, 1500);
	rightRoller.moveVelocity(-600);
	leftRoller.moveVelocity(-600);
	pros::delay(400);
	moveBack(50, 500);
}

void redIn3pt(){
}

void onePt(){
	 deploy();
	 rollerController->setTarget(170); // start rolling inwards
	 medDrive->setTarget("InsideForward"); // go forward to cube
	 medDrive->waitUntilSettled();
	 rollerController->setTarget(-200);
	 pros::delay(2000);
	 moveBack(50, 1000);
}

void progSkills(){
	deploy();
	rightRoller.moveVelocity(600);
	leftRoller.moveVelocity(600);
	medDrive->setTarget("OutsideForward");
	medDrive->waitUntilSettled();
	rightRoller.moveVelocity(0);
	leftRoller.moveVelocity(0);
	drive->turnAngle(-177_deg);
	rightRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
	leftRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
	pros::delay(200);
	fastDrive->setTarget("OutsideBack");
	fastDrive->waitUntilSettled();

	stackAndBack();
}

void autonomous() {
	redIn2pt();
}

void opcontrol() {

	while (true){

		drive->getModel()->arcade(0.9 * controller.getAnalog(ControllerAnalog::leftY),
		0.75 * controller.getAnalog(ControllerAnalog::rightX));

		if(runAutoButton.changedToPressed()){
			autonomous();
		}

		pusherControl();
		rollerControl();
		twoBarControl();
		updateTempWarning();
		goSlow();

		pros::delay(20);
	}
}
