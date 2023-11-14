/**
 * This file is part of mediasoup_client project.
 * Author:    Jackie Ou
 * Created:   2021-11-01
 **/

#ifndef GALLERY_VIEW_H
#define GALLERY_VIEW_H

#include <QFrame>
#include <vector>
#include <QLabel>
#include <QToolButton>
#include <QProgressBar>
#include <algorithm>
#include "api/media_stream_interface.h"
#include "api/scoped_refptr.h"
#include "mac_video_renderer.h"
#include "video_renderer.h"
#include "table.h"
#include "service/participant.h"

#define MV_STYLE_MAXNUM     64

 // F(id, row, col, label, image)
#define FOREACH_MV_STYLE(F) \
    F(MV_STYLE_1,  1, 1, " MV1",  ":/image/style1.png")     \
    F(MV_STYLE_2,  1, 2, " MV2",  ":/image/style2.png")     \
    F(MV_STYLE_4,  2, 2, " MV4",  ":/image/style4.png")     \
    F(MV_STYLE_9,  3, 3, " MV9",  ":/image/style9.png")     \
    F(MV_STYLE_16, 4, 4, " MV16", ":/image/style16.png")    \
    F(MV_STYLE_25, 5, 5, " MV25", ":/image/style25.png")    \
    F(MV_STYLE_36, 6, 6, " MV36", ":/image/style36.png")    \
    F(MV_STYLE_49, 7, 7, " MV49", ":/image/style49.png")    \
    F(MV_STYLE_64, 8, 8, " MV64",  ":/image/style64.png")   \

enum MV_STYLE {
#define ENUM_MV_STYLE(id, row, col, label, image) id,
    FOREACH_MV_STYLE(ENUM_MV_STYLE)
};

namespace Ui {
class GalleryView;
}

namespace webrtc {
	class VideoTrackInterface;
}

namespace vi {
    class IParticipant;
}

class VideoCanvas : public QWidget 
{
    Q_OBJECT
public:
    enum Status {
        ATTACHED,
        DETACHED,
    };

    VideoCanvas(QWidget* parent)
	: QWidget(parent)
    , _id(-1)
	, _track(nullptr)
	, _renderer(new VideoRenderer(this)) {
        _labelName = new QLabel(this);
        _buttonAudio = new QToolButton(this);
        _buttonVideo = new QToolButton(this);
        _progressBarVolume = new QProgressBar(this);
	}

    void init()  {
        _renderer->init();
        QFont font = QFont("Times", 10, QFont::Normal, false);
        _labelName->setFont(font);
        _labelName->setStyleSheet("color:red");
        _labelName->setAlignment(Qt::AlignmentFlag::AlignCenter);
        _labelName->setAutoFillBackground(true);

        _buttonAudio->setIcon(QIcon(":/app/resource/icons8-block-microphone-100.png"));

        _buttonVideo->setIcon(QIcon(":/app/resource/icons8-no-video-100.png"));

        _progressBarVolume->setRange(0, 99);
	}

	void destroy() {
		if (_renderer) {
            _renderer->destroy();
		}
        if (_track) {
            _track->RemoveSink(_renderer);
            _track = nullptr;
        }
        _status = Status::DETACHED;
	}

    void attach(std::shared_ptr<vi::IParticipant> participant) {
        if (!participant) {
            return;
        }

        if (_participant) {
            return;
        }

        _participant = participant;

        updateUI(participant);

        _status = Status::ATTACHED;
    }

    void update(std::shared_ptr<vi::IParticipant> participant) {
        if (!participant || !_participant) {
            return;
        }

        if (_participant->id() != participant->id()) {
            return;
        }

        _participant = participant;

        updateUI(participant);
    }

    void detach() {
        if (_renderer && _track) {
            _track->RemoveSink(_renderer);
            _track = nullptr;
        }

        _renderer->clear();

        _labelName->setText("");

        _buttonAudio->setIcon(QIcon(":/app/resource/icons8-block-microphone-100.png"));

        _buttonVideo->setIcon(QIcon(":/app/resource/icons8-no-video-100.png"));

        _progressBarVolume->setValue(0);

        _participant = nullptr;

        _status = Status::DETACHED;
    }

    int32_t id()  {
		return _id;
	}

    void setId(int32_t id) {
        _id = id;
    }

    Status status() {
        return _status;
    }

