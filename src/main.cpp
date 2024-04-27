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

pros::Motor_Group catapult({left_cat_motor1, left_cat_motor2, right_cat_motor1, right_cat_motor2});

pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::ADIDigitalIn holo_sensor(HOLO_SENSOR_PORT);

double dabs(double v) {
	return v < 0.0 ? -v : v;
}


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
  int setAngle = 190;
  int startAngle = 135;

  // grabing the triball
  puncher_motor.move_velocity(-120);
  double position = puncher_motor.get_position();
  while (dabs(puncher_motor.get_position() - position) < startAngle) {
    delay(20);
  }
  puncher_motor.brake();
  delay(300);

  // setting the triball
  puncher_motor.move_velocity(50);
  position = puncher_motor.get_position();
  while (dabs(puncher_motor.get_position() - position) < setAngle) {
    delay(20);
  }
  puncher_motor.brake();
  delay(300);
  
  // moving back to center
  puncher_motor.move_velocity(-70);
  position = puncher_motor.get_position();
  while (dabs(puncher_motor.get_position() - position) < setAngle - startAngle - 10) {
    delay(20);
  }
  puncher_motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  puncher_motor.brake();
  delay(300);
  puncher_motor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
}

void launch() {
  catapult.move_velocity(-100);
  while (holo_sensor.get_value()) {} // Allow cam to leave holo
  // Loop while holo is false
  while(true) {
    // Check holo sensor
    if(holo_sensor.get_value()) {
      catapult.brake();
      break;
    }
  }

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
  // get the current time
  start_time = pros::millis();
  // while loop delay until 5 seconds have passed
  delay(5000);
  // Run the catapult once which is 361 degrees to lauch preload
  // catapult.move_relative(-361, 100);
  launch();

  //
  // Removed the inial movement down to pick up a ball as that is handled in punch now
  // Currently untested
  //

  delay(500);
  // Then loop to load and launch catapult 11 times.
  for (int i = 0; i < 11; i++) { // 11 for normal matches
    punch(); // 1.5 seconds
    delay(750);
    // catapult.move_relative(-361, 100);
    launch();
    delay(750);
  }
}

void opcontrol() {
  bool joystickEnable = false;
  spin_motor.move_relative(300, 20);
  while (true) {
    // Launch and reload the catapult
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
      // catapult.move_relative(-361, 100);
      launch();
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
