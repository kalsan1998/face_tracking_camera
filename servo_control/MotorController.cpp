#include "MotorController.h"

#include <iostream>

#include "Chip.h"

MotorController::MotorController(Chip *chip, int channel) 
    : MotorController(chip, channel, 90) {}

MotorController::MotorController(Chip *chip, int channel, int init_angle) 
    : chip(chip), channel(channel) {
        if (!chip) {
            throw "MotorController: Received null chip";
        }
        if (channel < 0 || channel > 15) {
            throw "MotorController: Invalid channel";
        }
        start_reg = 6 + (4 * channel);
        stop_reg = start_reg + 2;

        set_angle(init_angle);
}

double MotorController::get_angle() const {
    return curr_angle;
}

void MotorController::set_angle(double angle) {
    // Write values to PWM registers
    // The chip has a PWM frequency of 200Hz (5ms per pulse).
    // The 4095 divides that 5ms into slices (1.2us per slice).
    //
    // The full range (0deg to 180deg -> 0.5ms to 2.5ms) gives us values of approximately 410 to
    // 2050.

    // Approximate values based on physically assessing things.
    static const int min = 300;
    static const int max = 2200;

    // Don't allow rotations near the min/max by this amount (just to be safe).
    static const int margin = 75;
    int stop_time = min + ((max - min) * angle / 180.0);

    std::cout << "Stop time: " << stop_time << std::endl;

    if (stop_time < min + margin || stop_time > max - margin) {
        throw "MotorController: Attempted to set angle out of range";
    }

    char buffer[3];
    int length = 3;

    buffer[0] = start_reg;
    buffer[1] = 0;
    buffer[2] = 0;
    chip->write_buffer(buffer, length);

    buffer[0] = stop_reg;
    buffer[1] = stop_time & 0xff; // Low bytes first
    buffer[2] = (stop_time >> 8) & 0xff;
    chip->write_buffer(buffer, length);

    curr_angle = angle;
}
