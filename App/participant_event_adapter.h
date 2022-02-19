#pragma once

#include <QObject>
#include <memory>
#include "service/i_participant_controller_observer.h"

namespace vi {
    class IParticipant;
}

class ParticipantEventAdapter : public QObject, public vi::IParticipantControllerObserver, public std::enable_shared_from_this<ParticipantEventAdapter>
{
    Q_OBJECT
public:
    explicit ParticipantEventAdapter(QObject *parent = nullptr);

protected:
    void onCreateParticipant(std::shared_ptr<vi::IParticipant> participant) override;

    void onUpdateParticipant(std::shared_ptr<vi::IParticipant> participant) override;

    void onRemoveParticipant(std::shared_ptr<vi::IParticipant> participant) override;

signals:
    void participantCreated(std::shared_ptr<vi::IParticipant> participant);

    void participantUpdated(std::shared_ptr<vi::IParticipant> participant);

    void participantRemoved(std::shared_ptr<vi::IParticipant> participant);
};

