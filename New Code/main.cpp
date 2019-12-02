#include "main.h"

int TOP_LEFT_DRIVE = 7;
int TOP_RIGHT_DRIVE = -8;
int BOTTOM_RIGHT_DRIVE = -9;
int BOTTOM_LEFT_DRIVE = 10;
int TWO_BAR = 14;
int RIGHT_ROLLER = 11;
int LEFT_ROLLER = 20;
int PUSHER = 17;

Controller masterController;

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
        double value = 1.0;
        pros::lcd::print(2, "Motor value: %d", value);
    } else {
        pros::lcd::clear_line(2);
    }
}

double getStrafeSpeed(ControllerButton left, ControllerButton right)
{
    if(left.isPressed())
    {
        return -1;
    }
    else if (right.isPressed())
    {
        return 1;
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
        pusher.moveAbsolute(-190, 200);
        pros::delay(200);
        twoBar.moveAbsolute(800, 100);
    }
    else if(down.isPressed())
    {
        twoBar.move_velocity(-200);
        // joe mama!
        //whos joe
    }
    else{
        pusher.setBrakeMode(AbstractMotor::brakeMode::hold);
        twoBar.setBrakeMode(AbstractMotor::brakeMode::hold);
        twoBar.move_velocity(0);
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
        leftRoller.move_velocity(500);
        rightRoller.move_velocity(500);
    }
    else if(down.isPressed())
    {
        leftRoller.move_velocity(-500);
        rightRoller.move_velocity(-500);
    }
    else if(out.isPressed()){
        leftRoller.move_velocity(-50);
        rightRoller.move_velocity(-50);
    }
    else
    {
        leftRoller.move_velocity(0);
        rightRoller.move_velocity(0);
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
        pusher.move_velocity(-30);
    }
    else if(backward.isPressed())
    {
        pusher.move_velocity(30);
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

auto myChassisF = ChassisControllerFactory::create(
                                                   7, -8, -9, 10,
                                                   AbstractMotor::gearset::green,
                                                   {4_in, 12.5_in}
                                                   );
auto profileControllerF = AsyncControllerFactory::motionProfile(
                                                                0.5,  // Maximum linear velocity of the Chassis in m/s
                                                                1.0,  // Maximum linear acceleration of the Chassis in m/s/s
                                                                10.0, // Maximum linear jerk of the Chassis in m/s/s/s
                                                                myChassisF // Chassis Controller
                                                                );

void initialize() {
    pros::lcd::initialize();
    pros::lcd::set_text(1, "Hello R_N#V2");
    
    pros::lcd::register_btn1_cb(on_center_button);
    //profileControllerF.generatePath({Point{0_ft, 0_ft, 0_deg}, Point{2.5_ft, 0_ft, 0_deg}}, "A");
    profileControllerF.generatePath({Point{0_ft, 0_ft, 0_deg}, Point{1.25_ft, 0_ft, 0_deg}}, "A");
    profileControllerF.generatePath({Point{0_ft, 0_ft, 0_deg}, Point{1.45_ft, 0_ft, 0_deg}}, "B");
    //profileControllerF.generatePath({Point{0_ft, 0_ft, 0_deg}, Point{0.95_ft, 0_ft, 0_deg}}, "C");
    profileControllerF.generatePath({Point{0_ft, 0_ft, 0_deg}, Point{0.22_ft, 0_ft, 0_deg}}, "C");
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

// auto myChassisR = ChassisControllerFactory::create(
//      3, -4, -1, 2,
//      AbstractMotor::gearset::green,
//      {4_in, 12.5_in}
// );
// auto profileControllerR = AsyncControllerFactory::motionProfile(
//   0.25,  // Maximum linear velocity of the Chassis in m/s
//   1.0,  // Maximum linear acceleration of the Chassis in m/s/s
//   10.0, // Maximum linear jerk of the Chassis in m/s/s/s
//   myChassisR // Chassis Controller
// );

auto rollerController = AsyncControllerFactory::velIntegrated({-11, 20});

void trashAuton(){
    profileControllerF.setTarget("A");
    profileControllerF.waitUntilSettled();
}


void blueOut(){
    
    //Deployment
    pusher.setBrakeMode(AbstractMotor::brakeMode::hold);
    pusher.moveRelative(-300, 75);
    pros::delay(2000);
    pusher.moveRelative(300, 75);
    pros::delay(1000);
    twoBar.moveRelative(650, 50);
    pros::delay(1000);
    rollerController.setTarget(500); //intake rollers
    profileControllerF.setTarget("A"); //go forward
    twoBar.moveRelative(-650, 50);
    //
    
    profileControllerF.waitUntilSettled(); //4 cubes in
    myChassisF.turnAngle(-300_deg); //turn toward goal zone
    myChassisF.waitUntilSettled(); // facing goal zone
    rollerController.setTarget(0); //stop rollers
    profileControllerF.setTarget("B"); //go to goal zone
    profileControllerF.waitUntilSettled(); //at goal zone
    rollerController.setTarget(-15);//slow roll out
    pros::delay(2500);//for 2.5 seconds
    rollerController.setTarget(0);//stop rollers
    rollerController.waitUntilSettled();
    pusher.moveRelative(-800, 80); //push pusher at same time as roller
    rollerController.setTarget(-5); //slow roll out
    pros::delay(3000); //complete stack for 3 second
    
    rollerController.setTarget(-7); //stop roller
    pusher.moveVelocity(50); //pull pusher back
    pros::delay(2000);
    rollerController.setTarget(0);
    pros::delay(1500);
    pusher.moveVelocity(0);
    backLeft.moveVelocity(-600);
    backRight.moveVelocity(-600);
    pros::delay(500);
    backLeft.moveVelocity(0);
    backRight.moveVelocity(0);
}

void blueIn(){
    //Deployment
    pusher.setBrakeMode(AbstractMotor::brakeMode::hold);
    pusher.moveRelative(-300, 100);
    pros::delay(2000);
    pusher.moveRelative(300, 75);
    pros::delay(1000);
    twoBar.moveRelative(800, 85);
    pros::delay(1000);
    twoBar.moveRelative(-800, 85);
    pros::delay(1000);
    rollerController.setTarget(500); //intake rollers
    profileControllerF.setTarget("A"); //go forward
    //
    profileControllerF.waitUntilSettled();
    myChassisF.turnAngle(189_deg); //turn toward goal zone
    profileControllerF.setTarget("B");
    rollerController.setTarget(500);
    profileControllerF.waitUntilSettled();
    myChassisF.turnAngle(103_deg);
    profileControllerF.setTarget("C");
    profileControllerF.waitUntilSettled();
    //
    rollerController.setTarget(-500);//slow roll out
    //pusher.moveRelative(-800, 80);
    pros::delay(4500);//for 2.5 seconds
    backLeft.moveVelocity(-600);
    backRight.moveVelocity(-600);
    pros::delay(800);
    backLeft.moveVelocity(0);
    backRight.moveVelocity(0);
}

void redOut(){
    
    //Deployment
    pusher.setBrakeMode(AbstractMotor::brakeMode::hold);
    pusher.moveRelative(-300, 75);
    pros::delay(2000);
    pusher.moveRelative(300, 75);
    pros::delay(1000);
    twoBar.moveRelative(650, 50);
    pros::delay(1000);
    rollerController.setTarget(500); //intake rollers
    profileControllerF.setTarget("A"); //go forward
    twoBar.moveRelative(-650, 50);
    //
    
    profileControllerF.waitUntilSettled(); //4 cubes in
    myChassisF.turnAngle(300_deg); //turn toward goal zone
    myChassisF.waitUntilSettled(); // facing goal zone
    rollerController.setTarget(0); //stop rollers
    profileControllerF.setTarget("B"); //go to goal zone
    profileControllerF.waitUntilSettled(); //at goal zone
    rollerController.setTarget(-15);//slow roll out
    pros::delay(2500);//for 2.5 seconds
    rollerController.setTarget(0);//stop rollers
    rollerController.waitUntilSettled();
    pusher.moveRelative(-800, 80); //push pusher at same time as roller
    rollerController.setTarget(-5); //slow roll out
    pros::delay(3000); //complete stack for 3 second
    
    rollerController.setTarget(-7); //stop roller
    pusher.moveVelocity(50); //pull pusher back
    pros::delay(2000);
    rollerController.setTarget(0);
    pros::delay(1500);
    pusher.moveVelocity(0);
    backLeft.moveVelocity(-600);
    backRight.moveVelocity(-600);
    pros::delay(500);
    backLeft.moveVelocity(0);
    backRight.moveVelocity(0);
}

void redIn(){
    //Deployment
    pusher.setBrakeMode(AbstractMotor::brakeMode::hold);
    pusher.moveRelative(-300, 100);
    pros::delay(2000);
    pusher.moveRelative(300, 75);
    pros::delay(1000);
    twoBar.moveRelative(800, 85);
    pros::delay(1000);
    twoBar.moveRelative(-800, 85);
    pros::delay(1000);
    rollerController.setTarget(500); //intake rollers
    profileControllerF.setTarget("A"); //go forward
    //
    profileControllerF.waitUntilSettled();
    myChassisF.turnAngle(-189_deg); //turn toward goal zone
    profileControllerF.setTarget("B");
    rollerController.setTarget(500);
    profileControllerF.waitUntilSettled();
    myChassisF.turnAngle(-103_deg);
    profileControllerF.setTarget("C");
    profileControllerF.waitUntilSettled();
    //
    rollerController.setTarget(-500);//slow roll out
    //pusher.moveRelative(-800, 80);
    pros::delay(4500);//for 2.5 seconds
    backLeft.moveVelocity(-600);
    backRight.moveVelocity(-600);
    pros::delay(800);
    backLeft.moveVelocity(0);
    backRight.moveVelocity(0);
}

void autonomous() {
    redIn();
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
        
        xChassis.xArcade(strafeSpeed*0.5, masterController.getAnalog(ControllerAnalog::leftY),
                         (masterController.getAnalog(ControllerAnalog::rightX))*0.7);
        
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
