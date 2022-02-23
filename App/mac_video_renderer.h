/**
 * This file is part of janus_client project.
 * Author:    Jackie Ou
 * Created:   2020-10-01
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

signals:
    void frameArrived(const webrtc::VideoFrame& frame);

private slots:
    void onFrameArrived(const webrtc::VideoFrame& frame);

private:
    void resizeVideo(size_t width, size_t height);

    void resizeViewport();

private:
	std::shared_ptr<webrtc::VideoFrame> _cacheFrame;

    bool _inited = false;

private:
    uint8_t* buffer_;
    GLuint texture_;
    int32_t width_, height_, buffer_size_;

    QSize _canvaSize;

};
