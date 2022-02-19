#pragma once

#include <QObject>
#include <memory>
#include "service/i_room_client_observer.h"
#include "api/media_stream_interface.h"

class RoomEventAdapter : public QObject, public vi::IRoomClientObserver, public std::enable_shared_from_this<RoomEventAdapter>
{
    Q_OBJECT
public:
    explicit RoomEventAdapter(QObject *parent = nullptr);

protected:
    void onRoomState(vi::RoomState state) override;

signals:
    void roomStateChanged(vi::RoomState state);
};
