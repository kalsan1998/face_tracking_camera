#include <exception>
#include <iostream>

#include "Chip.h"
#include "MotorController.h"

int main() {
    try {
        Chip chip;
        MotorController controller(&chip, 0);

        std::cout << "Starting..." << std::endl;
        while (true) {
            std::string input;
            std::cin >> input;
            if (input == "q") {
                std::cout << "Exiting..." << std::endl;
                break;
            }
            try {
                int val = std::stoi(input, nullptr);
                controller.set_angle(val);
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
