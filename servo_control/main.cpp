#include <exception>
#include <iostream>
#include <unistd.h>

#include "Chip.h"
#include "MotorController.h"

void curve_motion(MotorController &controller, double angle) {
    static const double step_size = 0.5; 
    double curr_angle = controller.get_angle();
    double steps_needed = (angle - curr_angle) / step_size;
    int direction = steps_needed < 0 ? -1 : 1;
    steps_needed *= direction;
    double adjusted_step = direction * step_size;
    for (double step = 0; step < steps_needed; ++step) {
        curr_angle += adjusted_step;
        controller.set_angle(curr_angle);
        usleep(10000);
    }
    controller.set_angle(angle);
} 

int main() {
    try {
        Chip chip;
        MotorController controller0(&chip, 0);
        MotorController controller1(&chip, 1);

        std::cout << "Starting..." << std::endl;
        while (true) {
            int channel = -1;
            std::cin >> channel;
            if (channel == -1) {
                std::cout << "Exiting..." << std::endl;
                break;
            }
            int val;
            std::cin >> val;
            try {
                if (channel == 0) {
                    curve_motion(controller0, val);
                } else if (channel == 1) {
                    curve_motion(controller1, val);
                }
            } catch (const char *msg) {
                std::cout << msg << std::endl;
            } catch (const std::exception &exc) {
                std::cout << "Error: " << exc.what() << std::endl;
            }
        }
    } catch (const char *msg) {
        std::cerr << msg << std::endl;
        return -1;
    }
}
