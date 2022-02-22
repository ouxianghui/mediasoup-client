#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include "service/mediasoup_api.h"
#include "service/i_room_client_observer.h"
#include "service/i_media_controller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace vi {
    class IParticipant;
}

class RoomEventAdapter;
class MediaEventAdapter;
class ParticipantEventAdapter;
class GalleryView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void init();

    void destroy();

private slots:
    void on_actionJoin_triggered();

    void on_actionleave_triggered();

    void on_actionEnbaleMicrophone_triggered();

    void on_actionDisableMicrophone_triggered();

    void on_actionMuteMicrophone_triggered();

    void on_actionUnmuteMicrophone_triggered();

    void on_actionEnableCamera_triggered();

    void on_actionDisableCamera_triggered();

    void onRoomStateChanged(vi::RoomState state);

    void onVideoTrackCreated(const std::string& id, webrtc::MediaStreamTrackInterface* track);

    void onVideoTrackRemoved(const std::string& id, webrtc::MediaStreamTrackInterface* track);

    void onParticipantCreated(std::shared_ptr<vi::IParticipant> participant);

    void onParticipantUpdated(std::shared_ptr<vi::IParticipant> participant);

    void onParticipantRemoved(std::shared_ptr<vi::IParticipant> participant);

    void closeEvent(QCloseEvent* event);

private:
    Ui::MainWindow *ui;

    std::shared_ptr<RoomEventAdapter> _roomEventAdapter;

    std::shared_ptr<MediaEventAdapter> _mediaEventAdapter;

    std::shared_ptr<ParticipantEventAdapter> _participantEventAdapter;

    GalleryView* _galleryView = nullptr;
};
#endif // MAINWINDOW_H
