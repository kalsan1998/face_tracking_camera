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
#include "VideoOutput.h"
 
#ifdef DEBUG
#define DEBUG_SNAPSHOT(filename, frame) \
    cv::imwrite(filename, frame);
#else
#define DEBUG_SNAPSHOT(filename, frame)
#endif


static const double BOUNDARY_MARGIN = 0.33;

static const double ANGLE_INCREMENT = 10;

#ifdef TEST
#include <opencv2/opencv.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#define VIDEO_OUT  "/dev/video3"
#endif

void curve_motion(MotorController &controller, double angle) {
    std::cout << "Moving motor" << std::endl;
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

void set_relative_angle(MotorController &controller, double relative_angle) {
    curve_motion(controller, controller.get_angle() + relative_angle);
}

void manual_motor_control(MotorController *horz_controller, MotorController *vert_controller) {
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
                curve_motion(*horz_controller, val);
            } else if (channel == 1) {
                curve_motion(*vert_controller, val);
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

int face_following(MotorController *horz_controller, MotorController *vert_controller) {
    try {
        cv::Mat init_frame;

        // TODO: create a frame buffer in another thread ???
        cv::VideoCapture capture;
        capture.open(0);
        if (capture.isOpened()) {
            capture.set(cv::CAP_PROP_FPS, 1);
            // capture.set(cv::CAP_PROP_BUFFERSIZE, 0);
            capture >> init_frame;

            VideoOutput video_out(init_frame.cols, init_frame.rows);

            const cv::Rect boundary = create_boundary(init_frame, BOUNDARY_MARGIN);
            
            const int max_height = init_frame.rows * 0.75;
            const int max_width = init_frame.cols * 0.75;

            FaceDetector face_detector;
            long long counter = 0;

            std::cout << "Starting video... " << std::endl;
            while (true) {
                cv::Mat frame;
                capture >> frame;
                
                cv::Mat gray_frame;
                cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);

                std::vector<cv::Rect> faces = face_detector.process_frame(gray_frame);
                
                cv::Scalar boundary_colour = cv::Scalar(255, 0, 0);
                if (!faces.empty()) {
                    int largest_area = 0;
                    cv::Rect largest_obj;

                    for (auto &face : faces) {
                        int area = face.width * face.height;
                        if (area > largest_area && face.width < max_width && face.height < max_height) {
                            largest_area = area;
                            largest_obj = face;
                        }
                    }
                    cv::rectangle(frame, largest_obj, cv::Scalar(0, 255, 0));
                    int center_x = largest_obj.x + (largest_obj.width / 2);
                    int center_y = largest_obj.y + (largest_obj.height / 2);

                    bool top_oob = center_y > boundary.y + boundary.height;
                    bool bot_oob = center_y < boundary.y;
                    bool left_oob = center_x < boundary.x;
                    bool right_oob = center_x > boundary.x + boundary.width;
                    if (top_oob || bot_oob || left_oob || right_oob) {
                        if (top_oob) {
                            DEBUG_SNAPSHOT("snapshots/" + std::to_string(counter++) + "top.jpg", frame);
                            set_relative_angle(*vert_controller, ANGLE_INCREMENT);
                        } else if (bot_oob) {
                            DEBUG_SNAPSHOT("snapshots/" + std::to_string(counter++) + "bot.jpg", frame);
                            set_relative_angle(*vert_controller, -1 * ANGLE_INCREMENT);
                        }
                        if (left_oob) {
                            DEBUG_SNAPSHOT("snapshots/" + std::to_string(counter++) + "left.jpg", frame);
                            set_relative_angle(*horz_controller, ANGLE_INCREMENT);
                        } else if (right_oob) {
                            DEBUG_SNAPSHOT("snapshots/" + std::to_string(counter++) + "right.jpg", frame);
                            set_relative_angle(*horz_controller, -1 * ANGLE_INCREMENT);
                        }
                        boundary_colour = cv::Scalar(0, 0, 255);
                    }
                }

                cv::Mat rgb_frame;
                cv::cvtColor(frame, rgb_frame, cv::COLOR_BGR2RGB);
                video_out.write_video(rgb_frame);
                //cv::rectangle(frame, boundary, boundary_colour);
                //cv::imshow("Face Detection", frame);
                //cv::waitKey(1); // Use 1ms because it gets limited by the webcam anyways
            }
        } else {
            std::cout << "Could not open camera" << std::endl;
        }
    } catch (const char *msg) {
        std::cerr << msg << std::endl;
    }
    return 0;
}

int main() {
    try {
        Chip chip;
        MotorController horz_controller(&chip, 0);
        MotorController vert_controller(&chip, 1);

        //std::thread motor_control_th(manual_motor_control, &horz_controller, &vert_controller);
        std::thread face_detection_th(face_following, &horz_controller, &vert_controller);

        //motor_control_th.join();
        face_detection_th.join();
    } catch (const char *msg) {
        std::cerr << msg << std::endl;
        return -1;
    }
}
