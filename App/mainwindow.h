#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QAction>
#include "service/mediasoup_api.h"
#include "service/i_room_client_observer.h"
#include "service/i_media_controller.h"
#include "service/i_participant_controller_observer.h"
#include "participant_controller_observer_wrapper.h"
#include "room_client_observer_wrapper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace vi {
    class IParticipant;
}

class GalleryView;
class ParticipantListView;
class QToolButton;
class QAction;

class MainWindow : public QMainWindow, public vi::IRoomClientObserver, public vi::IParticipantControllerObserver, public std::enable_shared_from_this<MainWindow>
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void init();

    void destroy();

private slots:
    // IRoomClientObserver
    void onRoomStateChanged(vi::RoomState state) override;

    void onCreateLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override;

    void onRemoveLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override;

    void onLocalAudioStateChanged(bool enabled, bool muted) override;

    void onLocalVideoStateChanged(bool enabled) override;

    void onLocalActiveSpeaker(int32_t volume) override;

    // IParticipantControllerObserver
    void onParticipantJoin(std::shared_ptr<vi::IParticipant> participant) override;

    void onParticipantLeave(std::shared_ptr<vi::IParticipant> participant) override;

    void onRemoteActiveSpeaker(std::shared_ptr<vi::IParticipant> participant, int32_t volume) override;

    void onDisplayNameChanged(std::shared_ptr<vi::IParticipant> participant) override;

    void onCreateRemoteVideoTrack(std::shared_ptr<vi::IParticipant> participant, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override;

    void onRemoveRemoteVideoTrack(std::shared_ptr<vi::IParticipant> participant, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) override;

    void onRemoteAudioStateChanged(std::shared_ptr<vi::IParticipant> participant, bool muted) override;

    void onRemoteVideoStateChanged(std::shared_ptr<vi::IParticipant> participant, bool muted) override;

private:
    void closeEvent(QCloseEvent* event) override;

    void updateToolBar();

    std::shared_ptr<vi::IParticipant> myself();

    void loadParticipants();

private slots:
    void onJoinRoom();

    void onLeaveRoom();

    void onEnableAudio();

    void onDisableAudio();

    void onMuteMicrophone();

    void onUnmuteMicrophone();

    void onEnableVideo();

    void onDisableVideo();

private:
    Ui::MainWindow *ui;

    GalleryView* _galleryView = nullptr;

    //std::shared_ptr<ParticipantListView> _participantListView;

    QToolButton* _connectButton;
    QAction* _joinAction;
    QAction* _leaveAction;

    QToolButton* _audioButton;
    QAction* _enableAudioAction;
    QAction* _disableAudioAction;

    QToolButton* _microphoneButton;
    QAction* _muteMicrophoneAction;
    QAction* _unmuteMicrophoneAction;

    QToolButton* _videoButton;
    QAction* _enableVideoAction;
    QAction* _disableVideoAction;

    std::shared_ptr<RoomClientObserverWrapper> _roomClientObserverWrapper;

    std::shared_ptr<ParticipantControllerObserverWrapper> _participantControllerObserverWrapper;
};
#endif // MAINWINDOW_H
