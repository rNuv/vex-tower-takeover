# VEX-Tower-Takeover
My 2019-2020 VEX Robotics season saw more experience with more advanced code techniques and methodologies than that of previous years. The code in this repo shows off the programming that went into our robot for Tower Takeover. We switched to PROS this year (instead of the traditional VEXCode) and made use of the Okapilib to (hopefully) transition to a smoother and more accurate autonomous routine. With techniques such as motion profiling and odometry, our robot began to win more auton points and make our way into semifinals at States. 

The descriptions below outline some fancy features I did to make our bot perform better. 

## 2D Motion Profiling

The OkapiLib has functionality for 2D Motion Profiling, essentially prerouting specific endpoints throughout the field. Imagine the VEX field as a 2D coordinate plane. All you would have to do is tell your bot to move to certain coordinates, and the methods will automatically calculate the motions and mechanics neccessary to get there. It also supports curvilinear motion (traveling in curves instead of straight lines), which is the coolest thing I've ever seen.

```Cpp
void initialize() {
  medDrive->generatePath({
    {0_ft, 0_ft, 0_deg},  
    {3.35_ft, 0_ft, 0_deg}},
    "OutsideForward"
  );

  medDrive->generatePath({
    {0_ft, 0_ft, 0_deg},
    {3.05_ft, 0_ft, 0_deg}},
    "OutsideRight"
  );

  fastDrive->generatePath({
    {0_ft, 0_ft, 0_deg},
    {2.56_ft, 0_ft, 0_deg}},
    "OutsideBack"
  );

  fastDrive->generatePath({
    {0_ft, 0_ft, 0_deg},
    {2.0_ft, 0_ft, 0_deg}},
    "Inside6ptForward"
  );

  fastDrive->generatePath({
    {0_ft, 0_ft, 0_deg},
    {2.35_ft, 0_ft, 0_deg}},
    "Inside6ptBack"
  );

  fastDrive->generatePath({
    {0_ft, 0_ft, 0_deg},
    {0.7_ft, 0_ft, 0_deg}},
    "Inside6ptBackLil"
  );
}
```
## LVGL Library

To prevent having to reupload autonomous code for different sides of the field, I decided to make an auton selector with the Little vGL library. This way, the drive team can just select the auton they want right before the match starts. Plus, it looks so cool..

```Cpp
void selectorInit(){
  
  // lvgl theme
  lv_theme_t *th = lv_theme_zen_init(360, NULL);
  lv_theme_set_current(th);
  
  // create a tab view object
  lv_obj_t *tabview;
  tabview = lv_tabview_create(lv_scr_act(), NULL);
  
  // add 3 tabs (the tabs are page (lv_page) and can be scrolled through)
  lv_obj_t *redTab = lv_tabview_add_tab(tabview, "Red");
  lv_obj_t *blueTab = lv_tabview_add_tab(tabview, "Blue");
  lv_obj_t *skillsTab = lv_tabview_add_tab(tabview, "Skills");
  
  lv_obj_t *redbtnmp = lv_btnm_create(redTab, NULL);
  lv_btnm_set_map(redbtnmp, btnmMap);
  lv_btnm_set_action(redbtnmp, redBtnmAction);
  lv_btnm_set_toggle(redbtnmp, true, 4); //3
  lv_obj_set_size(redbtnmp, 450, 50);
  lv_obj_set_pos(redbtnmp, 0, 100);
  lv_obj_align(redbtnmp, NULL, LV_ALIGN_CENTER, 0, 0);
  
  lv_obj_t *bluebtnmp = lv_btnm_create(blueTab, NULL);
  lv_btnm_set_map(bluebtnmp, btnmMap);
  lv_btnm_set_action(bluebtnmp, blueBtnmAction);
  lv_btnm_set_toggle(bluebtnmp, true, 4); //3
  lv_obj_set_size(bluebtnmp, 450, 50);
  lv_obj_set_pos(bluebtnmp, 0, 100);
  lv_obj_align(bluebtnmp, NULL, LV_ALIGN_CENTER, 0, 0);
  
  lv_obj_t *skillsBtn = lv_btn_create(skillsTab, NULL);
  ...

}
```

## V5 Inertial Sensor and PID

We somehow got our hands on the new V5 Inertial Sensor. With its crazy accuracy, our turning angles became monumentally precise. To complement the sensor, I decided to implement a PID (proportional, intergral, and derivative) controller to make turning astronomically consistent. In a nutshell, the bot calculates the angle left to turn as it turns (known as the error) and slows changes its speed proporional to that angle, so it slows down as it reaches its target. 

```cpp
void gyroPid (double angle) {

  double threshold;
  
  if(angle <= 0.0) {
    threshold = 1.5;
  }
  
  else {
    threshold = 0.7;
  }

  // variable instantiations
  double error = angle - imu_sensor.get_rotation();
  double integral;
  double derivative;
  double prevError;
  double kp = 0.98;
  double ki = 0.001;
  double kd = 5.5;

  while(fabs(error) > threshold) {
    error = angle - imu_sensor.get_rotation();
    integral  = integral + error;
    if(error == 0 || fabs(error) >= angle) {
      integral = 0;
    }
  
    derivative = error - prevError;
    prevError = error;
    double p = error * kp;
    double i = integral * ki;
    double d = derivative * kd;

    double vel = p + i + d;

    topRight.moveVelocity(-vel);
    topLeft.moveVelocity(vel);
    backRight.moveVelocity(-vel);
    backLeft.moveVelocity(vel);

    pros::delay(15);
  }
}
```

## Parabolic Stacking

Stacking was a huge part of this game. Going too fast could knock down the cubes and going too slow was just took too much time. I had the motor responsible for the pusher move as a function of the motor's encoder position so that it follows a parabolic behavior. Essentially, it slows down as it goes forward, getting the best of both worlds.

```cpp
void pusherControl() {

  pusher.setGearing(AbstractMotor::gearset::red);

  if (pushBackwardButton.isPressed() && pusher.getTargetVelocity() != 40) {
    pusher.moveVelocity(-100);
    
    if (pusher.getPosition() < 0) {
      pusher.tarePosition();
    }
    
  }
  
  else if (pushForwardButton.isPressed()) {
    rightRoller.setBrakeMode(AbstractMotor::brakeMode::brake);
    leftRoller.setBrakeMode(AbstractMotor::brakeMode::brake);
    
    if (pusher.getPosition() < 1450) {
      pusher.moveVelocity(100);
    }

    else if(pusher.getPosition() > 3800) {
      pusher.moveVelocity(0);
    }
    
    else {
      int x = pusher.getPosition();
      // parabolic equation
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
```
