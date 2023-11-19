/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#pragma once

#include <stddef.h>

#include <memory>

#include "api/video/video_frame.h"
#include "api/video/video_source_interface.h"
#include "media/base/video_adapter.h"
#include "media/base/video_broadcaster.h"
#include "rtc_base/synchronization/mutex.h"

namespace vi {

using namespace webrtc;

class BaseVideoCapturer : public rtc::VideoSourceInterface<VideoFrame> {
public:
    class FramePreprocessor {
    public:
        virtual ~FramePreprocessor() = default;

        virtual VideoFrame Preprocess(const VideoFrame& frame) = 0;
    };

    ~BaseVideoCapturer() override;

    void AddOrUpdateSink(rtc::VideoSinkInterface<VideoFrame>* sink,
                         const rtc::VideoSinkWants& wants) override;
    void RemoveSink(rtc::VideoSinkInterface<VideoFrame>* sink) override;
    void SetFramePreprocessor(std::unique_ptr<FramePreprocessor> preprocessor) {
        MutexLock lock(&lock_);
        preprocessor_ = std::move(preprocessor);
    }

protected:
    void OnFrame(const VideoFrame& frame);
    rtc::VideoSinkWants GetSinkWants();

private:
    void UpdateVideoAdapter();
    VideoFrame MaybePreprocess(const VideoFrame& frame);

    Mutex lock_;
    std::unique_ptr<FramePreprocessor> preprocessor_ RTC_GUARDED_BY(lock_);
    rtc::VideoBroadcaster broadcaster_;
    cricket::VideoAdapter video_adapter_;
};

}  // namespace vi

