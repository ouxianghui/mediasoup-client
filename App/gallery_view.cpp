/**
 * This file is part of mediasoup_client project.
 * Author:    Jackie Ou
 * Created:   2021-11-01
 **/

#include "gallery_view.h"
#include "ui_gallery_view.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QLabel>
//#include <WinSock2.h>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/timeb.h>
#endif


#define DRAG_WIDTH      160
#define DRAG_HEIGHT     120

#define DRAG_QSS    "border:3px groove #FF8C00"
#define RECT_QSS    "border:2px solid red"

namespace {
#ifdef _MSC_VER
    /* @see winsock2.h
    // Structure used in select() call, taken from the BSD file sys/time.h
    struct timeval {
        long    tv_sec;
        long    tv_usec;
    };
    */

    struct timezone {
        int tz_minuteswest; /* of Greenwich */
        int tz_dsttime;     /* type of dst correction to apply */
    };

    static inline int gettimeofday(struct timeval* tv, struct timezone* tz) {
        //改成使用chrono
        /*struct _timeb tb;
        _ftime(&tb);
        if (tv) {
            tv->tv_sec = (long)tb.time;
            tv->tv_usec = tb.millitm * 1000;
        }
        if (tz) {
            tz->tz_minuteswest = tb.timezone;
            tz->tz_dsttime = tb.dstflag;
        }*/
        return 0;
    }
#endif

    unsigned int gettick() {
#ifdef OS_WIN
        return GetTickCount();
#elif HAVE_CLOCK_GETTIME
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#else
        //改成使用chrono
        /*struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;*/
        return 0;
#endif
    }

    inline QRect adjustRect(QPoint pt1, QPoint pt2) {
        int x1 = qMin(pt1.x(), pt2.x());
        int x2 = qMax(pt1.x(), pt2.x());
        int y1 = qMin(pt1.y(), pt2.y());
        int y2 = qMax(pt1.y(), pt2.y());
        return QRect(QPoint(x1, y1), QPoint(x2, y2));
    }
}

GalleryView::GalleryView(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::GalleryView)
{
    ui->setupUi(this);
    initUI();
    _bStretch = false;
}

GalleryView::~GalleryView()
{
    delete ui;
}

void GalleryView::init()
{

}

void GalleryView::destroy()
{
    reset();
}

void GalleryView::initUI() {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    for (int i = 0; i < MV_STYLE_MAXNUM; ++i) {
        VideoCanvas* canvas = new VideoCanvas(this);
        canvas->init();
        canvas->setId(i + 1);
        _canvases.push_back(canvas);
        canvas->show();
    }

    setLayout(2, 2);

    _labelDrag = new QLabel(this);
    _labelDrag->setFixedSize(DRAG_WIDTH, DRAG_HEIGHT);
    _labelDrag->hide();
    _labelDrag->setStyleSheet(DRAG_QSS);

    _labelRect = new QLabel(this);
    _labelRect->hide();
    _labelRect->setStyleSheet(RECT_QSS);
}

void GalleryView::setLayout(int row, int col) {
    saveLayout();
    _table.init(row, col);
    updateUI();
}

void GalleryView::mergeCells(int lt, int rb) {
#if 1
    // find first non-stop player as lt
    for (int i = lt; i <= rb; ++i) {
        VideoCanvas* canvas = getCanvas(i);
        if (canvas && canvas->status() != VideoCanvas::DETACHED) {
            exchangeCells(canvas, getCanvas(lt));
            break;
        }
    }
#endif

    saveLayout();

    _table.merge(lt, rb);

    updateUI();
}

void GalleryView::exchangeCells(VideoCanvas* canvas1, VideoCanvas* canvas2) {
    qDebug("exchange %d<=>%d", canvas1->id(), canvas2->id());

    QRect rcTmp = canvas1->geometry();
    int idTmp = canvas1->id();

    canvas1->setGeometry(canvas2->geometry());
    canvas1->setId(canvas2->id());

    canvas2->setGeometry(rcTmp);
    canvas2->setId(idTmp);
}

VideoCanvas* GalleryView::getCanvas(int id) {
    for (int i = 0; i < _canvases.size(); ++i) {
        VideoCanvas* canvas = (VideoCanvas*)_canvases[i];
        if (canvas->id() == id) {
            return canvas;
        }
    }
    return NULL;
}

VideoCanvas* GalleryView::getCanvas(QPoint pt) {
    for (int i = 0; i < _canvases.size(); ++i) {
        QWidget* widget = _canvases[i];
        if (widget->isVisible() && widget->geometry().contains(pt)) {
            return (VideoCanvas*)widget;
        }
    }
    return NULL;
}

VideoCanvas* GalleryView::getIdleCanvas() {
    for (int i = 0; i < _canvases.size(); ++i) {
        VideoCanvas* canvas = (VideoCanvas*)_canvases[i];
        if (canvas->isVisible() && canvas->status() == VideoCanvas::DETACHED) {
            return canvas;
        }
    }
    return NULL;
}

#define SEPARATOR_LINE_WIDTH 4

