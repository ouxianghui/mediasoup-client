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
#include "logger/spd_logger.h"
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
    _roomClient = getEngine()->createRoomClient();
    rtc::Thread* callbackThread = getThread("main");
    _roomClient->addObserver(shared_from_this(), callbackThread);

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
}

void MainWindow::updateToolBar()
{
    if (_roomClient->getRoomState() == vi::RoomState::CLOSED) {
        _connectButton->setDefaultAction(_joinAction);
        _audioButton->setDisabled(true);
        _microphoneButton->setDisabled(true);
        _videoButton->setDisabled(true);
    }
    else if (_roomClient->getRoomState() == vi::RoomState::CONNECTED) {
        _connectButton->setDefaultAction(_leaveAction);
        _audioButton->setEnabled(true);
        _videoButton->setEnabled(true);

        if (_roomClient->isAudioEnabled()) {
            _audioButton->setDefaultAction(_disableAudioAction);
            _microphoneButton->setEnabled(true);
        }
        else {
            _audioButton->setDefaultAction(_enableAudioAction);
            _microphoneButton->setDisabled(true);
        }

        if (_roomClient->isAudioMuted()) {
            _microphoneButton->setDefaultAction(_unmuteMicrophoneAction);
        }
        else {
            _microphoneButton->setDefaultAction(_muteMicrophoneAction);
        }

        if (_roomClient->isVideoEnabled()) {
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

    if (_roomClient->isAudioEnabled()) {
        if (_roomClient->isAudioMuted()) {
            myself->setAudioMuted(true);
        }
        else {
            myself->setAudioMuted(false);
        }
    }
    else {
        myself->setAudioMuted(true);
    }

    if (_roomClient->isVideoEnabled()) {
        myself->setVideoMuted(false);
    }
    else {
        myself->setVideoMuted(true);
    }

    myself->setVideoTracks(_roomClient->getVideoTracks());

    myself->setSpeakingVolume(_roomClient->speakingVolume());

    return myself;
}

void MainWindow::loadParticipants()
{
    _galleryView->attach(myself());

    auto pc = _roomClient->getParticipantController();
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
    _roomClient->removeObserver(shared_from_this());

    if (_galleryView) {
        _galleryView->destroy();
    }
}

void MainWindow::onJoinRoom()
{
    _roomClient->join("wevisit.cn", 4443, "test", "jackie", nullptr);
}

void MainWindow::onLeaveRoom()
{
    _roomClient->leave();
}

void MainWindow::onEnableAudio()
{
    _roomClient->enableAudio(true);
}

void MainWindow::onDisableAudio()
{
    _roomClient->enableAudio(false);
}

void MainWindow::onMuteMicrophone()
{
    _roomClient->muteAudio(true);
}

void MainWindow::onUnmuteMicrophone()
{
    _roomClient->muteAudio(false);
}

void MainWindow::onEnableVideo()
{
    _roomClient->enableVideo(true);
}

void MainWindow::onDisableVideo()
{
    _roomClient->enableVideo(false);
}

void MainWindow::onRoomStateChanged(vi::RoomState state)
{
    updateToolBar();

    DLOG("onRoomStateChanged: {}", (int32_t)state);
    if (state == vi::RoomState::CONNECTED) {
        DLOG("MainWindow, RoomState::CONNECTED");

        _galleryView->setLayout(2, 2);

        rtc::Thread* callbackThread = getThread("main");
        _roomClient->getParticipantController()->addObserver(shared_from_this(), callbackThread);

        loadParticipants();

        _roomClient->enableAudio(true);
        _roomClient->enableVideo(true);
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


