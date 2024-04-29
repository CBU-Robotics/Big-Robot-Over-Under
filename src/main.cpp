#include "main.h"

// Motor Ports
const int LEFT_CAT_MOTOR1_PORT = 9;
const int LEFT_CAT_MOTOR2_PORT = 10;
const int RIGHT_CAT_MOTOR1_PORT = 1;
const int RIGHT_CAT_MOTOR2_PORT = 2;
const int Puncher_Motor_Port = 5;
const int SPIN_MOTOR_PORT = 12;
const int HOLO_SENSOR_PORT = 'A';

int count = 0;

// Declarations
pros::Motor left_cat_motor1(LEFT_CAT_MOTOR1_PORT, pros::E_MOTOR_GEAR_RED, false,
          pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor left_cat_motor2(LEFT_CAT_MOTOR2_PORT, pros::E_MOTOR_GEAR_RED, true,
          pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor right_cat_motor1(RIGHT_CAT_MOTOR1_PORT, pros::E_MOTOR_GEAR_RED,
           true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor right_cat_motor2(RIGHT_CAT_MOTOR2_PORT, pros::E_MOTOR_GEAR_RED,
           false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor puncher_motor(Puncher_Motor_Port, pros::E_MOTOR_GEAR_RED, false,
        pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor spin_motor(SPIN_MOTOR_PORT, pros::E_MOTOR_GEAR_RED, true,
           pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor_Group catapult({left_cat_motor1, left_cat_motor2, right_cat_motor1,
          right_cat_motor2});

pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::ADIDigitalIn holo_sensor(HOLO_SENSOR_PORT);

double dabs(double v) { return v < 0.0 ? -v : v; }

// Global timer
double start_time;
double current_time;

// void delay(int time) {
//   current_time = start_time;
//   while (current_time - start_time < time) {
//     current_time = pros::millis();
//   }
//   start_time += time;
// }

/**
 * This function moves the puncher_motor 165 degrees,
 * then stops for half a second and then moves back to its original position
 */

void fix_triball() {
  // setting the triball
  puncher_motor.move_velocity(50);
  while (puncher_motor.get_current_draw() < 2200) {
    pros::delay(20);
  }
  puncher_motor.brake();
  pros::delay(300);

  // moving back to center
  puncher_motor.move_velocity(-60);
  double position = puncher_motor.get_position();
  while (dabs(puncher_motor.get_position() - position) < 60) {
    pros::delay(20);
  }
  puncher_motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  puncher_motor.brake();
  pros::delay(300);
  puncher_motor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
}

void punch() {
  // grabbing the triball
  puncher_motor.move_velocity(-75);
  while (puncher_motor.get_current_draw() < 2200) {
    pros::delay(20);
  }

  puncher_motor.brake();
  pros::delay(300);

  fix_triball();
}

//function returns average current of catapult motors
int get_average_current(){
  return (dabs(left_cat_motor1.get_current_draw())+dabs(left_cat_motor2.get_current_draw())+
    dabs(right_cat_motor1.get_current_draw())+dabs(right_cat_motor2.get_current_draw()))/4;
}

void fix_catapult(){
  catapult.move_velocity(25);
      
  while(get_average_current() < 1000){
    pros::delay(10);
  }

  catapult.brake();

  catapult.move_relative(-362, 100);
}

//function returns average torque of catapult motors
double get_average_torque(){
  return (dabs(left_cat_motor1.get_torque())+dabs(left_cat_motor2.get_torque())+
    dabs(right_cat_motor1.get_torque())+dabs(right_cat_motor2.get_torque()))/4;
}


void launch(bool auto_fix) {
  //start moving catapult to hard coded position
  std::cout << "launch works" << std::endl;

  double start_pos = left_cat_motor1.get_position();
  catapult.move_velocity(-100);


  while (dabs(left_cat_motor1.get_position() - start_pos) < 358){
    std::cout << "pos " << dabs(left_cat_motor1.get_position() - start_pos) << std::endl;
    
    if (auto_fix && get_average_torque() < 0.1){
      fix_catapult();
    }

    pros::delay(2);
  }
  catapult.brake();
}

void initialize() {
  puncher_motor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
  catapult.set_brake_modes(pros::E_MOTOR_BRAKE_HOLD);
}

void disabled() {}

void competition_initialize() {}

// We have one preload, one alliance triball, and 10 match loads that can be
// introduced in autonomous Small Robot also has one preload
void autonomous() {
  fix_triball();
  // get the current time
  start_time = pros::millis();
  // while loop delay until 5 seconds have passed
  pros::delay(5000);
  // Run the catapult once which is 361 degrees to lauch preload
  // catapult.move_relative(-361, 100);
  launch(true);

  //
  // Removed the inial movement down to pick up a ball as that is handled
  // in punch now Currently untested
  //

  pros::delay(500);
  // Then loop to load and launch catapult 11 times.
  for (int i = 0; i < 11; i++) { // 11 for normal matches
    punch();         // 1.5 seconds
    pros::delay(750);
    // catapult.move_relative(-361, 100);
    launch(true);
    pros::delay(750);
  }
}

void opcontrol() {
  bool joystickEnable = false;
  spin_motor.move_relative(300, 20);
  while (true) {
    // Launch and reload the catapult
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
      std::cout << "button A works" << std::endl;
      // catapult.move_relative(-361, 100);
      launch(false);
    }

    /**
     * IMPORTANT: Drop the intake to get match load introduced
     * between autonomous and driver control
     */
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
      punch();
    }

    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
      fix_catapult();
    }

    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)) {
      fix_triball();
    }

    pros::delay(20);
  }

}
