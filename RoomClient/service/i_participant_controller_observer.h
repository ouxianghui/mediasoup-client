#pragma once

#include <memory>

namespace vi {

class IParticipant;

class IParticipantControllerObserver {
public:
    virtual ~IParticipantControllerObserver() = default;

    virtual void onCreateParticipant(std::shared_ptr<IParticipant> participant) = 0;

    virtual void onUpdateParticipant(std::shared_ptr<IParticipant> participant) = 0;

    virtual void onRemoveParticipant(std::shared_ptr<IParticipant> participant) = 0;
};

}
