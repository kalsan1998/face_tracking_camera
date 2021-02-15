#include "FaceDetector.h"

static const std::string FRONT_FACE_CLASSIFIER_DATA = "classifiers/haarcascade_frontalface_alt.xml";
static const std::string FRONT_FACE_CLASSIFIER_DATA2 ="classifiers/haarcascade_frontalface_alt2.xml";
static const std::string SIDE_FACE_CLASSIFIER_DATA = "classifiers/side_face.xml";

FaceDetector::FaceDetector() {
    front_face_cc.load(FRONT_FACE_CLASSIFIER_DATA);
    front_face_cc2.load(FRONT_FACE_CLASSIFIER_DATA2);
    side_face_cc.load(SIDE_FACE_CLASSIFIER_DATA);
}

static void get_objects_from_frame(cv::CascadeClassifier &cc, std::vector<cv::Rect> &objs,
                                   cv::Mat &frame) {
    std::vector<cv::Rect> objs_tmp;
    cc.detectMultiScale(frame, objs_tmp);
    std::copy(objs_tmp.begin(), objs_tmp.end(), std::back_inserter(objs));
}

std::vector<cv::Rect> FaceDetector::process_frame(cv::Mat &frame) {
    if (frame.empty()) {
        throw "Empty frame";
    }

    std::vector<cv::Rect> faces;
    get_objects_from_frame(front_face_cc, faces, frame);
    get_objects_from_frame(front_face_cc2, faces, frame);
    get_objects_from_frame(side_face_cc, faces, frame);

    std::vector<cv::Rect> faces_flipped;
    cv::Mat flipped_frame;
    cv::flip(frame, flipped_frame, 1);
    get_objects_from_frame(side_face_cc, faces_flipped, frame);
    for (auto &face : faces_flipped) {
        if (face.x >= frame.cols / 2) {
            face.x = frame.cols - face.x;
        } else {
            face.x = frame.cols - face.x - face.width;
        }
    }
    std::copy(faces_flipped.begin(), faces_flipped.end(), std::back_inserter(faces));
    return faces;
}
