#include "participant_controller.h"
#include "logger/spd_logger.h"
#include "i_participant_event_handler.h"
#include "participant.h"
#include "media_controller.h"
#include "rtc_base/thread.h"

namespace vi {

ParticipantController::ParticipantController(rtc::Thread* mediasoupThread, std::shared_ptr<IMediaController> mediaController)
    : _mediasoupThread(mediasoupThread)
    , _mediaController(mediaController)
    {

    }

    ParticipantController::~ParticipantController()
    {
        DLOG("~ParticipantController()");
    }

    void ParticipantController::init()
    {

    }

    void ParticipantController::destroy()
    {
        _participantMap.clear();
    }

    void ParticipantController::addObserver(std::shared_ptr<IParticipantEventHandler> observer, rtc::Thread* callbackThread)
    {
        UniversalObservable<IParticipantEventHandler>::addWeakObserver(observer, callbackThread);
    }

    void ParticipantController::removeObserver(std::shared_ptr<IParticipantEventHandler> observer)
    {
        UniversalObservable<IParticipantEventHandler>::removeObserver(observer);
    }

    std::shared_ptr<IParticipant> ParticipantController::getParticipant(const std::string& pid)
    {
        if (_participantMap.find(pid) != _participantMap.end()) {
            return _participantMap[pid];
        }

        return nullptr;
    }

    std::unordered_map<std::string, std::shared_ptr<IParticipant>> ParticipantController::getParticipants()
    {
        return _participantMap;
    }

    void ParticipantController::muteAudio(const std::string& pid, bool muted)
    {
        _mediaController->muteAudio(pid, muted);
    }

    bool ParticipantController::isAudioMuted(const std::string& pid)
    {
        return _mediaController->isAudioMuted(pid);
    }

    void ParticipantController::muteVideo(const std::string& pid, bool muted)
    {
        _mediaController->muteVideo(pid, muted);
    }

    bool ParticipantController::isVideoMuted(const std::string& pid)
    {
        return _mediaController->isVideoMuted(pid);
    }

