#include "Chip.h"

#include <fcntl.h>  // required for I2C device configuration
#include <linux/i2c-dev.h> // required for constant definitions
#include <sys/ioctl.h> // required for I2C device usage
#include <unistd.h>

static const int PCA9685_ADDR = 0x40; 
static const char *I2C_FILENAME = "/dev/i2c-1";
Chip::Chip() {
    init();
}

Chip::~Chip() {
    close(i2c_fd);
}

void Chip::init() {
    // We set the mode of the chip (using register 0) to allow PWM
    // In PWM mode, we set the times using LEDn_ON and LEDn_OFF registers (n is the channel). Each 
    // register holds values between 0 and 4095. The ON time is the time the output is asserted
    // and the OFF time is the time when the output is negated.

    i2c_fd = open(I2C_FILENAME, O_RDWR);
    if (i2c_fd < 0) {
        throw "Chip: Failed to open i2c file";
    }

    ioctl(i2c_fd, I2C_SLAVE, PCA9685_ADDR); // Set the I2C address for upcoming transactions

    char buffer[2];
    int length = 2;

    // Enable multi-byte writing; we want to write 2 bytes at a time when setting PWM values
    buffer[0] = 0;
    buffer[1] = 0x20;
    write_buffer(buffer, length);

    // Enable Prescale change
    buffer[0] = 0;
    buffer[1] = 0x10;
    //write_buffer(buffer, length);

    // Update Prescale register to set feqruency to 50Hz
    buffer[0] = 0xfe;
    buffer[1] = 0x1e;
    write_buffer(buffer, length);

    // Enable multi-byte writing; we want to write 2 bytes at a time when setting PWM values
    buffer[0] = 0;
    buffer[1] = 0x20;
    //write_buffer(buffer, length);
}

void Chip::write_buffer(char *buffer, int length) {
    write(i2c_fd, buffer, length);
}


