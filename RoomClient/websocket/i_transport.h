/**
 * This file is part of janus_client project.
 * Author:    Jackie Ou
 * Created:   2020-10-01
 **/

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace vi {

class ITransportObserver;
class ITransport {
public:
    virtual ~ITransport() {}

    virtual void init() = 0;

    virtual void destroy() = 0;

    virtual void addObserver(std::shared_ptr<ITransportObserver> observer) = 0;

    virtual void removeObserver(std::shared_ptr<ITransportObserver> observer) = 0;

    virtual void connect(const std::string& url, const std::string& subprotocol) = 0;

    virtual void disconnect() = 0;

    virtual void send(const std::string& text) = 0;

    virtual void send(const std::vector<uint8_t>& data) = 0;
};

}
