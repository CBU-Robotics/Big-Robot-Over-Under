#include "main.h"

// Motor Ports
const int LEFT_CAT_MOTOR_PORT = 11;
const int RIGHT_CAT_MOTOR_PORT = 2;
const int Puncher_Motor_Port = 10;

int count = 0;

pros::ADIDigitalIn limit ('A');

// Declarations
pros::Motor left_cat_motor(LEFT_CAT_MOTOR_PORT, pros::E_MOTOR_GEAR_GREEN, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor right_cat_motor(RIGHT_CAT_MOTOR_PORT, pros::E_MOTOR_GEAR_GREEN, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor puncher_motor(Puncher_Motor_Port, pros::E_MOTOR_GEAR_RED, true, pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor_Group catapult({left_cat_motor, right_cat_motor});

pros::Controller master(pros::E_CONTROLLER_MASTER);

/**
 * This function moves the puncher_motor 165 degrees,
 * then stops for half a second and then moves back to its original position
*/
void punch() {
    puncher_motor.move_relative(135, 40);
    pros::delay(2000);
    puncher_motor.move_relative(-135, 40);
    pros::delay(1000);
}

void initialize() {
    puncher_motor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
}

void disabled() {}

void competition_initialize() {
}

void autonomous() {
    pros::delay(13500); 
    // Run the catapult once which is 720 degrees
    // Then loop 10 times punching twice followed by another catapult launch.
    catapult.move_relative(720, 30);
    pros::delay(4000);
    for (int i = 0; i < 10; i++) {
        punch();
        catapult.move_relative(720, 30);
        pros::delay(4000);
    }
}

void opcontrol() {

    bool isRunning = false;

    while (true) {
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)){
			catapult.move_relative(720, 30);
        }
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)){
            punch();
        }
        
        pros::delay(20);
    }
}
