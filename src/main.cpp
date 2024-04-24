#include "main.h"

// Motor Ports
const int LEFT_CAT_MOTOR1_PORT = 13;
const int LEFT_CAT_MOTOR2_PORT = 14;
const int RIGHT_CAT_MOTOR1_PORT = 2;
const int RIGHT_CAT_MOTOR2_PORT = 4;
const int Puncher_Motor_Port = 10;
const int SPIN_MOTOR_PORT = 12;

int count = 0;

pros::ADIDigitalIn limit('A');

// Declarations
pros::Motor left_cat_motor1(LEFT_CAT_MOTOR1_PORT, pros::E_MOTOR_GEAR_GREEN, false,
                           pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor left_cat_motor2(LEFT_CAT_MOTOR2_PORT, pros::E_MOTOR_GEAR_GREEN, true,
                            pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor right_cat_motor1(RIGHT_CAT_MOTOR1_PORT, pros::E_MOTOR_GEAR_GREEN,
                            true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor right_cat_motor2(RIGHT_CAT_MOTOR2_PORT, pros::E_MOTOR_GEAR_GREEN,
                             false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor puncher_motor(Puncher_Motor_Port, pros::E_MOTOR_GEAR_RED, true,
                          pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor spin_motor(SPIN_MOTOR_PORT, pros::E_MOTOR_GEAR_RED, true,
                       pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor_Group catapult({left_cat_motor1, left_cat_motor2, right_cat_motor1, right_cat_motor2});

pros::Controller master(pros::E_CONTROLLER_MASTER);

double dabs(double v) { return v < 0 ? -v : v; }

// Global timer
double start_time;
double current_time;

void delay(int time) {
  current_time = start_time;
  while (current_time - start_time < time) {
    current_time = pros::millis();
  }
  start_time += time;
}

/**
 * This function moves the puncher_motor 165 degrees,
 * then stops for half a second and then moves back to its original position
 */
void punch() {
  // puncher_motor.move_relative(140,60);
  puncher_motor.brake();
  puncher_motor.move_relative(140, 100); // was 70 rpm
  double position = puncher_motor.get_position();
  while (int i = dabs(puncher_motor.get_position() - position) < 130) {
    puncher_motor.modify_profiled_velocity(sqrt((140*140) - (i*i)));
    i = dabs(puncher_motor.get_position() - position);
    delay(20);
  }
  

  delay(500);
  // puncher_motor.move_relative(-140, 70);
  puncher_motor.move_relative(-140, 100); // was 70 rpm
  double position2 = puncher_motor.get_position();
  while (int i = dabs(puncher_motor.get_position() - position2) < 130) {
    puncher_motor.modify_profiled_velocity(sqrt((140*140) - (i*i)));
    i = dabs(puncher_motor.get_position() - position2);
    delay(20);
  }

  delay(1000);
}

void initialize() { puncher_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST); }

void disabled() {}

void competition_initialize() {}

// We have one preload, one alliance triball, and 10 match loads that can be
// introduced in autonomous Small Robot also has one preload
void autonomous() {
  // get the current time
  start_time = pros::millis();
  // while loop delay until 5 seconds have passed
  delay(5000);
  // Run the catapult once which is 723 degrees to lauch preload
  catapult.move_relative(-723, 100);
  puncher_motor.move_relative(-100, 70);
  delay(500);
  // Then loop to load and launch catapult 11 times.
  for (int i = 0; i < 11; i++) { // 11 for normal matches
    punch(); // 1.5 seconds
    delay(750);
    catapult.move_relative(-723, 100);
    delay(750);
  }
}

void opcontrol() {
  bool joystickEnable = false;
  spin_motor.move_relative(300, 20);
  while (true) {
    // Launch and reload the catapult
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
      catapult.move_relative(-723, 100);
      // if the joystick is not at 0, move the catapult manuelly to fix cam
      // issues
    } else if (master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y) != 0 && joystickEnable == true) {
      const double MAX_RPM = 30.;
      int t = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
      if (t < -10 || t > 10)
        catapult.move_velocity((int)((double)(t) / 127. * MAX_RPM));
      else
        catapult.move_velocity(0);
    } else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)) {
      joystickEnable = !joystickEnable;
    }
    
    /**
     * IMPORTANT: Drop the intake to get match load introduced
     * between autonomous and driver control
     */
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
	punch();
    }

    pros::delay(20);
  }
}
