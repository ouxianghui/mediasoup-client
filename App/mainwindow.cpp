#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "service/signaling_client.h"
#include "service/signaling_models.h"
#include "service/room_client.h"
#include "websocket/websocket_request.h"
#include "service/signaling_models.h"
#include "logger/u_logger.h"
#include "app_delegate.h"
#include "room_event_adapter.h"
#include "media_event_adapter.h"
#include "participant_event_adapter.h"
#include "gallery_view.h"
#include "service/i_media_controller.h"
#include "service/i_participant_controller.h"
#include "mac_video_renderer.h"
#include "video_renderer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _roomEventAdapter = std::make_shared<RoomEventAdapter>(this);
    connect(_roomEventAdapter.get(), &RoomEventAdapter::roomStateChanged, this, &MainWindow::onRoomStateChanged);

    _mediaEventAdapter = std::make_shared<MediaEventAdapter>(this);
    connect(_mediaEventAdapter.get(), &MediaEventAdapter::videoTrackCreated, this, &MainWindow::onVideoTrackCreated);
    connect(_mediaEventAdapter.get(), &MediaEventAdapter::videoTrackRemoved, this, &MainWindow::onVideoTrackRemoved);

    _participantEventAdapter = std::make_shared<ParticipantEventAdapter>(this);
    connect(_participantEventAdapter.get(), &ParticipantEventAdapter::participantCreated, this, &MainWindow::onParticipantCreated);
    connect(_participantEventAdapter.get(), &ParticipantEventAdapter::participantUpdated, this, &MainWindow::onParticipantUpdated);
    connect(_participantEventAdapter.get(), &ParticipantEventAdapter::participantRemoved, this, &MainWindow::onParticipantRemoved);
}

MainWindow::~MainWindow()
{
    delete ui;
    DLOG("~MainWindow()");
}

void MainWindow::init()
{
    RClient->addObserver(_roomEventAdapter);
    RClient->getMediaController()->addObserver(_mediaEventAdapter);
    RClient->getParticipantController()->addObserver(_participantEventAdapter);

    if (!_galleryView) {
        _galleryView = new GalleryView(this);
        setCentralWidget(_galleryView);
    }
}

void MainWindow::destroy()
{
    RClient->removeObserver(_roomEventAdapter);
    RClient->getMediaController()->removeObserver(_mediaEventAdapter);
    RClient->getParticipantController()->removeObserver(_participantEventAdapter);

    if (_galleryView) {
        _galleryView->removeAll();
    }
}

void MainWindow::onRoomStateChanged(vi::RoomState state)
{
    if (state == vi::RoomState::CONNECTED) {
        RClient->getMediaController()->enableAudio(true);
        RClient->getMediaController()->enableVideo(true);
    }
}

void MainWindow::onVideoTrackCreated(const std::string& id, webrtc::MediaStreamTrackInterface* track)
{
    if (!track) {
        return;
    }
    //MacVideoRenderer* renderer = new MacVideoRenderer(_galleryView);
    VideoRenderer* renderer = new VideoRenderer(_galleryView);
    renderer->init();
    renderer->show();

    webrtc::VideoTrackInterface* vt = static_cast<webrtc::VideoTrackInterface*>(track);
    std::shared_ptr<ContentView> view = std::make_shared<ContentView>(id, vt, renderer);
    view->init();

    _galleryView->insertView(view);
}

void MainWindow::onVideoTrackRemoved(const std::string& id, webrtc::MediaStreamTrackInterface* track)
{
    if (!track) {
        return;
    }
    _galleryView->removeView(id);
}

void MainWindow::on_actionJoin_triggered()
{
    RClient->join("192.168.64.3", 4443, "test-room", "jackie", nullptr);
}

void MainWindow::on_actionleave_triggered()
{

}

void MainWindow::on_actionEnbaleMicrophone_triggered()
{

}

void MainWindow::on_actionDisableMicrophone_triggered()
{

}

void MainWindow::on_actionMuteMicrophone_triggered()
{

}

void MainWindow::on_actionUnmuteMicrophone_triggered()
{

}

void MainWindow::on_actionEnableCamera_triggered()
{
    RClient->getMediaController()->enableVideo(true);
}

void MainWindow::on_actionDisableCamera_triggered()
{
    RClient->getMediaController()->enableVideo(false);
}

void MainWindow::onParticipantCreated(std::shared_ptr<vi::IParticipant> participant)
{

}

void MainWindow::onParticipantUpdated(std::shared_ptr<vi::IParticipant> participant)
{

}

void MainWindow::onParticipantRemoved(std::shared_ptr<vi::IParticipant> participant)
{

}

void MainWindow::closeEvent(QCloseEvent* event)
{
    destroy();
}
