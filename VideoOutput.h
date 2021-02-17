#ifndef __VIDEO_OUTPUT_H__
#define __VIDEO_OUTPUT_H__

#include <unistd.h>

namespace cv {
    class Mat;
}

class VideoOutput {
  public:
    VideoOutput(int width, int height);
    ~VideoOutput();
    void write_video(const cv::Mat &frame);
  private:
    size_t frame_sz;
    int output;

};

#endif // __VIDEO_OUTPUT_H__
