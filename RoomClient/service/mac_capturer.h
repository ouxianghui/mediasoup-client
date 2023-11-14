#pragma once

#include <memory>
#include <vector>

#include "api/media_stream_interface.h"
#include "api/scoped_refptr.h"
#include "pc/video_track_source.h"
#include "modules/video_capture/video_capture_factory.h"
#include "modules/video_capture/video_capture.h"
#include "rtc_base/thread.h"
#include "base_video_capturer.h"
#include "logger/u_logger.h"

namespace vi {
using namespace webrtc;

class MacCapturer : public BaseVideoCapturer,
                    public rtc::VideoSinkInterface<VideoFrame> {
public:
    static MacCapturer* Create(size_t width,
                               size_t height,
                               size_t target_fps,
                               size_t capture_device_index);
    ~MacCapturer() override;

    void OnFrame(const VideoFrame& frame) override;

private:
    MacCapturer(size_t width,
                size_t height,
                size_t target_fps,
                size_t capture_device_index);
    void Destroy();

    void* capturer_;
    void* adapter_;
};

class MacTrackSource : public webrtc::VideoTrackSource {
public:
    MacTrackSource(std::unique_ptr<BaseVideoCapturer> video_capturer, bool is_screencast)
        : VideoTrackSource(/*remote=*/false),
          video_capturer_(std::move(video_capturer)),
          is_screencast_(is_screencast) {}

    ~MacTrackSource() { DLOG("~MacTrackSource()"); }

    void Start() { SetState(kLive); }

    void Stop() { SetState(kMuted); }

    bool is_screencast() const override { return is_screencast_; }

protected:
    rtc::VideoSourceInterface<VideoFrame>* source() override {
        return video_capturer_.get();
    }

private:
    std::unique_ptr<BaseVideoCapturer> video_capturer_;
    const bool is_screencast_;
};


}
