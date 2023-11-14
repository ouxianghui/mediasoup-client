#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QToolButton>
#include "service/signaling_client.h"
#include "service/signaling_models.h"
#include "service/room_client.h"
#include "websocket/websocket_request.h"
#include "service/signaling_models.h"
#include "logger/u_logger.h"
#include "service/engine.h"
#include "gallery_view.h"
#include "service/i_media_controller.h"
#include "service/i_participant_controller.h"
#include "mac_video_renderer.h"
#include "video_renderer.h"
#include "utils/thread_provider.h"
#include "participant_list_view.h"
#include "service/participant.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMinimumWidth(1920);
    this->setMinimumHeight(1080);
    //QPalette palette;
    //palette.setColor(QPalette::Background, QColor("red"));
    //this->setPalette(palette);
    //this->setAutoFillBackground(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    DLOG("~MainWindow()");
}

void MainWindow::init()
{
    _roomClientObserverWrapper = std::make_shared<RoomClientObserverWrapper>(this);

    connect(_roomClientObserverWrapper.get(), &RoomClientObserverWrapper::roomStateChanged, this, &MainWindow::onRoomStateChanged, Qt::QueuedConnection);
    connect(_roomClientObserverWrapper.get(), &RoomClientObserverWrapper::createLocalVideoTrack, this, &MainWindow::onCreateLocalVideoTrack, Qt::QueuedConnection);
    connect(_roomClientObserverWrapper.get(), &RoomClientObserverWrapper::removeLocalVideoTrack, this, &MainWindow::onRemoveLocalVideoTrack, Qt::QueuedConnection);
    connect(_roomClientObserverWrapper.get(), &RoomClientObserverWrapper::localAudioStateChanged, this, &MainWindow::onLocalAudioStateChanged, Qt::QueuedConnection);
    connect(_roomClientObserverWrapper.get(), &RoomClientObserverWrapper::localVideoStateChanged, this, &MainWindow::onLocalVideoStateChanged, Qt::QueuedConnection);
    connect(_roomClientObserverWrapper.get(), &RoomClientObserverWrapper::localActiveSpeaker, this, &MainWindow::onLocalActiveSpeaker, Qt::QueuedConnection);

    _participantControllerObserverWrapper = std::make_shared<ParticipantControllerObserverWrapper>(this);

    connect(_participantControllerObserverWrapper.get(), &ParticipantControllerObserverWrapper::participantJoin, this, &MainWindow::onParticipantJoin, Qt::QueuedConnection);
    connect(_participantControllerObserverWrapper.get(), &ParticipantControllerObserverWrapper::participantLeave, this, &MainWindow::onParticipantLeave, Qt::QueuedConnection);
    connect(_participantControllerObserverWrapper.get(), &ParticipantControllerObserverWrapper::remoteActiveSpeaker, this, &MainWindow::onRemoteActiveSpeaker, Qt::QueuedConnection);
    connect(_participantControllerObserverWrapper.get(), &ParticipantControllerObserverWrapper::displayNameChanged, this, &MainWindow::onDisplayNameChanged, Qt::QueuedConnection);
    connect(_participantControllerObserverWrapper.get(), &ParticipantControllerObserverWrapper::createRemoteVideoTrack, this, &MainWindow::onCreateRemoteVideoTrack, Qt::QueuedConnection);
    connect(_participantControllerObserverWrapper.get(), &ParticipantControllerObserverWrapper::removeRemoteVideoTrack, this, &MainWindow::onRemoveRemoteVideoTrack, Qt::QueuedConnection);
    connect(_participantControllerObserverWrapper.get(), &ParticipantControllerObserverWrapper::remoteAudioStateChanged, this, &MainWindow::onRemoteAudioStateChanged, Qt::QueuedConnection);
    connect(_participantControllerObserverWrapper.get(), &ParticipantControllerObserverWrapper::remoteVideoStateChanged, this, &MainWindow::onRemoteVideoStateChanged, Qt::QueuedConnection);

    rtc::Thread* callbackThread = getThread("main");
    //getRoomClient()->addObserver(shared_from_this(), callbackThread);

    getRoomClient()->addObserver(_roomClientObserverWrapper, callbackThread);
    getRoomClient()->getParticipantController()->addObserver(_participantControllerObserverWrapper, callbackThread);

    if (!_galleryView) {
        _galleryView = new GalleryView(this);
        _galleryView->init();
        _galleryView->setFrameShape(QFrame::Shape::Box);
        setCentralWidget(_galleryView);
    }

    ui->toolBar->setIconSize(QSize(64, 64));

    _connectButton = new QToolButton(this);
    ui->toolBar->addWidget(_connectButton);

    _joinAction = new QAction(this);
    _joinAction->setIcon(QIcon(":/app/resource/icons8-connected-100.png"));
    _joinAction->setToolTip("join room");
    connect(_joinAction, &QAction::triggered, this, &MainWindow::onJoinRoom);

    _leaveAction = new QAction(this);
    _leaveAction->setIcon(QIcon(":/app/resource/icons8-disconnected-100.png"));
    _leaveAction->setToolTip("leave room");
    connect(_leaveAction, &QAction::triggered, this, &MainWindow::onLeaveRoom);

    _connectButton->setDefaultAction(_joinAction);

    ui->toolBar->addSeparator();

    _audioButton = new QToolButton(this);
    ui->toolBar->addWidget(_audioButton);

    _enableAudioAction = new QAction(this);
    _enableAudioAction->setIcon(QIcon(":/app/resource/icons8-voice-100.png"));
    _enableAudioAction->setToolTip("enable audio");
    connect(_enableAudioAction, &QAction::triggered, this, &MainWindow::onEnableAudio);

    _disableAudioAction = new QAction(this);
    _disableAudioAction->setIcon(QIcon(":/app/resource/icons8-mute-100.png"));
    _disableAudioAction->setToolTip("disable audio");
    connect(_disableAudioAction, &QAction::triggered, this, &MainWindow::onDisableAudio);

    _audioButton->setDefaultAction(_enableAudioAction);

    ui->toolBar->addSeparator();

    _microphoneButton = new QToolButton(this);
    ui->toolBar->addWidget(_microphoneButton);

    _muteMicrophoneAction = new QAction(this);
    _muteMicrophoneAction->setIcon(QIcon(":/app/resource/icons8-block-microphone-100.png"));
    _muteMicrophoneAction->setToolTip("mute microphone");
    connect(_muteMicrophoneAction, &QAction::triggered, this, &MainWindow::onMuteMicrophone);

    _unmuteMicrophoneAction = new QAction(this);
    _unmuteMicrophoneAction->setIcon(QIcon(":/app/resource/icons8-microphone-100.png"));
    _unmuteMicrophoneAction->setToolTip("unmute microphone");
    connect(_unmuteMicrophoneAction, &QAction::triggered, this, &MainWindow::onUnmuteMicrophone);

    _microphoneButton->setDefaultAction(_muteMicrophoneAction);

    ui->toolBar->addSeparator();

    _videoButton = new QToolButton(this);
    ui->toolBar->addWidget(_videoButton);

    _enableVideoAction = new QAction(this);
    _enableVideoAction->setIcon(QIcon(":/app/resource/icons8-video-call-100.png"));
    _enableVideoAction->setToolTip("enable video");
    connect(_enableVideoAction, &QAction::triggered, this, &MainWindow::onEnableVideo);

    _disableVideoAction = new QAction(this);
    _disableVideoAction->setIcon(QIcon(":/app/resource/icons8-no-video-100.png"));
    _disableVideoAction->setToolTip("disable video");
    connect(_disableVideoAction, &QAction::triggered, this, &MainWindow::onDisableVideo);

    _videoButton->setDefaultAction(_enableVideoAction);

    updateToolBar();

//    getRoomClient()->join("47.97.188.24", 4443, "test", "jackie", nullptr);
}

