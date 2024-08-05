/**
 * This file is part of mediasoup_client project.
 * Author:    Jackie Ou
 * Created:   2021-11-01
 **/

#pragma once

#include <memory>
#include "api/video/video_sink_interface.h"
#include "api/video/video_frame.h"
#include <mutex>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class MacVideoRenderer
        : public QOpenGLWidget
        , public QOpenGLFunctions
        , public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
    Q_OBJECT

public:
    MacVideoRenderer(QWidget *parent);

    ~MacVideoRenderer();

    void init();

    void destroy();

protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    void OnFrame(const webrtc::VideoFrame& frame) override;

Q_SIGNALS:
    void frameArrived(const webrtc::VideoFrame& frame);

private Q_SLOTS:
    void onFrameArrived(const webrtc::VideoFrame& frame);

private:
    void resizeVideo(size_t width, size_t height);

    void resizeViewport();

private:
    std::shared_ptr<webrtc::VideoFrame> _cacheFrame;

    bool _inited = false;

private:
    uint8_t* _buffer;
    GLuint _texture;
    int32_t _width, _height, _bufferSize;

    QSize _canvaSize;

};
