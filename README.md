## mediasoup-client
* C++ client side library for building mediasoup based applications
* macOS

## Dependencies

* [Qt6](http://download.qt.io/archive/qt/) 
* [asio](https://github.com/chriskohlhoff/asio) 
* [websocketpp](https://github.com/zaphoyd/websocketpp) 
* [rapidjson](https://github.com/Tencent/rapidjson.git) 
* [spdlog](https://github.com/gabime/spdlog)
* [WebRTC](https://github.com/shiguredo-webrtc-build/webrtc-build/releases/download/m99.4844.1.0/webrtc.macos_x86_64.tar.gz)

* *Note:* 
* asio/websocketpp/rapidjson/spdlog have joined this repository as submodules

## IDE
* Xcode
* Run `/Applications/Qt/6.2.3/macos/bin/qmake ./App/App.pro -spec macx-xcode` and `/Applications/Qt/6.2.3/macos/bin/qmake ./RoomClient/RoomClient.pro -spec macx-xcode` to generate Xcode project.

* Qt Creator
* Open Mediasoup.pro using Qt Creator.