void MainWindow::updateToolBar()
{
    if (getRoomClient()->getRoomState() == vi::RoomState::CLOSED) {
        _connectButton->setDefaultAction(_joinAction);
        _audioButton->setDisabled(true);
        _microphoneButton->setDisabled(true);
        _videoButton->setDisabled(true);
    }
    else if (getRoomClient()->getRoomState() == vi::RoomState::CONNECTED) {
        _connectButton->setDefaultAction(_leaveAction);
        _audioButton->setEnabled(true);
        _videoButton->setEnabled(true);

        if (getRoomClient()->isAudioEnabled()) {
            _audioButton->setDefaultAction(_disableAudioAction);
            _microphoneButton->setEnabled(true);
        }
        else {
            _audioButton->setDefaultAction(_enableAudioAction);
            _microphoneButton->setDisabled(true);
        }

        if (getRoomClient()->isAudioMuted()) {
            _microphoneButton->setDefaultAction(_unmuteMicrophoneAction);
        }
        else {
            _microphoneButton->setDefaultAction(_muteMicrophoneAction);
        }

        if (getRoomClient()->isVideoEnabled()) {
            _videoButton->setDefaultAction(_disableVideoAction);
        }
        else {
            _videoButton->setDefaultAction(_enableVideoAction);
        }
    }
}