    void ParticipantController::createParticipant(const std::string& pid, const std::string& displayName)
    {
        if (_participantMap.find(pid) != _participantMap.end()) {
            return;
        }

        _participantMap[pid] = std::make_shared<Participant>(pid, displayName);

        auto participant = _participantMap[pid];
        if (!participant) {
            return;
        }

        auto impl = std::dynamic_pointer_cast<Participant>(participant);
        impl->setActive(true);
        UniversalObservable<IParticipantEventHandler>::notifyObservers([wself = weak_from_this(), participant](const auto& observer) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            observer->onParticipantJoin(participant);
        });
    }

    void ParticipantController::onNewPeer(std::shared_ptr<signaling::NewPeerNotification> notification)
    {
        if (!notification) {
            return;
        }

        auto pid = notification->data->id.value_or("");
        if (pid.empty()) {
            return;
        }

        if (_participantMap.find(pid) == _participantMap.end()) {
            createParticipant(pid, notification->data->displayName.value_or(""));
        }
    }

    void ParticipantController::onPeerClosed(std::shared_ptr<signaling::PeerClosedNotification> notification)
    {
        if (!notification) {
            return;
        }

        auto pid = notification->data->peerId.value_or("");
        if (pid.empty()) {
            return;
        }

        auto participant = _participantMap[pid];
        if (!participant) {
            return;
        }

        auto impl = std::dynamic_pointer_cast<Participant>(participant);
        impl->setActive(false);

        UniversalObservable<IParticipantEventHandler>::notifyObservers([wself = weak_from_this(), participant](const auto& observer) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            observer->onParticipantLeave(participant);
        });

        _participantMap.erase(pid);
    }

    void ParticipantController::onPeerDisplayNameChanged(std::shared_ptr<signaling::PeerDisplayNameChangedNotification> notification)
    {
        if (!notification) {
            return;
        }

        auto pid = notification->data->peerId.value_or("");
        if (pid.empty()) {
            return;
        }

        auto participant = _participantMap[pid];
        if (!participant) {
            return;
        }

        auto impl = std::dynamic_pointer_cast<Participant>(participant);
        impl->setDisplayName(notification->data->displayName.value_or(""));
        UniversalObservable<IParticipantEventHandler>::notifyObservers([wself = weak_from_this(), participant](const auto& observer) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            observer->onDisplayNameChanged(participant);
        });
    }

    void ParticipantController::onActiveSpeaker(std::shared_ptr<signaling::ActiveSpeakerNotification> notification)
    {
        if (!notification) {
            return;
        }

        auto pid = notification->data->peerId.value_or("");
        if (pid.empty()) {
            return;
        }

        auto participant = _participantMap[pid];
        if (!participant) {
            return;
        }

        int32_t volume = notification->data->volume.value_or(0);
        //// The exact formula to convert from dBs (-100..0) to linear (0..1) is: Math.pow(10, dBs / 20)
        //// However it does not produce a visually useful output, so let exagerate
        //// it a bit. Also, let convert it from 0..1 to 0..10 and avoid value 1 to
        //// minimize component renderings.
        //auto volume = std::round(std::pow<double>(10, (double)dBs / 85) * 10);

        //if (volume == 1) {
        //    volume = 0;
        //}
        //DLOG("ParticipantController: dBs = {}, volume = {}", dBs, volume);
        auto impl = std::dynamic_pointer_cast<Participant>(participant);
        impl->setSpeakingVolume(volume);
        UniversalObservable<IParticipantEventHandler>::notifyObservers([wself = weak_from_this(), participant, volume](const auto& observer) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            observer->onRemoteActiveSpeaker(participant, volume);
        });
    }

    void ParticipantController::onRemoteAudioStateChanged(const std::string& pid, bool muted)
    {
        if (pid.empty()) {
            return;
        }

        auto participant = _participantMap[pid];
        if (!participant) {
            return;
        }

        auto impl = std::dynamic_pointer_cast<Participant>(participant);
        impl->setAudioMuted(muted);

        UniversalObservable<IParticipantEventHandler>::notifyObservers([wself = weak_from_this(), participant, muted](const auto& observer) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            observer->onRemoteAudioStateChanged(participant, muted);
        });
    }

    void ParticipantController::onRemoteVideoStateChanged(const std::string& pid, bool muted)
    {
        if (pid.empty()) {
            return;
        }

        auto participant = _participantMap[pid];
        if (!participant) {
            return;
        }

        auto impl = std::dynamic_pointer_cast<Participant>(participant);
        impl->setVideoMuted(muted);

        UniversalObservable<IParticipantEventHandler>::notifyObservers([wself = weak_from_this(), participant, muted](const auto& observer) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            observer->onRemoteVideoStateChanged(participant, muted);
        });
    }

    void ParticipantController::onCreateRemoteAudioTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
    {
        if (pid.empty()) {
            return;
        }

        auto participant = _participantMap[pid];
        if (!participant) {
            return;
        }

        auto impl = std::dynamic_pointer_cast<Participant>(participant);
        impl->setAudioTracks(_mediaController->getRemoteAudioTracks(pid));
    }

    void ParticipantController::onRemoveRemoteAudioTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
    {
        if (pid.empty()) {
            return;
        }

        auto participant = _participantMap[pid];
        if (!participant) {
            return;
        }

        auto impl = std::dynamic_pointer_cast<Participant>(participant);
        impl->setAudioTracks(_mediaController->getRemoteAudioTracks(pid));
    }

    void ParticipantController::onCreateRemoteVideoTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
    {
        if (pid.empty()) {
            return;
        }

        auto participant = _participantMap[pid];
        if (!participant) {
            return;
        }

        auto impl = std::dynamic_pointer_cast<Participant>(participant);
        impl->setVideoTracks(_mediaController->getRemoteVideoTracks(pid));

        UniversalObservable<IParticipantEventHandler>::notifyObservers([wself = weak_from_this(), participant, tid, track](const auto& observer) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            observer->onCreateRemoteVideoTrack(participant, tid, track);
        });
    }

    void ParticipantController::onRemoveRemoteVideoTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
    {
        if (pid.empty()) {
            return;
        }

        auto participant = _participantMap[pid];
        if (!participant) {
            return;
        }

        auto impl = std::dynamic_pointer_cast<Participant>(participant);
        impl->setVideoTracks(_mediaController->getRemoteVideoTracks(pid));

        UniversalObservable<IParticipantEventHandler>::notifyObservers([wself = weak_from_this(), participant, tid, track](const auto& observer) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            observer->onRemoveRemoteVideoTrack(participant, tid, track);
        });
    }

}
