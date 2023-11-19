/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#pragma once

#include <memory>
#include <vector>

#include "api/media_stream_interface.h"
#include "api/scoped_refptr.h"
#include "pc/video_track_source.h"
#include "modules/video_capture/video_capture_factory.h"
#include "modules/video_capture/video_capture.h"
#include "rtc_base/thread.h"
#include "service/base_video_capturer.h"
#include "logger/spd_logger.h"

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

    void start();

    void stop();

private:
    MacCapturer(size_t width,
                size_t height,
                size_t target_fps,
                size_t capture_device_index);
    void Destroy();

    size_t width_;
    size_t height_;
    size_t target_fps_;
    size_t capture_device_index_;

    void* capturer_;
    void* adapter_;
};

class MacTrackSource : public webrtc::VideoTrackSource {
public:
    MacTrackSource(std::unique_ptr<MacCapturer> video_capturer, bool is_screencast)
        : VideoTrackSource(/*remote=*/false),
          video_capturer_(std::move(video_capturer)),
          is_screencast_(is_screencast) {}

    ~MacTrackSource() { DLOG("~MacTrackSource()"); }

    void start() {
        video_capturer_->start();
        SetState(kLive);
    }

    void stop() {
        video_capturer_->stop();
        SetState(kMuted);
    }

    bool is_screencast() const override { return is_screencast_; }

protected:
    rtc::VideoSourceInterface<VideoFrame>* source() override {
        return video_capturer_.get();
    }

private:
    std::unique_ptr<MacCapturer> video_capturer_;
    const bool is_screencast_;
};


}
