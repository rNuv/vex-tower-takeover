#include "main.h"

int TOP_LEFT_DRIVE = 2;
int TOP_RIGHT_DRIVE = -4;
int BOTTOM_RIGHT_DRIVE = -3;
int BOTTOM_LEFT_DRIVE = 1;

int TWO_BAR = 14;
int RIGHT_ROLLER = 11;
int LEFT_ROLLER = 20;
int PUSHER = -5;

Controller controller;

ControllerButton backDriveButton(ControllerDigital::B);
ControllerButton slowRollOut(ControllerDigital::A);
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
Motor backLeft(BOTTOM_LEFT_DRIVE);
Motor backRight(BOTTOM_RIGHT_DRIVE);

auto drive = ChassisControllerBuilder()
.withMotors({TOP_LEFT_DRIVE, BOTTOM_LEFT_DRIVE}, {TOP_RIGHT_DRIVE, BOTTOM_RIGHT_DRIVE})
// Green gearset, 4 in wheel diam, 11.5 in wheel track
// .withGains(
//     {0.001, 0, 0.0001}, // Distance controller gains
//     {0.001, 0, 0.0001}, // Turn controller gains
//     {0.001, 0, 0.0001}  // Angle controller gains (helps drive straight)
// )
.withDimensions(AbstractMotor::gearset::green, {{4_in, 9_in}, 450})
.withOdometry()
.buildOdometry();

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */


void lift(ControllerButton up, ControllerButton down)
{
    if(up.isPressed())
    {
        pusher.moveAbsolute(-190, 200);
        pros::delay(200);
        twoBar.moveAbsolute(800, 100);
    }
    else if(down.isPressed())
    {
        twoBar.moveVelocity(-200);
        // joe mama!
        //whos joe
    }
    else{
        pusher.setBrakeMode(AbstractMotor::brakeMode::hold);
        twoBar.setBrakeMode(AbstractMotor::brakeMode::hold);
        twoBar.moveVelocity(0);
    }
}

void backDrive(ControllerButton drive)
{
    if(drive.isPressed())
    {
        backLeft.moveVelocity(600);
        backRight.moveVelocity(600);
    }
}

void roll(ControllerButton up, ControllerButton down, ControllerButton out)
{
    rightRoller.setGearing(AbstractMotor::gearset::red);
    leftRoller.setGearing(AbstractMotor::gearset::red);
    
    if(up.isPressed())
    {
        leftRoller.moveVelocity(500);
        rightRoller.moveVelocity(500);
    }
    else if(down.isPressed())
    {
        leftRoller.moveVelocity(-500);
        rightRoller.moveVelocity(-500);
    }
    else if(out.isPressed()){
        leftRoller.moveVelocity(-50);
        rightRoller.moveVelocity(-50);
    }
    else
    {
        leftRoller.moveVelocity(0);
        rightRoller.moveVelocity(0);
        rightRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
        leftRoller.setBrakeMode(AbstractMotor::brakeMode::hold);
    }
}

void push(ControllerButton forward, ControllerButton backward)
{
    pusher.setGearing(AbstractMotor::gearset::red);
    
    if(forward.isPressed())
    {
        leftRoller.moveVelocity(-3);
        rightRoller.moveVelocity(-3);
        pusher.moveVelocity(-30);
    }
    else if(backward.isPressed())
    {
        pusher.moveVelocity(30);
    }
    else
    {
        pusher.moveVelocity(0);
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
    pros::lcd::set_text(1, "Hello R_N#V2");
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

void blueOut(){
}

void blueIn(){
    
}

void redOut(){
    
}

void redIn(){
    
}

void autonomous() {
    drive->moveDistance(1_ft);
    drive->turnAngle(90_deg);
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


void opcontrol() {
    while (true)
    {
        
        drive->getModel()->arcade(controller.getAnalog(ControllerAnalog::leftY),
                                  controller.getAnalog(ControllerAnalog::rightX));
        
        lift(liftUpButton, liftDownButton);
        roll(rollUpButton, rollDownButton, slowRollOut);
        push(pushForwardButton, pushBackwardButton);
        backDrive(backDriveButton);
        
        if(runAutoButton.changedToPressed()){
            autonomous();
        }
        
        pros::delay(20);
    }
}
