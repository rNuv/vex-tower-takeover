#include "main.h"

const int TOP_LEFT_DRIVE = 2;
const int TOP_RIGHT_DRIVE = -4;
const int BOTTOM_RIGHT_DRIVE = -3;
const int BOTTOM_LEFT_DRIVE = 1;

const int TWO_BAR = 14;
const int RIGHT_ROLLER = -11;
const int LEFT_ROLLER = 20;
const int PUSHER = 5;

const int NUM_HEIGHTS = 3;
const int height1 = 0;
const int height2 = -1700;
const int height3 = -2100;

const int push1 = 0;
const int push2 = 1600;
const int push3 = 1600;

// const int heights[NUM_HEIGHTS] = {height1, height2, height3};
// const int pushes[NUM_HEIGHTS] = {push1, push2, push3};

const int ROLLER_SPEED = 100;
const int SLOW_ROLLER_SPEED = 50;
const int PUSHER_SPEED = 22;
const int PUSHER_BACK_SPEED = 35;

Controller controller;

ControllerButton runDeploymentButton(ControllerDigital::B);
ControllerButton downHeight(ControllerDigital::Y);
ControllerButton upHeight(ControllerDigital::X);
ControllerButton slowRollButton(ControllerDigital::A);
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

auto drive = ChassisControllerBuilder()
		.withMotors({TOP_LEFT_DRIVE, BOTTOM_LEFT_DRIVE}, {TOP_RIGHT_DRIVE, BOTTOM_RIGHT_DRIVE})
		// .withGains(
    //     {0.001, 0, 0.0000}, // Distance controller gains
    //     {0.001, 0, 0.0001}, // Turn controller gains
    //     {0.001, 0, 0.0001}  // Angle controller gains (helps drive straight)
    // )
		.withDimensions(AbstractMotor::gearset::green, {{4_in, 14_in}, 900})
		.build();

auto rollerController = AsyncVelControllerBuilder()
												.withMotor({RIGHT_ROLLER, LEFT_ROLLER})
												.build();

auto pushController = AsyncPosControllerBuilder()
											.withMotor(PUSHER)
											.build();

auto liftController = AsyncPosControllerBuilder()
											.withMotor(TWO_BAR)
											.build();

auto profileController = AsyncMotionProfileControllerBuilder()
													.withLimits({0.3, 0.5, 5.0}) // tune numbers
											    .withOutput(drive)
											    .buildMotionProfileController();

// void liftPushRoll(ControllerButton out, ControllerButton in, ControllerButton slowRoll,
// 	ControllerButton forward, ControllerButton backward,
// 	ControllerButton liftUp, ControllerButton liftDown, ControllerButton up, ControllerButton down,
// 	int goal){
	// leftRoller.setGearing(AbstractMotor::gearset::red);
	// rightRoller.setGearing(AbstractMotor::gearset::red);
	// pusher.setGearing(AbstractMotor::gearset::red);
	//
	// if(out.isPressed())
	// {
	// 	leftRoller.moveVelocity(ROLLER_SPEED);
	// 	rightRoller.moveVelocity(ROLLER_SPEED);
	// }
	// else if(in.isPressed())
	// {
	// 	leftRoller.moveVelocity(-ROLLER_SPEED);
	// 	rightRoller.moveVelocity(-ROLLER_SPEED);
	// }
	// else if(slowRoll.isPressed())
	// {
	// 	leftRoller.moveVelocity(-SLOW_ROLLER_SPEED);
	// 	rightRoller.moveVelocity(-SLOW_ROLLER_SPEED);
	// }
	// else if(forward.isPressed())
  // {
  //   pusher.moveVelocity(PUSHER_SPEED);
  // }
  // else if(backward.isPressed())
  // {
  //   pusher.moveVelocity(-PUSHER_BACK_SPEED);
  // }
	// else if(liftUp.isPressed())
	// {
	// 	pushController->setTarget(1350);
	// 	pros::delay(200);
	// 	twoBar.moveVelocity(-100);
	// }
	// else if(liftDown.isPressed())
	// {
	// 	liftController->setTarget(0);
	// }
  // else
  // {
  //  pusher.moveVelocity(0);
	// 	twoBar.moveVelocity(0);
	// 	leftRoller.moveVelocity(0);
	// 	rightRoller.moveVelocity(0);
	// 	rightRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
	// 	leftRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
	// 	twoBar.setBrakeMode(AbstractMotor::brakeMode::hold);
	// 	pusher.setBrakeMode(AbstractMotor::brakeMode::hold);
  // }
