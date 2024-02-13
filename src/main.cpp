#include "main.h"

// Motor Ports
const int LEFT_CAT_MOTOR_PORT = 11;
const int RIGHT_CAT_MOTOR_PORT = 2;
const int Puncher_Motor_Port = 10;

int count = 0;

pros::ADIDigitalIn limit('A');

// Declarations
pros::Motor left_cat_motor(LEFT_CAT_MOTOR_PORT, pros::E_MOTOR_GEAR_GREEN, false,
                           pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor right_cat_motor(RIGHT_CAT_MOTOR_PORT, pros::E_MOTOR_GEAR_GREEN,
                            true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor puncher_motor(Puncher_Motor_Port, pros::E_MOTOR_GEAR_RED, true,
                          pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor_Group catapult({left_cat_motor, right_cat_motor});

pros::Controller master(pros::E_CONTROLLER_MASTER);

/**
 * This function moves the puncher_motor 165 degrees,
 * then stops for half a second and then moves back to its original position
 */
void punch() {
  puncher_motor.move_relative(100, 40);
  pros::delay(1000);
  puncher_motor.move_relative(-100, 80);
  pros::delay(1000);
  puncher_motor.brake();
}

void initialize() { puncher_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST); }

void disabled() {}

void competition_initialize() {}

void autonomous() {
  pros::delay(13500);
  // Run the catapult once which is 720 degrees
  // Then loop 10 times punching twice followed by another catapult launch.
  catapult.move_relative(-720, 100);
  pros::delay(3000);
  for (int i = 0; i < 11; i++) {
    punch(); // 2 seconds
    catapult.move_relative(-720, 100);
    pros::delay(3000);
  }
}

double dabs(double v) { return v < 0 ? -v : v; }

void opcontrol() {

  while (true) {
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
      catapult.move_relative(-720, 100);
      double p = catapult.get_positions().at(0);
      int t = 200;
      while (dabs(catapult.get_positions().at(0) - p) < 720 && t != 0) {
        pros::delay(20);
	t --;
      }
    } else {
      const double MAX_RPM = 30.;
      int t = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
      if (t < -10 || t > 10)
        catapult.move_velocity((int)((double)(t) / 127. * MAX_RPM));
      else
        catapult.move_velocity(0);
    }
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
      punch();
    }

    pros::delay(20);
  }
}
