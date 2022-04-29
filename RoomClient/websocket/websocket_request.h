#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "logger/u_logger.h"
#include "i_transport.h"
#include "asio.hpp"

namespace vi {

using ResolveCallback = std::function<void(const std::string& json)>;
using RejectCallback = std::function<void(int32_t errorCode, const std::string& errorInfo)>;

class WebsocketRequest {
public:
    WebsocketRequest(int64_t id, uint32_t timeout)
        : _id(id)
        , _timeout(timeout)
        , _timer(_context) {
        DLOG("request.id = {}", _id);
    }

    ~WebsocketRequest() {
        _timer.cancel();
        //_context.stop();
        if (_thread.joinable()) {
            _thread.join();
        }
    }

    int32_t id() {
        return _id;
    }

    void setText(const std::string& text) {
        _text = text;
    }

    const std::string& text() {
        return _text;
    }

    void setData(const std::vector<uint8_t>& data) {
        _data = data;
    }

    const std::vector<uint8_t>& data() {
        return _data;
    }

    const asio::steady_timer& timer() {
        return _timer;
    }

    void ticktock() {
        _timer.expires_after(std::chrono::milliseconds(_timeout));
        _timer.async_wait(std::bind(&WebsocketRequest::timeout, this));
        _thread = std::thread([this](){
            _context.run();
            DLOG("--> stoped");
        });
    }

    void setResolveCallback(ResolveCallback resolve) {
        _resolve = resolve;
    }

    void setRejectCallback(RejectCallback reject) {
        _reject = reject;
    }

    void resolve(const std::string& json) {
        _timer.cancel();
        if (_resolve) {
            _resolve(json);
        }
    }

    void reject(int32_t errorCode, const std::string& errorInfo) {
        _timer.cancel();
        if (_reject) {
            _reject(errorCode, errorInfo);
        }
    }

    void timeout() {
        _timer.cancel();
        if (_reject) {
            //_reject(1, "request timeout");
        }
    }

    void close() {
        _timer.cancel();
        if (_reject) {
            _reject(2, "connection closed");
        }
    }

private:
    int64_t _id = -1;
    std::string _text;
    std::vector<uint8_t> _data;
    ResolveCallback _resolve;
    RejectCallback _reject;
    uint32_t _timeout;
    asio::io_context _context;
    asio::steady_timer _timer;
    std::thread _thread;
};

}
