#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>

#include "FaceDetector.h"

static const double BOUNDARY_MARGIN = 0.1;

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

int main() {
    cv::VideoCapture capture;
    cv::Mat frame;

    capture.open(0);
    capture.set(cv::CAP_PROP_FPS, 3);

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
            
            // TODO: Track largest face

            bool out_of_bounds = false;
                for (auto &face : faces) {
                cv::rectangle(frame, face, cv::Scalar(0, 255, 0));
                out_of_bounds |= is_out_of_bounds(boundary, face);
            }

            cv::Scalar boundary_colour = out_of_bounds ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 0, 0);
            cv::rectangle(frame, boundary, boundary_colour);
            cv::imshow("Face Detection", frame);
            cv::waitKey(1); // Use 1ms because it gets limited by the webcam anyways
        }
    } else {
        std::cout << "Could not open camera" << std::endl;
    }
    return 0;
}
