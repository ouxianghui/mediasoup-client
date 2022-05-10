#ifndef PARTICIPANT_LIST_VIEW_H
#define PARTICIPANT_LIST_VIEW_H

#include <QWidget>
#include <memory>

namespace vi {
    class IParticipant;
    class IRoomClient;
}

namespace Ui {
    class ParticipantListView;
}

class QListWidgetItem;

class ParticipantListView : public QWidget, public std::enable_shared_from_this<ParticipantListView>
{
    Q_OBJECT

public:
    explicit ParticipantListView(std::shared_ptr<vi::IRoomClient> vrc, QWidget *parent = Q_NULLPTR);

    ~ParticipantListView();

    void addParticipant(std::shared_ptr<vi::IParticipant> participant);

    void updateParticipant(std::shared_ptr<vi::IParticipant> participant);

    void removeParticipant(std::shared_ptr<vi::IParticipant> participant);

    void removeAll();

    bool hasParticipant(const std::string& pid);

    QListWidgetItem* getItem(const std::string& pid);

private:
    Ui::ParticipantListView *ui;

    std::weak_ptr<vi::IRoomClient> _vrc;
};

#endif // PARTICIPANT_LIST_VIEW_H
