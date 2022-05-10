#ifndef PARTICIPANT_ITEM_VIEW_H
#define PARTICIPANT_ITEM_VIEW_H

#include <QWidget>
#include <memory>

namespace vi {
    class IParticipant;
    class IRoomClient;
}

namespace Ui {
    class ParticipantItemView;
}

class QToolButton;
class QAction;

class ParticipantItemView : public QWidget
{
    Q_OBJECT

public:
    ParticipantItemView(std::shared_ptr<vi::IParticipant> participant, std::shared_ptr<vi::IRoomClient> vrc, QWidget *parent = Q_NULLPTR);

    ~ParticipantItemView();

    void init();

    void update(std::shared_ptr<vi::IParticipant> participant);

private slots:
    void onMuteAudio();

    void onUnmuteAudio();

    void onMuteVideo();

    void onUnmuteVideo();

    void onVolumeTimeout();

private:
    Ui::ParticipantItemView *ui;

    std::shared_ptr<vi::IParticipant> _participant;

    std::weak_ptr<vi::IRoomClient> _vrc;

    QAction* _muteAudioAction;
    QAction* _unmuteAudioAction;

    QAction* _muteVideoAction;
    QAction* _unmuteVideoAction;
};

#endif // PARTICIPANT_ITEM_VIEW_H
