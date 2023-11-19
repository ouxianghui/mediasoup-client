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
#include "capturer_track_source.hpp"
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

		void OnFrame(const VideoFrame& frame);

	protected:
		
		rtc::VideoSinkWants GetSinkWants();

	private:
		void UpdateVideoAdapter();
		VideoFrame MaybePreprocess(const VideoFrame& frame);

		Mutex lock_;
		std::unique_ptr<FramePreprocessor> preprocessor_ RTC_GUARDED_BY(lock_);
		rtc::VideoBroadcaster broadcaster_;
		cricket::VideoAdapter video_adapter_;
	};

	class VcmCapturer {
	public:
		static VcmCapturer* Create(size_t width,
			size_t height,
			size_t target_fps,
			size_t capture_device_index, rtc::VideoSinkInterface<VideoFrame>* sink, rtc::Thread* thread);
		virtual ~VcmCapturer();

		int32_t start();

		int32_t stop();

	private:
		VcmCapturer(rtc::Thread* thread);
		bool Init(size_t width,
			size_t height,
			size_t target_fps,
			size_t capture_device_index,  rtc::VideoSinkInterface<VideoFrame>* sink);
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

    class WindowsCapturerTrackSource : public CapturerTrackSource,  public rtc::VideoSinkInterface<VideoFrame> {
    public:
        ~WindowsCapturerTrackSource() {}

        static rtc::scoped_refptr<WindowsCapturerTrackSource> Create(rtc::Thread* thread, const std::string& deviceId) {
            
            std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(webrtc::VideoCaptureFactory::CreateDeviceInfo());
			if (!info) {
				return nullptr;
			}
			int num_devices = info->NumberOfDevices();
			if (num_devices == 0) {
				return nullptr;
			}
			auto countref_ptr = new rtc::RefCountedObject<WindowsCapturerTrackSource>(thread, deviceId);
			return rtc::scoped_refptr<WindowsCapturerTrackSource>(countref_ptr);
		}

		std::shared_ptr< VcmCapturer> createCapturer(rtc::Thread* thread, const std::string& deviceId) {
			const size_t kWidth = 1280;
			const size_t kHeight = 720;
			const size_t kFps = 30;

			std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(webrtc::VideoCaptureFactory::CreateDeviceInfo());
			if (!info) {
				return nullptr;
			}
			std::shared_ptr< VcmCapturer> ret = nullptr;
			int num_devices = info->NumberOfDevices();
			for (int i = 0; i < num_devices; ++i) {
				char szDeviceName[255] = {};
				char szDeviceId[255] = {};
				info->GetDeviceName(i, szDeviceName, sizeof(szDeviceName), szDeviceId, sizeof(szDeviceId));

				if (deviceId == szDeviceId) {
					ret = std::shared_ptr< VcmCapturer>(VcmCapturer::Create(kWidth, kHeight, kFps, i, this, thread));
				}
			}

			if (!ret && num_devices > 0) {
				ret = std::shared_ptr< VcmCapturer>(VcmCapturer::Create(kWidth, kHeight, kFps, 0, this, thread));
			}
			return ret;
		}

		int32_t startCapture(const std::string& deviceId) override {
			if (capturer_) {
				if (deviceId != m_deviceId) {
					capturer_->stop();
					capturer_ = nullptr;
				}
			}
			
			m_deviceId = deviceId; 
			if (!capturer_) {
				capturer_ = createCapturer(m_thread, deviceId);
			}

			
			if (!capturer_) {
				return -1;
			}

			if(!_videoCapturer) {
				_videoCapturer = std::make_shared<SimpleVideoCapturer>();
			}
			m_captureing = true;
			return capturer_->start();
		}

		int32_t stopCapture() override {
			m_captureing = false;
			if (!capturer_) {
				return -1;
			}
			auto ret = capturer_->stop();
			capturer_ = nullptr;
			return ret;
		}

		bool getCaptureing() override {
			return m_captureing;
		}

		void OnFrame(const VideoFrame& frame) {
			_videoCapturer->OnFrame(frame);
		}

		rtc::VideoSourceInterface<webrtc::VideoFrame> *source() override {
			return _videoCapturer.get();
		}

	protected:
        explicit WindowsCapturerTrackSource(rtc::Thread* thread, const std::string& deviceId) : CapturerTrackSource(false),
			m_deviceId(deviceId),
			m_thread(thread){
			
		}

	private:
		std::shared_ptr<SimpleVideoCapturer> _videoCapturer;
		std::shared_ptr<VcmCapturer> capturer_;
		bool m_captureing = false;
		rtc::Thread* m_thread = nullptr;
		std::string m_deviceId;

	};
}

