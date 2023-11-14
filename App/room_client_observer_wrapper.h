#ifndef ROOMCLIENTOBSERVERWRAPPER_H
#define ROOMCLIENTOBSERVERWRAPPER_H

#include <memory>
#include <QObject>
#include "service/i_room_client_observer.h"
#include "api/media_stream_interface.h"

class RoomClientObserverWrapper : public QObject, public vi::IRoomClientObserver, public std::enable_shared_from_this<RoomClientObserverWrapper>
{
    Q_OBJECT
public:
    RoomClientObserverWrapper(QObject* parent = nullptr);

    void init();

    void destroy();

signals:
    void roomStateChanged(vi::RoomState state);

    void createLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track);

    void removeLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track);

    void localAudioStateChanged(bool enabled, bool muted);

    void localVideoStateChanged(bool enabled);

    void localActiveSpeaker(int32_t volume);

protected:
    // IRoomClientObserver
    void onRoomStateChanged(vi::RoomState state) override;

    void onCreateLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override;

    void onRemoveLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override;

    void onLocalAudioStateChanged(bool enabled, bool muted) override;

    void onLocalVideoStateChanged(bool enabled) override;

    void onLocalActiveSpeaker(int32_t volume) override;
};

#endif // ROOMCLIENTOBSERVERWRAPPER_H
