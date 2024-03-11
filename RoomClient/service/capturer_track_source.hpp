#pragma once

#include <memory>
#include <vector>
#include "absl/memory/memory.h"
#include "api/scoped_refptr.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "pc/video_track_source.h"
#include "api/video/video_frame.h"
#include "api/video/video_source_interface.h"
#include "api/video/video_sink_interface.h"
#include "media/base/video_adapter.h"
#include "media/base/video_broadcaster.h"
#include "rtc_base/thread.h"

namespace vi {

	using namespace webrtc;

    class CapturerTrackSource : public webrtc::VideoTrackSource {
    public:
        CapturerTrackSource(bool isRemote) : webrtc::VideoTrackSource(isRemote) {}
        virtual int32_t startCapture(const std::string& deviceName) = 0;
        virtual int32_t stopCapture() = 0;
        virtual bool getCaptureing() = 0;
    };
}