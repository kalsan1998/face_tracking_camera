#ifndef __MOTOR_CONTROLLER_H__
#define __MOTOR_CONTROLLER_H__

class Chip;

class MotorController {
  public:
    MotorController(Chip *chip, int channel);
    MotorController(Chip *chip, int channel, int init_angle);

    double get_angle() const;
    void set_angle(double angle);

  private:

    Chip *chip;
    const int channel;
    char start_reg;
    char stop_reg;
    double curr_angle;
};

#endif // __MOTOR_CONTROLLER_H__

