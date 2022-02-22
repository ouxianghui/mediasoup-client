#include "mac_capturer.h"

#import "sdk/objc/base/RTCVideoCapturer.h"
#import "sdk/objc/components/capturer/RTCCameraVideoCapturer.h"
#import "sdk/objc/native/api/video_capturer.h"
#import "sdk/objc/native/src/objc_frame_buffer.h"
#include "rtc_base/async_invoker.h"
#include "logger/u_logger.h"
#include "utils/thread_provider.h"
#import "helpers/RTCDispatcher+Private.h"

@interface MacVideoSourceAdapter : NSObject <RTC_OBJC_TYPE (RTCVideoCapturerDelegate)>
  @property(nonatomic) vi::MacCapturer *capturer;
@end

@implementation MacVideoSourceAdapter
@synthesize capturer = _capturer;

- (void)capturer:(RTC_OBJC_TYPE(RTCVideoCapturer) *)capturer
  didCaptureVideoFrame:(RTC_OBJC_TYPE(RTCVideoFrame) *)frame {
    const int64_t timestamp_us = frame.timeStampNs / rtc::kNumNanosecsPerMicrosec;
    rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer =
            new rtc::RefCountedObject<webrtc::ObjCFrameBuffer>(frame.buffer);
    _capturer->OnFrame(webrtc::VideoFrame::Builder()
                       .set_video_frame_buffer(buffer)
                       .set_rotation(webrtc::kVideoRotation_0)
                       .set_timestamp_us(timestamp_us)
                       .build());
}

@end

namespace {

AVCaptureDeviceFormat *SelectClosestFormat(AVCaptureDevice *device, size_t width, size_t height) {
    NSArray<AVCaptureDeviceFormat *> *formats =
            [RTC_OBJC_TYPE(RTCCameraVideoCapturer) supportedFormatsForDevice:device];
    AVCaptureDeviceFormat *selectedFormat = nil;
    int currentDiff = INT_MAX;
    for (AVCaptureDeviceFormat *format in formats) {
        CMVideoDimensions dimension = CMVideoFormatDescriptionGetDimensions(format.formatDescription);
        int diff =
                std::abs((int64_t)width - dimension.width) + std::abs((int64_t)height - dimension.height);
        if (diff < currentDiff) {
            selectedFormat = format;
            currentDiff = diff;
        }
    }
    return selectedFormat;
}

}  // namespace

namespace vi {

using namespace webrtc;

MacCapturer::MacCapturer(size_t width,
                         size_t height,
                         size_t target_fps,
                         size_t capture_device_index) {
    MacVideoSourceAdapter *adapter = [[MacVideoSourceAdapter alloc] init];
    adapter_ = (__bridge_retained void *)adapter;
    adapter.capturer = this;

    RTC_OBJC_TYPE(RTCCameraVideoCapturer) *capturer =
            [[RTC_OBJC_TYPE(RTCCameraVideoCapturer) alloc] initWithDelegate:adapter];
    capturer_ = (__bridge_retained void *)capturer;

    AVCaptureDevice *device =
            [[RTC_OBJC_TYPE(RTCCameraVideoCapturer) captureDevices] objectAtIndex:capture_device_index];
    AVCaptureDeviceFormat *format = SelectClosestFormat(device, width, height);
    [capturer startCaptureWithDevice:device format:format fps:target_fps];
}

MacCapturer *MacCapturer::Create(size_t width,
                                 size_t height,
                                 size_t target_fps,
                                 size_t capture_device_index) {
    return new MacCapturer(width, height, target_fps, capture_device_index);
}

void MacCapturer::Destroy() {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    MacVideoSourceAdapter *adapter = (__bridge_transfer MacVideoSourceAdapter *)adapter_;
    RTC_OBJC_TYPE(RTCCameraVideoCapturer) *capturer =
            (__bridge_transfer RTC_OBJC_TYPE(RTCCameraVideoCapturer) *)capturer_;
    [capturer stopCapture];
#pragma clang diagnostic pop
}

MacCapturer::~MacCapturer() {
    Destroy();
    DLOG("~MacCapturer()");
}

void MacCapturer::OnFrame(const VideoFrame &frame) {
    BaseVideoCapturer::OnFrame(frame);
}

}
