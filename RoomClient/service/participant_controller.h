#pragma once

#include <memory>
#include "i_participant_controller.h"
#include "i_media_controller_observer.h"
#include "utils/universal_observable.hpp"
#include "i_signaling_observer.h"
#include "rtc_base/thread.h"
#include "service/i_media_controller_observer.h"

namespace vi {

    class IMediaController;
    class Participant;

    class ParticipantController :
            public IParticipantController,
            public ISignalingObserver,
            public IMediaControllerObserver,
            public UniversalObservable<IParticipantControllerObserver>,
            public std::enable_shared_from_this<ParticipantController>
    {
    public:
        ParticipantController(rtc::Thread* internalThread, std::shared_ptr<IMediaController> mediaController);

        ~ParticipantController();

        void init();

        void destroy();

        void addObserver(std::shared_ptr<IParticipantControllerObserver> observer, rtc::Thread* callbackThread) override;

        void removeObserver(std::shared_ptr<IParticipantControllerObserver> observer) override;

        std::shared_ptr<IParticipant> getParticipant(const std::string& pid) override;

        std::unordered_map<std::string, std::shared_ptr<IParticipant>> getParticipants() override;

        void muteAudio(const std::string& pid, bool muted) override;

        bool isAudioMuted(const std::string& pid) override;

        void muteVideo(const std::string& pid, bool muted) override;

        bool isVideoMuted(const std::string& pid) override;

        void createParticipant(const std::string& pid, const std::string& displayName);

    protected:
        // ISignalingObserver
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
        // IMediaControllerObserver
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
        rtc::Thread* _internalThread;

        std::shared_ptr<IMediaController> _mediaController;

        // key: peer id
        std::unordered_map<std::string, std::shared_ptr<IParticipant>> _participantMap;
    };

}
