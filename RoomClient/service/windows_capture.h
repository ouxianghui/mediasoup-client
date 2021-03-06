/**
 * This file is part of mediasoup_client project.
 * Author:    Jackie Ou
 * Created:   2021-11-01
 **/

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

	class SimpleVideoCapturer : public rtc::VideoSourceInterface<VideoFrame> {
	public:
		class FramePreprocessor {
		public:
			virtual ~FramePreprocessor() = default;

			virtual VideoFrame Preprocess(const VideoFrame& frame) = 0;
		};

		~SimpleVideoCapturer() override;

		void AddOrUpdateSink(rtc::VideoSinkInterface<VideoFrame>* sink, const rtc::VideoSinkWants& wants) override;
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

	class VcmCapturer : public SimpleVideoCapturer, public rtc::VideoSinkInterface<VideoFrame> {
	public:
		static VcmCapturer* Create(size_t width,
			size_t height,
			size_t target_fps,
			size_t capture_device_index, rtc::Thread* thread);
		virtual ~VcmCapturer();

		void OnFrame(const VideoFrame& frame) override;

		int32_t start();

		int32_t stop();

	private:
		VcmCapturer(rtc::Thread* thread);
		bool Init(size_t width,
			size_t height,
			size_t target_fps,
			size_t capture_device_index);
		void Destroy();

		rtc::scoped_refptr<webrtc::VideoCaptureModule> _createDevice(const char* uniqueID);

		int32_t _startCapture();

		int32_t _stopCapture();

		void _release();

	private:
		rtc::scoped_refptr<VideoCaptureModule> vcm_;
		VideoCaptureCapability capability_;
		rtc::Thread* thread_;
	};

    class WindowsCapturerTrackSource : public webrtc::VideoTrackSource {
    public:
        ~WindowsCapturerTrackSource() {}

        static rtc::scoped_refptr<WindowsCapturerTrackSource> Create(rtc::Thread* thread) {
            const size_t kWidth = 1280;
            const size_t kHeight = 720;
			const size_t kFps = 30;
			std::unique_ptr<VcmCapturer> capturer;
            std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(webrtc::VideoCaptureFactory::CreateDeviceInfo());
			if (!info) {
				return nullptr;
			}
			int num_devices = info->NumberOfDevices();
			for (int i = 0; i < num_devices; ++i) {
                capturer = absl::WrapUnique(VcmCapturer::Create(kWidth, kHeight, kFps, i, thread));
				if (capturer) {
                    return new rtc::RefCountedObject<WindowsCapturerTrackSource>(std::move(capturer));
				}
			}

			return nullptr;
		}

		int32_t startCapture() {
			if (!capturer_) {
				return -1;
			}
			return capturer_->start();
		}

		int32_t stopCapture() {
			if (!capturer_) {
				return -1;
			}
			return capturer_->stop();
		}

	protected:
        explicit WindowsCapturerTrackSource(
			std::unique_ptr<VcmCapturer> capturer)
			: VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}

	private:
		rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
			return capturer_.get();
		}
		std::unique_ptr<VcmCapturer> capturer_;

	};
}

