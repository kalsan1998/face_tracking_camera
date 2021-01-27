#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>

static const std::string FRONT_FACE_CLASSIFIER_DATA = "./classifiers/frontal_face.xml";

static const std::string SIDE_FACE_CLASSIFIER_DATA = "./classifiers/side_face.xml";

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

    cv::CascadeClassifier front_face_cascade, side_face_cascade;
    front_face_cascade.load(FRONT_FACE_CLASSIFIER_DATA);
    side_face_cascade.load(SIDE_FACE_CLASSIFIER_DATA);
    capture.open(0);

    capture >> frame;
    static const double boundary_margin = 0.1;
    const cv::Rect boundary = create_boundary(frame, boundary_margin);

    const int y_axis = frame.cols / 2;

    if (capture.isOpened()) {
        std::cout << "Starting video... " << std::endl;
        while (true) {
            capture >> frame;
            if (frame.empty()) {
                std::cout << "Detected empty frame" << std::endl;
                break;
            }
            
            // TODO: Track largest face

            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

            std::vector<cv::Rect> faces;
            front_face_cascade.detectMultiScale(gray, faces);
            bool out_of_bounds = false;
            for (auto &face : faces) {
                cv::rectangle(frame, face, cv::Scalar(0, 255, 0));
                out_of_bounds |= is_out_of_bounds(boundary, face);
            }
            if (faces.empty()) {
                side_face_cascade.detectMultiScale(gray, faces);
                for (auto &face : faces) {
                    cv::rectangle(frame, face, cv::Scalar(0, 255, 0));
                    out_of_bounds |= is_out_of_bounds(boundary, face);
                }
            }
            if (faces.empty()) {
                cv::Mat flipped_frame;
                cv::flip(gray, flipped_frame, 1);
                side_face_cascade.detectMultiScale(flipped_frame, faces);
                for (auto &face : faces) {
                    if (face.x >= y_axis) {
                        face.x = frame.cols - face.x;
                    } else {
                        face.x = frame.cols - face.x - face.width;
                    }
                    cv::rectangle(frame, face, cv::Scalar(0, 255, 0));
                    out_of_bounds |= is_out_of_bounds(boundary, face);
                }
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
