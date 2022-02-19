#pragma once

#include <string>

namespace webrtc {
    class MediaStreamTrackInterface;
}
namespace vi {

enum class RoomState {
    UNKNOWN = 0,
    CONNECTING,
    CONNECTED,
    CLOSED
};

class IRoomClientObserver {
public:
    virtual ~IRoomClientObserver() = default;

    virtual void onRoomState(RoomState state) = 0;
};

}
