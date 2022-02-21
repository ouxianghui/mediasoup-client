#pragma once

#ifdef WEBRTC_MAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include "api/video/video_sink_interface.h"
#include "api/video/video_frame.h"

namespace vi {

using namespace webrtc;

class GlRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
public:
    void OnFrame(const webrtc::VideoFrame& frame) override;

protected:
    GlRenderer();

    void init();

    void destroy();

    void resizeViewport(size_t width, size_t height);

private:
    bool is_init_;
    uint8_t* buffer_;
    GLuint texture_;
    size_t width_, height_, buffer_size_;

    void resizeVideo(size_t width, size_t height);
};

}
