/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#define MSC_CLASS "MediaStreamTrackFactory"

#include "rtc_context.hpp"
#include <iostream>
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/create_peerconnection_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/media_stream_interface.h"
#include "api/task_queue/default_task_queue_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "modules/audio_device/include/audio_device.h"
#include "system_wrappers/include/clock.h"
#include "rtc_base/thread.h"
#include "logger/spd_logger.h"
#include "MediaSoupClientErrors.hpp"
//#include "capturer_factory.h"
//#include "rtsp_video_capturer.h"
#include "engine.h"

using namespace mediasoupclient;


namespace vi {

RTCContext::RTCContext()
{

}

RTCContext::~RTCContext()
{
    DLOG("~RTCContext()");
}

void RTCContext::init()
{
    if (_factory) {
        DLOG("already configured");
        return;
    }

    _networkThread = rtc::Thread::CreateWithSocketServer().release();
    _signalingThread = rtc::Thread::Create().release();
    _workerThread = rtc::Thread::Create().release();

    _networkThread->SetName("network_thread", nullptr);
    _signalingThread->SetName("signaling_thread", nullptr);
    _workerThread->SetName("worker_thread", nullptr);

    if (!_networkThread->Start() || !_signalingThread->Start() || !_workerThread->Start()) {
        ELOG("thread start errored");
    }

    _adm = _workerThread->Invoke<rtc::scoped_refptr<webrtc::AudioDeviceModule>>(RTC_FROM_HERE, [this]() {
        _taskQueueFactory = webrtc::CreateDefaultTaskQueueFactory();
        return webrtc::AudioDeviceModule::Create(webrtc::AudioDeviceModule::kPlatformDefaultAudio, _taskQueueFactory.get());
    });

    //_workerThread->Invoke<void>(RTC_FROM_HERE, [this]() {
    //    //_adm->InitPlayout();
    //    ELOG("_adm->InitPlayout() = {}", _adm->InitPlayout());
    //    ELOG("_adm->StartPlayout() = {}", _adm->StartPlayout());
    //});

    _videoDecoderFactory =  webrtc::CreateBuiltinVideoDecoderFactory();

    this->_factory = webrtc::CreatePeerConnectionFactory(this->_networkThread,
                                                         this->_workerThread,
                                                         this->_signalingThread,
                                                         _adm,
                                                         webrtc::CreateBuiltinAudioEncoderFactory(),
                                                         webrtc::CreateBuiltinAudioDecoderFactory(),
                                                         webrtc::CreateBuiltinVideoEncoderFactory(),
                                                         webrtc::CreateBuiltinVideoDecoderFactory(),
                                                         nullptr /*audio_mixer*/,
                                                         nullptr /*audio_processing*/);
}

void RTCContext::destroy()
{
    if (_factory) {
        _factory = nullptr;
    }

    _workerThread->Invoke<void>(RTC_FROM_HERE, [this]() {
        _adm = nullptr;
    });
}

rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> RTCContext::factory()
{
    return _factory;
}

rtc::Thread* RTCContext::networkThread()
{
    return _networkThread;
}

rtc::Thread* RTCContext::signalingThread()
{
    return _signalingThread;
}

rtc::Thread* RTCContext::workerThread()
{
    return _workerThread;
}

rtc::scoped_refptr<webrtc::AudioDeviceModule> RTCContext::adm()
{
    return _adm;
}

std::unique_ptr<webrtc::VideoDecoderFactory>& RTCContext::videoDecoderFactory()
{
    return _videoDecoderFactory;
}

}

//static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory;

///* MediaStreamTrack holds reference to the threads of the PeerConnectionFactory.
// * Use plain pointers in order to avoid threads being destructed before tracks.
// */
//static rtc::Thread* networkThread;
//static rtc::Thread* signalingThread;
//static rtc::Thread* workerThread;

//static std::unique_ptr<webrtc::VideoDecoderFactory> _videoDecoderFactory;

//static void createFactory()
//{
//    networkThread   = rtc::Thread::Create().release();
//    signalingThread = rtc::Thread::Create().release();
//    workerThread    = rtc::Thread::Create().release();

//    networkThread->SetName("network_thread", nullptr);
//    signalingThread->SetName("signaling_thread", nullptr);
//    workerThread->SetName("worker_thread", nullptr);

//    if (!networkThread->Start() || !signalingThread->Start() || !workerThread->Start())
//    {
//        MSC_THROW_INVALID_STATE_ERROR("thread start errored");
//    }

//    webrtc::PeerConnectionInterface::RTCConfiguration config;

//    auto audioCaptureModule = FakeAudioCaptureModule::Create();
//    if (!audioCaptureModule)
//    {
//        MSC_THROW_INVALID_STATE_ERROR("audio capture module creation errored");
//    }

//    factory = webrtc::CreatePeerConnectionFactory(
//                networkThread,
//                workerThread,
//                signalingThread,
//                audioCaptureModule,
//                webrtc::CreateBuiltinAudioEncoderFactory(),
//                webrtc::CreateBuiltinAudioDecoderFactory(),
//                webrtc::CreateBuiltinVideoEncoderFactory(),
//                webrtc::CreateBuiltinVideoDecoderFactory(),
//                nullptr /*audio_mixer*/,
//                nullptr /*audio_processing*/);

//    if (!factory)
//    {
//        MSC_THROW_ERROR("error ocurred creating peerconnection factory");
//    }

//    _videoDecoderFactory =  webrtc::CreateBuiltinVideoDecoderFactory();
//}

// Audio track creation.
rtc::scoped_refptr<webrtc::AudioTrackInterface> createAudioTrack(const std::string& label)
{
    auto context = getEngine()->getRTCContext();
    if (!context) {
        ELOG("RTCContext is null");
        return nullptr;
    }
    auto factory = context->factory();
    if (!factory) {
        ELOG("PeerConnectionFactory is null");
        return nullptr;
    }

    cricket::AudioOptions options;
    options.highpass_filter = false;

    rtc::scoped_refptr<webrtc::AudioSourceInterface> source = factory->CreateAudioSource(options);

    return factory->CreateAudioTrack(label, source.release());
}

// Video track creation.
rtc::scoped_refptr<webrtc::VideoTrackInterface> createVideoTrack(const std::string& videoUrl,
                                                                 const std::map<std::string,std::string>& opts,
                                                                 const std::regex& publishFilter)
{
    auto context = getEngine()->getRTCContext();
    if (!context) {
        ELOG("RTCContext is null");
        return nullptr;
    }

    auto pcf = context->factory();
    if (!pcf) {
        ELOG("pcf is null");
        return nullptr;
    }

    auto& vdf = context->videoDecoderFactory();
    if (!pcf) {
        ELOG("VideoDecoderFactory is null");
        return nullptr;
    }

    return nullptr;

    //auto videoTrackSource = CapturerFactory::CreateVideoSource(videoUrl, opts, publishFilter, pcf, vdf);

    //return pcf->CreateVideoTrack(rtc::CreateRandomUuid(), videoTrackSource);
}
