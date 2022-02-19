#include "room_event_adapter.h"

RoomEventAdapter::RoomEventAdapter(QObject *parent)
    : QObject{parent}
{

}

void RoomEventAdapter::onRoomState(vi::RoomState state)
{
    emit roomStateChanged(state);
}
