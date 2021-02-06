#ifndef __FACE_DETECTOR_H__
#define __FACE_DETECTOR_H__

#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/objdetect.hpp>

namespace cv {
    class VideoCapture;
};

class FaceDetector {
  public:
    FaceDetector();

    std::vector<cv::Rect> process_frame(cv::Mat &frame);

  private:
    cv::CascadeClassifier front_face_cc;
    cv::CascadeClassifier front_face_cc2;
    cv::CascadeClassifier side_face_cc;
};

#endif // __FACE_DETECTOR_H__
