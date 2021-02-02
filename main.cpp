#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>

#include "Chip.h"
#include "FaceDetector.h"
#include "MotorController.h"

static const double BOUNDARY_MARGIN = 0.1;

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

void motor_control() {
    Chip chip;
    MotorController controller0(&chip, 0);
    MotorController controller1(&chip, 1);

    std::cout << "Starting Motor Control..." << std::endl;
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
}

cv::Rect create_boundary(const cv::Mat &frame, double margin) {
    int rows = frame.rows;
    int cols = frame.cols;
    int row_offset = margin * rows;
    int col_offset = margin * cols;
    cv::Rect boundary(col_offset, row_offset, cols - (2 * col_offset), rows - (2 * row_offset));
    return boundary;
}

bool is_out_of_bounds(const cv::Rect &boundary, const cv::Rect &object) {
    return object.x < boundary.x || object.y < boundary.y || (object.x + object.width > boundary.x + boundary.width) || (object.y + object.height > boundary.y + boundary.height);
}

int face_detection() {
    cv::VideoCapture capture;
    cv::Mat frame;

    capture.open(0);
    capture.set(cv::CAP_PROP_FPS, 1);

    capture >> frame;
    const cv::Rect boundary = create_boundary(frame, BOUNDARY_MARGIN);

    FaceDetector face_detector;

    if (capture.isOpened()) {
        std::cout << "Starting video... " << std::endl;
        while (true) {
            capture >> frame;

            cv::Mat gray_frame;
            cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);

            std::vector<cv::Rect> faces = face_detector.process_frame(gray_frame);
            
            cv::Scalar boundary_colour = cv::Scalar(255, 0, 0);
            if (!faces.empty()) {
                int largest_area = 0;
                cv::Rect largest_object;

                for (auto &face : faces) {
                    int area = face.width * face.height;
                    if (area > largest_area) {
                        largest_area = area;
                        largest_object = face;
                    }
                }
                cv::rectangle(frame, largest_object, cv::Scalar(0, 255, 0));

                bool out_of_bounds = is_out_of_bounds(boundary, largest_object);
                if (out_of_bounds) {
                    boundary_colour = cv::Scalar(0, 0, 255);
                }
            }
            cv::rectangle(frame, boundary, boundary_colour);
            cv::imshow("Face Detection", frame);
            cv::waitKey(1); // Use 1ms because it gets limited by the webcam anyways
        }
    } else {
        std::cout << "Could not open camera" << std::endl;
    }
    return 0;
}

int main() {
    try {
        std::thread motor_control_th(motor_control);
        std::thread face_detection_th(face_detection);

        motor_control_th.join();
        face_detection_th.detach();
    } catch (const char *msg) {
        std::cerr << msg << std::endl;
        return -1;
    }
}
