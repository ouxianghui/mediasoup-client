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
#include "i_participant_controller.h"
#include "i_media_event_handler.h"
#include "utils/universal_observable.hpp"
#include "i_signaling_event_handler.h"
#include "i_media_event_handler.h"

namespace rtc {
    class Thread;
}

namespace webrtc {
    class MediaStreamTrackInterface;
}

namespace vi {

    class IMediaController;
    class Participant;

    class ParticipantController :
            public IParticipantController,
            public ISignalingEventHandler,
            public IMediaEventHandler,
            public UniversalObservable<IParticipantEventHandler>,
            public std::enable_shared_from_this<ParticipantController>
    {
    public:
        ParticipantController(rtc::Thread* mediasoupThread, std::shared_ptr<IMediaController> mediaController);

        ~ParticipantController();

        void init();

        void destroy();

        void addObserver(std::shared_ptr<IParticipantEventHandler> observer, rtc::Thread* callbackThread) override;

        void removeObserver(std::shared_ptr<IParticipantEventHandler> observer) override;

        std::shared_ptr<IParticipant> getParticipant(const std::string& pid) override;

        std::unordered_map<std::string, std::shared_ptr<IParticipant>> getParticipants() override;

        void muteAudio(const std::string& pid, bool muted) override;

        bool isAudioMuted(const std::string& pid) override;

        void muteVideo(const std::string& pid, bool muted) override;

        bool isVideoMuted(const std::string& pid) override;

        void createParticipant(const std::string& pid, const std::string& displayName);

    protected:
        // ISignalingEventHandler
        void onOpened() override {}

        void onClosed() override {}

        // Request from SFU
        void onNewConsumer(std::shared_ptr<signaling::NewConsumerRequest> request) override {}

        void onNewDataConsumer(std::shared_ptr<signaling::NewDataConsumerRequest> request) override {}

        // Notification from SFU
        void onProducerScore(std::shared_ptr<signaling::ProducerScoreNotification> notification) override {}

        void onConsumerScore(std::shared_ptr<signaling::ConsumerScoreNotification> notification) override {}

        void onNewPeer(std::shared_ptr<signaling::NewPeerNotification> notification) override;

        void onPeerClosed(std::shared_ptr<signaling::PeerClosedNotification> notification) override;

        void onPeerDisplayNameChanged(std::shared_ptr<signaling::PeerDisplayNameChangedNotification> notification) override;

        void onConsumerPaused(std::shared_ptr<signaling::ConsumerPausedNotification> notification) override {}

        void onConsumerResumed(std::shared_ptr<signaling::ConsumerResumedNotification> notification) override {}

        void onConsumerClosed(std::shared_ptr<signaling::ConsumerClosedNotification> notification) override {}

        void onConsumerLayersChanged(std::shared_ptr<signaling::ConsumerLayersChangedNotification> notification) override {}

        void onDataConsumerClosed(std::shared_ptr<signaling::DataConsumerClosedNotification> notification) override {}

        void onDownlinkBwe(std::shared_ptr<signaling::DownlinkBweNotification> notification) override {}

        void onActiveSpeaker(std::shared_ptr<signaling::ActiveSpeakerNotification> notification) override;

    protected:
        // IMediaEventHandler
        void onLocalAudioStateChanged(bool enabled, bool muted) override {}

        void onLocalVideoStateChanged(bool enabled) override {}

        void onRemoteAudioStateChanged(const std::string& pid, bool muted) override;

        void onRemoteVideoStateChanged(const std::string& pid, bool muted) override;

        void onCreateLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override {}

        void onRemoveLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override {}

        void onCreateRemoteAudioTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override;

        void onRemoveRemoteAudioTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override;

        void onCreateRemoteVideoTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override;

        void onRemoveRemoteVideoTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override;
    private:
        rtc::Thread* _mediasoupThread;

        std::shared_ptr<IMediaController> _mediaController;

        // key: peer id
        std::unordered_map<std::string, std::shared_ptr<IParticipant>> _participantMap;
    };

}
