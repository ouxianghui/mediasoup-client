#include "participant_item_view.h"
#include "ui_participant_item_view.h"
#include <QAction>
#include <QTimer>
#include "logger/spd_logger.h"
#include "service/i_room_client.h"
#include "service/i_participant.h"
#include "service/i_participant_controller.h"


ParticipantItemView::ParticipantItemView(std::shared_ptr<vi::IParticipant> participant, std::shared_ptr<vi::IRoomClient> vrc, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ParticipantItemView)
    , _participant(participant)
    , _vrc(vrc)
{
    ui->setupUi(this);
    this->setMinimumHeight(64);
    init();
}

ParticipantItemView::~ParticipantItemView()
{
    delete ui;
}

void ParticipantItemView::init()
{
    ui->toolButtonAudio->setMinimumSize(QSize(32, 32));
    ui->toolButtonVideo->setMinimumSize(QSize(32, 32));

    _muteAudioAction = new QAction(this);
    _muteAudioAction->setIcon(QIcon(":/app/resource/icons8-block-microphone-100.png"));
    _muteAudioAction->setToolTip("mute audio");
    connect(_muteAudioAction, &QAction::triggered, this, &ParticipantItemView::onMuteAudio);

    _unmuteAudioAction = new QAction(this);
    _unmuteAudioAction->setIcon(QIcon(":/app/resource/icons8-microphone-100.png"));
    _unmuteAudioAction->setToolTip("unmute audio");
    connect(_unmuteAudioAction, &QAction::triggered, this, &ParticipantItemView::onUnmuteAudio);

    _muteVideoAction = new QAction(this);
    _muteVideoAction->setIcon(QIcon(":/app/resource/icons8-no-video-100.png"));
    _muteVideoAction->setToolTip("mute video");
    connect(_muteVideoAction, &QAction::triggered, this, &ParticipantItemView::onMuteVideo);

    _unmuteVideoAction = new QAction(this);
    _unmuteVideoAction->setIcon(QIcon(":/app/resource/icons8-video-call-100.png"));
    _unmuteVideoAction->setToolTip("unmute video");
    connect(_unmuteVideoAction, &QAction::triggered, this, &ParticipantItemView::onUnmuteVideo);

    update(_participant);
}

void ParticipantItemView::update(std::shared_ptr<vi::IParticipant> newParticipant)
{
    if (!newParticipant && !_participant) {
        DLOG("Invalid participant");
        return;
    }

    if (newParticipant !=  _participant) {
        _participant = newParticipant;
    }

    ui->labelDisplayName->setText(_participant->displayName().c_str());

    if (_participant->hasAudio()) {
        ui->progressBarVolume->setValue(100+_participant->speakingVolume());
        QTimer::singleShot(3000, this, &ParticipantItemView::onVolumeTimeout);
    }
    else {
        ui->progressBarVolume->setValue(0);
    }

    ui->progressBarVolume->setEnabled(_participant->hasAudio());

    ui->toolButtonAudio->setEnabled(_participant->hasAudio());

    if (_participant->isAudioMuted()) {
        ui->toolButtonAudio->setDefaultAction(_muteAudioAction);
        ui->progressBarVolume->setValue(0);
    }
    else {
        ui->toolButtonAudio->setDefaultAction(_unmuteAudioAction);
    }

    ui->toolButtonVideo->setEnabled(_participant->hasVideo());

    if (_participant->isVideoMuted()) {
        ui->toolButtonVideo->setDefaultAction(_muteVideoAction);
    }
    else {
        ui->toolButtonVideo->setDefaultAction(_unmuteVideoAction);
    }
}

void ParticipantItemView::onMuteAudio()
{
    // Need Mediasoup server support
    return;

    auto vrc = _vrc.lock();
    if (!vrc) {
        DLOG("Invalid vrc");
        return;
    }

    if (!_participant) {
        DLOG("Invalid participant");
        return;
    }

    if (!_participant->hasAudio()) {
        DLOG("no audio");
        return;
    }

    if (_participant->isAudioMuted()) {
        DLOG("ignore operation");
        return;
    }

    auto pc = vrc->getParticipantController();
    if (!pc) {
         DLOG("Invalid participant controller");
        return;
    }
    pc->muteAudio(_participant->id(), true);
}

void ParticipantItemView::onUnmuteAudio()
{
    // Need Mediasoup server support
    return;
    auto vrc = _vrc.lock();
    if (!vrc) {
        DLOG("Invalid vrc");
        return;
    }

    if (!_participant) {
        DLOG("Invalid participant");
        return;
    }

    if (!_participant->hasAudio()) {
        DLOG("no audio");
        return;
    }

    if (!_participant->isAudioMuted()) {
        DLOG("ignore operation");
        return;
    }

    auto pc = vrc->getParticipantController();
    if (!pc) {
         DLOG("Invalid participant controller");
        return;
    }
    pc->muteAudio(_participant->id(), false);
}

void ParticipantItemView::onMuteVideo()
{
    // Need Mediasoup server support
    return;
    auto vrc = _vrc.lock();
    if (!vrc) {
        DLOG("Invalid vrc");
        return;
    }

    if (!_participant) {
        DLOG("Invalid participant");
        return;
    }

    if (!_participant->hasVideo()) {
        DLOG("no video");
        return;
    }

    if (_participant->isVideoMuted()) {
        DLOG("ignore operation");
        return;
    }

    auto pc = vrc->getParticipantController();
    if (!pc) {
         DLOG("Invalid participant controller");
        return;
    }
    pc->muteVideo(_participant->id(), true);
}

void ParticipantItemView::onUnmuteVideo()
{
    // Need Mediasoup server support
    return;
    auto vrc = _vrc.lock();
    if (!vrc) {
        DLOG("Invalid vrc");
        return;
    }

    if (!_participant) {
        DLOG("Invalid participant");
        return;
    }

    if (!_participant->hasVideo()) {
        DLOG("no video");
        return;
    }

    if (!_participant->isVideoMuted()) {
        DLOG("ignore operation");
        return;
    }

    auto pc = vrc->getParticipantController();
    if (!pc) {
         DLOG("Invalid participant controller");
        return;
    }
    pc->muteVideo(_participant->id(), true);
}

void ParticipantItemView::onVolumeTimeout()
{
    ui->progressBarVolume->setValue(0);
}