    std::string pid() {
        return _participant ? _participant->id() : "";
    }

private:
    void updateUI(std::shared_ptr<vi::IParticipant> participant) {
        if (!participant) {
            return;
        }

        _labelName->setText(participant->displayName().c_str());

        if (participant->isAudioMuted()) {
            _buttonAudio->setIcon(QIcon(":/app/resource/icons8-block-microphone-100.png"));
        }
        else {
            _buttonAudio->setIcon(QIcon(":/app/resource/icons8-microphone-100.png"));
            _progressBarVolume->setValue(100 + participant->speakingVolume());
        }

        if (participant->isVideoMuted()) {
            _buttonVideo->setIcon(QIcon(":/app/resource/icons8-no-video-100.png"));
        }
        else {
            _buttonVideo->setIcon(QIcon(":/app/resource/icons8-video-call-100.png"));
        }

        auto trackMap = participant->getVideoTracks();
        if (!trackMap.empty()) {

            auto item = trackMap.begin();

            rtc::scoped_refptr<webrtc::VideoTrackInterface> track((webrtc::VideoTrackInterface*)item->second.get());

            if (!_track) {
                _track = track;
                if (_track) {
                    rtc::VideoSinkWants wants;
                    _track->AddOrUpdateSink(_renderer, wants);
                    _renderer->reset();
                }
            }
            else {
                if (_track != track) {
                    _track->RemoveSink(_renderer);
                    _track = track;
                    if (_track) {
                        rtc::VideoSinkWants wants;
                        _track->AddOrUpdateSink(_renderer, wants);
                        _renderer->reset();
                    }
                }
            }
        }

        if (participant->isVideoMuted()) {
            if (_track) {
                _track->RemoveSink(_renderer);
                _track = nullptr;
            }
            _renderer->clear();
        }
        else {
            _renderer->reset();
        }
    }

protected:
    void resizeEvent(QResizeEvent* event) override {

        _renderer->setGeometry(this->rect());

        _labelName->setGeometry(QRect(this->rect().left() + 10, this->rect().bottom() - 35, 100, 30));

        _buttonAudio->setGeometry(QRect(this->rect().left() + 10 + 110, this->rect().bottom() - 35, 30, 30));

        _buttonVideo->setGeometry(QRect(this->rect().left() + 10 + 150, this->rect().bottom() - 35, 30, 30));

        _progressBarVolume->setGeometry(QRect(this->rect().left() + 10 + 190, this->rect().bottom() - 30, 100, 20));

        QWidget::resizeEvent(event);
    }


private:
    int32_t _id;

    rtc::scoped_refptr<webrtc::VideoTrackInterface> _track;

    VideoRenderer* _renderer;

    Status _status = Status::DETACHED;

    std::shared_ptr<vi::IParticipant> _participant;

    QLabel* _labelName;
    QToolButton* _buttonAudio;
    QToolButton* _buttonVideo;
    QProgressBar* _progressBarVolume;

};

class GalleryView : public QFrame
{
    Q_OBJECT

public:
    explicit GalleryView(QWidget *parent = nullptr);

    ~GalleryView();

    enum Action {
        STRETCH,
        EXCHANGE,
        MERGE,
    };

public:
    void init();

    void destroy();

    void attach(std::shared_ptr<vi::IParticipant> participant);

    void detach(std::shared_ptr<vi::IParticipant> participant);

    void update(std::shared_ptr<vi::IParticipant> participant);

    void setLayout(int row, int col);

    void reset();

protected slots:

    void saveLayout();

    void restoreLayout();

    void mergeCells(int lt, int rb);

    void exchangeCells(VideoCanvas* canvas1, VideoCanvas* canvas2);

    void stretch(QWidget* widget);

protected:
    void initUI();

    void updateUI();

    void resizeEvent(QResizeEvent* e) override;

    void mousePressEvent(QMouseEvent* e) override;

    void mouseReleaseEvent(QMouseEvent* e) override;

    void mouseMoveEvent(QMouseEvent* e) override;

    void mouseDoubleClickEvent(QMouseEvent* e) override;

    VideoCanvas* getCanvas(int id);

    VideoCanvas* getCanvas(QPoint pt);

    VideoCanvas* getIdleCanvas();

private:
    Ui::GalleryView* ui;

    Table _table;
    Table _prevTable;

    QVector<QWidget*> _canvases;

    QPoint _ptMousePress;
    uint64_t _tsMousePress;

    Action _action;
    bool _bStretch;

    QLabel* _labelRect;
    QLabel* _labelDrag;
};

#endif // GALLERY_VIEW_H
