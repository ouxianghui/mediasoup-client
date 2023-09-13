#pragma once

#include <regex>
#include <map>
#include "api/scoped_refptr.h"

namespace rtc {
    class Thread;
}

namespace webrtc {
    class PeerConnectionFactoryInterface;
    class MediaStreamTrackInterface;
    class TaskQueueFactory;
    class AudioDeviceModule;
    class VideoDecoderFactory;
    class AudioTrackInterface;
    class VideoTrackInterface;
}

namespace vi {

class RTCContext {

public:
    RTCContext();

    ~RTCContext();

    void init();

    void destroy();

    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory();

    rtc::Thread* networkThread();

    rtc::Thread* signalingThread();

    rtc::Thread* workerThread();

    rtc::scoped_refptr<webrtc::AudioDeviceModule> adm();

    std::unique_ptr<webrtc::VideoDecoderFactory>& videoDecoderFactory();

private:
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _factory;

    /* MediaStreamTrack holds reference to the threads of the PeerConnectionFactory.
     * Use plain pointers in order to avoid threads being destructed before tracks.
     */
    rtc::Thread* _networkThread;

    rtc::Thread* _signalingThread;

    rtc::Thread* _workerThread;

    std::unique_ptr<webrtc::TaskQueueFactory> _taskQueueFactory;

    rtc::scoped_refptr<webrtc::AudioDeviceModule> _adm;

    std::unique_ptr<webrtc::VideoDecoderFactory> _videoDecoderFactory;
};

}

rtc::scoped_refptr<webrtc::AudioTrackInterface> createAudioTrack(const std::string& label);

rtc::scoped_refptr<webrtc::VideoTrackInterface> createVideoTrack(const std::string& videoUrl,
                                                                 const std::map<std::string,std::string>& opts,
                                                                 const std::regex& publishFilter);
