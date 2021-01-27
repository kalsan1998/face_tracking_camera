#ifndef __MOTOR_CONTROLLER_H__
#define __MOTOR_CONTROLLER_H__

class Chip;

class MotorController {
  public:
    MotorController(Chip *chip, int channel);
    MotorController(Chip *chip, int channel, int init_angle);
    void set_angle(int angle);

  private:

    Chip *chip;
    const int channel;
    char start_reg;
    char stop_reg;
};

#endif // __MOTOR_CONTROLLER_H__

