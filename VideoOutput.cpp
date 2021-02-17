#include "VideoOutput.h"

#include <opencv2/core/mat.hpp>

#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>

static const char *VIDEO_OUT = "/dev/video3";

VideoOutput::VideoOutput(int width, int height) {
    output = open(VIDEO_OUT, O_RDWR);
    if (output < 0) {
        throw "ERROR: could not open output device";
    }

    v4l2_format vid_format;
    memset(&vid_format, 0, sizeof(vid_format));
    vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

    if (ioctl(output, VIDIOC_G_FMT, &vid_format) < 0) {
        throw "ERROR: unable to get video format";
    }

    frame_sz = width * height * 3; // Times 3 because RGB
    vid_format.fmt.pix.width = width;
    vid_format.fmt.pix.height = height;
    vid_format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    vid_format.fmt.pix.sizeimage = frame_sz;
    vid_format.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(output, VIDIOC_S_FMT, &vid_format) < 0) {
        throw "ERROR: unable to set video fromat";
    }
}

VideoOutput::~VideoOutput() {
    close(output);
}

void VideoOutput::write_video(const cv::Mat &frame) {
    size_t written = write(output, frame.data, frame_sz);
    if (written < 0) {
        throw "ERROR: could not write tot output device";
    }
}