// }

void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello R_N#V11");

	profileController->generatePath({
	  {0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
	  {2.05_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
	  "OutsideForward" // Profile name
	);

	profileController->generatePath({
		{0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
		{2.25_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
		"OutsideBack" // Profile name
	);

	profileController->generatePath({
		{0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
		{1.67_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
		"InsideForward" // Profile name
	);

	profileController->generatePath({
		{0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
		{2.0_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
		"InsideRight" // Profile name
	);

	profileController->generatePath({
		{0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
		{0.9_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
		"InsideBack" // Profile name
	);

	profileController->generatePath({
		{0_ft, 0_ft, 0_deg},  // Profile starting position, this will normally be (0, 0, 0)
		{0.4_ft, 0_ft, 0_deg}}, // The next point in the profile, 3 feet forward
		"InsideForwardLittle" // Profile name
	);

}

void tilterControl() {
    if (pushBackwardButton.isPressed() && pusher.getTargetVelocity() != 40) {
        pusher.moveVelocity(-100);
        if (pusher.getPosition() < 0) {
            pusher.tarePosition();
        }
    } else if (pushForwardButton.isPressed()) {
        if (pusher.getPosition() < 500) {
            pusher.moveVelocity(100);
        } else {
            int vel = 50 + (450 + (-1 * pusher.getPosition() + 250)) * 0.1111;
            if (vel < 10) {
                vel = 10;
            }
            pusher.moveVelocity(vel);
        }
    }
    if (pushForwardButton.changedToReleased() || pushBackwardButton.changedToReleased()) {
        pusher.moveVelocity(0);
    }
}


void disabled() {}

void deploy(){
	rightRoller.moveVelocity(-600);
	leftRoller.moveVelocity(-600);
	pros::delay(2500);
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

void blueOut(){
	 deploy();
	 rightRoller.moveVelocity(600);
	 leftRoller.moveVelocity(600);
	 profileController->setTarget("OutsideForward");
	 profileController->waitUntilSettled();
	 rightRoller.moveVelocity(0);
	 leftRoller.moveVelocity(0);
	 drive->turnAngle(-75_deg);
	 rightRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
	 leftRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
	 pros::delay(200);
	 profileController->setTarget("OutsideBack");
	 profileController->waitUntilSettled();
	 rightRoller.moveVelocity(-10);
	 leftRoller.moveVelocity(-10);
	 pros::delay(1000);
	 pushController->setTarget(2000);
	 pros::delay(2500);
	 pushController->setTarget(0);
	 pros::delay(200);

	 rightRoller.moveVelocity(-15);
	 leftRoller.moveVelocity(-15);
	 topRight.moveVelocity(100);
	 topLeft.moveVelocity(-100);
	 backLeft.moveVelocity(-100);
	 backRight.moveVelocity(100);
	 pros::delay(1500);
	 topRight.moveVelocity(0);
	 topLeft.moveVelocity(0);
	 backLeft.moveVelocity(0);
	 backRight.moveVelocity(0);
 }

void blueIn3pt(){
	 deploy();
	 profileController->setTarget("InsideForwardLittle"); // go forward to cube
	 rollerController->setTarget(100); // start rolling inwards
	 profileController->waitUntilSettled(); // wait until stopped
	 moveBack(20, 300); //move back a little
	 drive->turnAngle(42_deg); // turn to top right cube
	 profileController->setTarget("InsideForward"); // get second cube
	 profileController->waitUntilSettled();
	 drive->turnAngle(50_deg); // turn to third cube
	 profileController->setTarget("InsideRight"); // go to third cube
	 profileController->waitUntilSettled();
	 drive->turnAngle(34_deg); //turn toward goal zone
	 profileController->setTarget("InsideBack"); // go to goal zone
	 rollerController->setTarget(0); // stop rollers
	 profileController->waitUntilSettled();

	 rollerController->setTarget(-30);
	 pushController->setTarget(2000);
	 pushController->waitUntilSettled();
	 rollerController->setTarget(0);
	 rollerController->waitUntilSettled();
	 rollerController->setTarget(50);
	 moveBack(40, 700);
 }

void blueIn2pt(){
	 deploy();
	 profileController->setTarget("InsideForward"); // go forward to cube
	 rollerController->setTarget(170); // start rolling inwards
	 profileController->waitUntilSettled(); // wait until stopped
	 drive->turnAngle(90_deg); // turn to right
	 profileController->setTarget("InsideRight"); // go to third cube
	 profileController->waitUntilSettled();
	 drive->turnAngle(45_deg); // turn to goal zone
	 profileController->setTarget("InsideBack"); // go to goal zone
	 rollerController->setTarget(0); // stop rollers
	 profileController->waitUntilSettled();
	 rollerController->setTarget(-70);
	 pushController->setTarget(1500);
	 pushController->waitUntilSettled();
	 moveBack(40, 1500);
}

void redOut(){
	 deploy();
	 rightRoller.moveVelocity(600);
	 leftRoller.moveVelocity(600);
	 profileController->setTarget("OutsideForward");
	 profileController->waitUntilSettled();
	 rightRoller.moveVelocity(0);
	 leftRoller.moveVelocity(0);
	 drive->turnAngle(75_deg);
	 rightRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
	 leftRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
	 pros::delay(200);
	 profileController->setTarget("OutsideBack");
	 profileController->waitUntilSettled();
	 rightRoller.moveVelocity(-10);
	 leftRoller.moveVelocity(-10);
	 pros::delay(1000);
	 pushController->setTarget(2000);
	 pros::delay(2500);
	 pushController->setTarget(0);
	 pros::delay(200);

	 rightRoller.moveVelocity(-15);
	 leftRoller.moveVelocity(-15);
	 topRight.moveVelocity(100);
	 topLeft.moveVelocity(-100);
	 backLeft.moveVelocity(-100);
	 backRight.moveVelocity(100);
	 pros::delay(1500);
	 topRight.moveVelocity(0);
	 topLeft.moveVelocity(0);
	 backLeft.moveVelocity(0);
	 backRight.moveVelocity(0);
 }

void redIn2pt(){
	 deploy();
	 profileController->setTarget("InsideForward"); // go forward to cube
	 rollerController->setTarget(110); // start rolling inwards
	 profileController->waitUntilSettled(); // wait until stopped
	 drive->turnAngle(-95_deg); // turn to right
	 profileController->setTarget("InsideRight"); // go to third cube
	 profileController->waitUntilSettled();
	 drive->turnAngle(-50_deg); // turn to goal zone
	 profileController->setTarget("InsideBack"); // go to goal zone
	 rollerController->setTarget(0); // stop rollers
	 profileController->waitUntilSettled();
	 rollerController->setTarget(-70);
	 pushController->setTarget(1500);
	 pushController->waitUntilSettled();
	 moveBack(40, 1500);
}

void redIn3pt(){
	 deploy();
	 profileController->setTarget("InsideForwardLittle"); // go forward to cube
	 rollerController->setTarget(100); // start rolling inwards
	 profileController->waitUntilSettled(); // wait until stopped
	 moveBack(20, 300); //move back a little
	 drive->turnAngle(-21_deg); // turn to top right cube
	 profileController->setTarget("InsideForward"); // get second cube
	 profileController->waitUntilSettled();
	 drive->turnAngle(-25_deg); // turn to third cube
	 profileController->setTarget("InsideRight"); // go to third cube
	 profileController->waitUntilSettled();
	 drive->turnAngle(-17_deg); //turn toward goal zone
	 profileController->setTarget("InsideBack"); // go to goal zone
	 rollerController->setTarget(0); // stop rollers
	 profileController->waitUntilSettled();

	 rollerController->setTarget(-30);
	 pushController->setTarget(2000);
	 pushController->waitUntilSettled();
	 rollerController->setTarget(0);
	 rollerController->waitUntilSettled();
	 rollerController->setTarget(50);
	 moveBack(40, 700);
 }

void autonomous() {
	 blueIn2pt();
}

void opcontrol() {
	leftRoller.setGearing(AbstractMotor::gearset::red);
	rightRoller.setGearing(AbstractMotor::gearset::red);
	pusher.setGearing(AbstractMotor::gearset::red);
	int heights[NUM_HEIGHTS] = {height1, height2, height3};
	int pushes[NUM_HEIGHTS] = {push1, push2, push3};
	int goalHeight = 0;

	while (true){

		drive->getModel()->arcade(0.9 * controller.getAnalog(ControllerAnalog::leftY),
		0.75 * controller.getAnalog(ControllerAnalog::rightX));

		tilterControl();

		// // liftPushRoll(rollOutButton, rollInButton, slowRollButton, pushForwardButton,
		// // 	pushBackwardButton, liftUpButton, liftDownButton, upHeight, downHeight, goalHeight);
		//
		// if(runDeploymentButton.changedToPressed()){
		// 	deploy();
		// }
		//
		// if(rollOutButton.isPressed())
		// {
		// 	leftRoller.moveVelocity(ROLLER_SPEED);
		// 	rightRoller.moveVelocity(ROLLER_SPEED);
		// }
		// else if(rollInButton.isPressed())
		// {
		// 	leftRoller.moveVelocity(-ROLLER_SPEED);
		// 	rightRoller.moveVelocity(-ROLLER_SPEED);
		// }
		// else if(slowRollButton.isPressed())
		// {
		// 	leftRoller.moveVelocity(-SLOW_ROLLER_SPEED);
		// 	rightRoller.moveVelocity(-SLOW_ROLLER_SPEED);
		// }
		// else if(pushForwardButton.isPressed())
		// {
		// 	pusher.moveVelocity(PUSHER_SPEED);
		// }
		// else if(pushBackwardButton.isPressed())
		// {
		// 	pusher.moveVelocity(-PUSHER_BACK_SPEED);
		// }
		// else if(liftUpButton.isPressed())
		// {
		// 	twoBar.moveVelocity(-100);
		// }
		// else if(liftDownButton.isPressed())
		// {
		// 	liftController->setTarget(0);
		// }
		// else if(upHeight.changedToPressed() && goalHeight < NUM_HEIGHTS - 1)
		// {
		// 	goalHeight++;
		// 	liftController->setTarget(heights[goalHeight]);
		// 	pushController->setTarget(pushes[goalHeight]);
		//
		// 	if(goalHeight == 1){
		// 		pros::delay(1300); //0 to 1
		// 	}
		// 	else if(goalHeight == 2){
		// 		pros::delay(900); // 1 to 2
		// 	}
		// }
		// else if (downHeight.changedToPressed() && goalHeight > 0) {
		// 	goalHeight--;
		// 	liftController->setTarget(heights[goalHeight]);
		// 	pushController->setTarget(pushes[goalHeight]);
		//
		// 	if(goalHeight == 0){
		// 		pros::delay(1300); // 1 to 0
		// 	}
		// 	else if(goalHeight == 1){
		// 		pros::delay(900); // 2 to 1
		// 	}
		// }
		// else
		// {
		// 	pusher.moveVelocity(0);
		// 	twoBar.moveVelocity(0);
		// 	leftRoller.moveVelocity(0);
		// 	rightRoller.moveVelocity(0);
		// 	rightRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
		// 	leftRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
		// 	twoBar.setBrakeMode(AbstractMotor::brakeMode::hold);
		// 	pusher.setBrakeMode(AbstractMotor::brakeMode::hold);
		// }


		pros::delay(20);
	}
}