void GalleryView::updateUI() {
    int row = _table.row;
    int col = _table.col;
    if (row == 0 || col == 0) return;
    int cell_w = width() / col;
    int cell_h = height() / row;

    int margin_x = (width() - cell_w * col) / 2;
    int margin_y = (height() - cell_h * row) / 2;
    int x = margin_x;
    int y = margin_y;
    for (int i = 0; i < _canvases.size(); ++i) {
        _canvases[i]->hide();
    }

    int cnt = 0;
    TableCell cell;
    for (int r = 0; r < row; ++r) {
        for (int c = 0; c < col; ++c) {
            int id = r * col + c + 1;
            if (_table.getTableCell(id, cell)) {
                QWidget* widget = getCanvas(id);
                if (widget) {
                    widget->setGeometry(x, y, cell_w * cell.colspan() - SEPARATOR_LINE_WIDTH, cell_h * cell.rowspan() - SEPARATOR_LINE_WIDTH);
                    widget->show();
                    ++cnt;
                }
            }
            x += cell_w;
        }
        x = margin_x;
        y += cell_h;
    }

    _bStretch = (cnt == 1);
}

void GalleryView::resizeEvent(QResizeEvent* e) {
    updateUI();
}

void GalleryView::mousePressEvent(QMouseEvent* e) {
    _ptMousePress = e->pos();
    _tsMousePress = gettick();
}

void GalleryView::mouseReleaseEvent(QMouseEvent* e) {
    if (_action == EXCHANGE) {
        VideoCanvas* canvas1 = getCanvas(_ptMousePress);
        VideoCanvas* canvas2 = getCanvas(e->pos());
        if (canvas1 && canvas2 && canvas1 != canvas2) {
            exchangeCells(canvas1, canvas2);
        }
    }
    else if (_action == MERGE) {
        QRect rc = adjustRect(_ptMousePress, e->pos());
        VideoCanvas* canvas1 = getCanvas(rc.topLeft());
        VideoCanvas* canvas2 = getCanvas(rc.bottomRight());
        if (canvas1 && canvas2 && canvas1 != canvas2) {
            mergeCells(canvas1->id(), canvas2->id());
        }
    }

    _labelRect->setVisible(false);
    _labelDrag->setVisible(false);
    setCursor(Qt::ArrowCursor);
}

void GalleryView::mouseMoveEvent(QMouseEvent* e) {
    VideoCanvas* canvas = getCanvas(_ptMousePress);
    if (canvas == NULL) {
        return;
    }

    if (e->buttons() == Qt::LeftButton) {
        if (!_labelDrag->isVisible()) {
            if (gettick() - _tsMousePress < 300) return;
            _action = EXCHANGE;
            setCursor(Qt::OpenHandCursor);
            _labelDrag->setPixmap(canvas->grab().scaled(_labelDrag->size()));
            _labelDrag->setVisible(true);
        }
        if (_labelDrag->isVisible()) {
            _labelDrag->move(e->pos() - QPoint(_labelDrag->width() / 2, _labelDrag->height()));
        }
    }
    else if (e->buttons() == Qt::RightButton) {
        if (!_labelRect->isVisible()) {
            _action = MERGE;
            setCursor(Qt::CrossCursor);
            _labelRect->setVisible(true);
        }
        if (_labelRect->isVisible()) {
            _labelRect->setGeometry(adjustRect(_ptMousePress, e->pos()));
        }
    }
}

void GalleryView::mouseDoubleClickEvent(QMouseEvent* e) {
    VideoCanvas* canvas = getCanvas(e->pos());
    if (canvas) {
        stretch(canvas);
    }
}

void GalleryView::stretch(QWidget* widget) {
    if (_table.row == 1 && _table.col == 1) return;
    if (_bStretch) {
        restoreLayout();
        _bStretch = false;
    }
    else {
        saveLayout();
        for (int i = 0; i < _canvases.size(); ++i) {
            _canvases[i]->hide();
        }
        widget->setGeometry(rect());
        widget->show();
        _bStretch = true;
    }
}

void GalleryView::saveLayout() {
    _prevTable = _table;
}

void GalleryView::restoreLayout() {
    Table tmp = _table;
    _table = _prevTable;
    _prevTable = tmp;
    updateUI();
}

void GalleryView::attach(std::shared_ptr<vi::IParticipant> participant) {
    if (!participant) {
        return;
    }
    VideoCanvas* canvas = nullptr;
    for (int i = 0; i < _canvases.size(); ++i) {
        VideoCanvas* vc = (VideoCanvas*)_canvases[i];
        if (vc->pid() == participant->id()) {
            canvas = vc;
        }
    }

    if (!canvas) {
        canvas = getIdleCanvas();
        if (canvas == NULL) {
            QMessageBox::information(this, tr("Info"), tr("No spare canvas"));
        }
        else {
            canvas->attach(participant);
        }
    }
}


void GalleryView::detach(std::shared_ptr<vi::IParticipant> participant) {
    if (!participant) {
        return;
    }

    VideoCanvas* canvas = nullptr;

    for (int i = 0; i < _canvases.size(); ++i) {
        VideoCanvas* vc = (VideoCanvas*)_canvases[i];
        if (vc->pid() == participant->id()) {
            canvas = vc;
        }
    }

    if (canvas) {
        canvas->detach();
    }
}

void GalleryView::update(std::shared_ptr<vi::IParticipant> participant) {
    if (!participant) {
        return;
    }

    VideoCanvas* canvas = nullptr;

    for (int i = 0; i < _canvases.size(); ++i) {
        VideoCanvas* vc = (VideoCanvas*)_canvases[i];
        if (vc->pid() == participant->id()) {
            canvas = vc;
        }
    }

    if (canvas) {
        canvas->update(participant);
    }
}

void GalleryView::reset()
{
    for (int i = 0; i < _canvases.size(); ++i) {
        VideoCanvas* vc = (VideoCanvas*)_canvases[i];
        vc->detach();
    }
}
