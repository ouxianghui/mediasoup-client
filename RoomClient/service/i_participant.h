#pragma once

#include <memory>
#include <string>

namespace vi {

class IParticipant {
public:
    virtual ~IParticipant() = default;

    virtual std::string id() = 0;

    virtual std::string displayName() = 0;
};

}
