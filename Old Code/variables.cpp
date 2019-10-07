#include "variables.h"
#include "main.h"
#include <math.h>

pros::Motor move1 (4);
pros::Motor move2 (2);
pros::Motor move3 (1);
pros::Motor move4 (-3);

pros::Motor twoBar (5);
pros::Motor leftRoller (6);
pros::Motor rightRoller (7);
pros::Motor pusher (8);


void drive(int ch1, int ch2, int ch3, int ch4)
{
  move4.move(ch3  + ch4 + ch1);
  move1.move(ch3 - ch4 + ch1);
  move2.move(-(ch3 - ch4 - ch1));
  move3.move(-(ch3 + ch4 - ch2));
}

void lift(bool liftUpButton, bool liftDownButton)
{
  if(liftUpButton)
  {
    twoBar.move_velocity(100);
  }
  else if(liftDownButton)
  {
    twoBar.move_velocity(-100);
  }
  else{
    twoBar.move_velocity(0);
  }
}

void roll(bool rollUpButton, bool rollDownButton)
{
  if(rollUpButton)
  {
    leftRoller.move_velocity(100);
    rightRoller.move_velocity(-100);
  }
  else if(rollDownButton)
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
void push(bool pushForwardButton, bool pullBackwardsButton){
  if(pushForwardButton)
  {
    pusher.move_velocity(20);
  }
  else if(pullBackwardsButton)
  {
    pusher.move_velocity(-20);
  }
  else
  {
    pusher.move_velocity(0);
  }
}