std::shared_ptr<vi::IParticipant> MainWindow::myself()
{
    auto myself = std::make_shared<vi::Participant>("0", "[You]");

    if (getRoomClient()->isAudioEnabled()) {
        if (getRoomClient()->isAudioMuted()) {
            myself->setAudioMuted(true);
        }
        else {
            myself->setAudioMuted(false);
        }
    }
    else {
        myself->setAudioMuted(true);
    }

    if (getRoomClient()->isVideoEnabled()) {
        myself->setVideoMuted(false);
    }
    else {
        myself->setVideoMuted(true);
    }

    myself->setVideoTracks(getRoomClient()->getVideoTracks());

    myself->setSpeakingVolume(getRoomClient()->speakingVolume());

    return myself;
}

void MainWindow::loadParticipants()
{
    _galleryView->attach(myself());

    auto pc = getRoomClient()->getParticipantController();
    if (!pc) {
        return;
    }
    auto participantMap = pc->getParticipants();
    for (const auto& pair : participantMap) {
        _galleryView->attach(pair.second);
    }
}

void MainWindow::destroy()
{
    //getRoomClient()->removeObserver(shared_from_this());
    getRoomClient()->removeObserver(_roomClientObserverWrapper);

    if (_galleryView) {
        _galleryView->destroy();
    }
}

void MainWindow::onJoinRoom()
{
//    getRoomClient()->join("124.221.73.157", 4443, "test2", "jackie1", nullptr);
    getRoomClient()->join("10.32.56.14", 4443, "123123123", "jackie", nullptr);
//    getRoomClient()->join("localhost", 8443, "test2", "jackie1", nullptr);
}

void MainWindow::onLeaveRoom()
{
    getRoomClient()->leave();
}

void MainWindow::onEnableAudio()
{
    getRoomClient()->enableAudio(true);
}

void MainWindow::onDisableAudio()
{
    getRoomClient()->enableAudio(false);
}

void MainWindow::onMuteMicrophone()
{
    getRoomClient()->muteAudio(true);
}

void MainWindow::onUnmuteMicrophone()
{
    getRoomClient()->muteAudio(false);
}

void MainWindow::onEnableVideo()
{
    getRoomClient()->enableVideo(true);
}

void MainWindow::onDisableVideo()
{
    getRoomClient()->enableVideo(false);
}

void MainWindow::onRoomStateChanged(vi::RoomState state)
{
    updateToolBar();

    DLOG("onRoomStateChanged: {}", (int32_t)state);
    if (state == vi::RoomState::CONNECTED) {
        DLOG("MainWindow, RoomState::CONNECTED");

        _galleryView->setLayout(2, 2);

        rtc::Thread* callbackThread = getThread("main");
        //getRoomClient()->getParticipantController()->addObserver(shared_from_this(), callbackThread);

        loadParticipants();

        //getRoomClient()->enableAudio(true);
        //getRoomClient()->enableVideo(true);
    }
    else if (state == vi::RoomState::CLOSED) {
        _galleryView->reset();
    }
}

void MainWindow::onCreateLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    _galleryView->update(myself());
}

void MainWindow::onRemoveLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    _galleryView->update(myself());
}

void MainWindow::onLocalAudioStateChanged(bool enabled, bool muted)
{
    updateToolBar();
    _galleryView->update(myself());
}

void MainWindow::onLocalVideoStateChanged(bool enabled)
{
    updateToolBar();

    _galleryView->update(myself());
}

void MainWindow::onLocalActiveSpeaker(int32_t volume)
{
    _galleryView->update(myself());
}

void MainWindow::onParticipantJoin(std::shared_ptr<vi::IParticipant> participant)
{
    _galleryView->attach(participant);
}

void MainWindow::onParticipantLeave(std::shared_ptr<vi::IParticipant> participant)
{
    _galleryView->detach(participant);
}

void MainWindow::onRemoteActiveSpeaker(std::shared_ptr<vi::IParticipant> participant, int32_t volume)
{
    _galleryView->update(participant);
}

void MainWindow::onDisplayNameChanged(std::shared_ptr<vi::IParticipant> participant)
{
    _galleryView->update(participant);
}

void MainWindow::onCreateRemoteVideoTrack(std::shared_ptr<vi::IParticipant> participant, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    _galleryView->update(participant);
}

void MainWindow::onRemoveRemoteVideoTrack(std::shared_ptr<vi::IParticipant> participant, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    _galleryView->update(participant);
}

void MainWindow::onRemoteAudioStateChanged(std::shared_ptr<vi::IParticipant> participant, bool muted)
{
    _galleryView->update(participant);
}

void MainWindow::onRemoteVideoStateChanged(std::shared_ptr<vi::IParticipant> participant, bool muted)
{
    _galleryView->update(participant);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    destroy();
}


