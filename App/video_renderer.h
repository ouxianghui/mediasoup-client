
#pragma once

#include <memory>
#include "api/video/video_sink_interface.h"
#include "api/video/video_frame.h"
#include <QTimer>
#include <mutex>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include "blockingconcurrentqueue.h"

class VideoShader;
class I420TextureCache;
class QTimer;

class VideoRenderer
    : public QOpenGLWidget
    , public QOpenGLFunctions
    , public rtc::VideoSinkInterface<webrtc::VideoFrame>
    , public std::enable_shared_from_this<VideoRenderer>
{
    Q_OBJECT

public:
    VideoRenderer(QWidget *parent);

    ~VideoRenderer();

    void init();

    void destroy();

    void clear();

    void reset();

protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    void OnFrame(const webrtc::VideoFrame& frame) override;

    void resizeEvent(QResizeEvent *e) override;

private slots:
    void cleanup();

    void onRendering();

private:
    std::shared_ptr<VideoShader> _videoShader;

    std::shared_ptr<I420TextureCache> _i420TextureCache;

    std::shared_ptr<webrtc::VideoFrame> _cacheFrame;

    QTimer* _renderingTimer;

    moodycamel::BlockingConcurrentQueue<std::shared_ptr<webrtc::VideoFrame>> _frameQ;

    bool _locked = false;

    //FILE* _fp = nullptr;
};
