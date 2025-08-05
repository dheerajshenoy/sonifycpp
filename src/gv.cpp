/*
 *
 *   Contains functions that are used to draw the line, circle or path on the
 * graphicsview along with rendering the image
 *
 */

#include "gv.hpp"

#include <qnamespace.h>

GV::GV(QWidget *parent) noexcept : QGraphicsView(parent)
{
    this->setScene(m_scene);
    m_scene->addItem(m_pi);
    this->setAcceptDrops(true);
    this->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    this->setResizeAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
    this->show();
}

// Get the current location in the audiodata array
void
GV::setAudioIndex(const int &index) noexcept
{
    m_audio_index = index;
    emit audioIndexSet();
}

// Function to handle draw path mode
void
GV::setDrawPathMode(const bool &t) noexcept
{
    m_draw_path_mode = t;

    if (m_draw_path_mode)
    {
        m_pathDrawnPixelsPos.clear();
        if (!m_path_item)
        {
            m_path_item = new QGraphicsPathItem();
            m_path_item->setPen(QPen(Qt::red, 2));
            m_scene->addItem(m_path_item);
        }
    }
}

// Method to set the traverse mode, so as to prepare for the animation
void
GV::setTraverse(const Traverse &t) noexcept
{
    if (m_traverse == t)
        return;

    m_traverse = t;

    // Clean the canvas
    if (m_li && m_scene)
    {
        m_scene->removeItem(m_li);
        delete m_li;
        m_li = nullptr;
    }
    if (m_ci && m_scene)
    {
        m_scene->removeItem(m_ci);
        delete m_ci;
        m_ci = nullptr;
    }

    if (m_pathi && m_scene)
    {
        m_scene->removeItem(m_pathi);
        delete m_pathi;
        m_pathi = nullptr;
    }

    switch (t)
    {
        case Traverse::LEFT_TO_RIGHT:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem(m_obj_color);
                m_scene->addItem(m_li);
            }

            connect(this, &GV::audioIndexSet, m_li, [&]()
            { m_li->setLine(m_audio_index, 0, m_audio_index, m_height); });
            break;

        case Traverse::RIGHT_TO_LEFT:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem(m_obj_color);
                m_scene->addItem(m_li);
                qDebug() << "DD";
            }
            connect(this, &GV::audioIndexSet, m_li, [&]()
            {
                m_li->setLine(m_width - m_audio_index, 0,
                              m_width - m_audio_index, m_height);
            });
            break;

        case Traverse::TOP_TO_BOTTOM:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem(m_obj_color);
                m_scene->addItem(m_li);
            }
            connect(this, &GV::audioIndexSet, m_li, [&]()
            { m_li->setLine(0, m_audio_index, m_width, m_audio_index); });
            break;

        case Traverse::BOTTOM_TO_TOP:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem(m_obj_color);
                m_scene->addItem(m_li);
            }
            connect(this, &GV::audioIndexSet, m_li, [&]()
            {
                m_li->setLine(0, m_height - m_audio_index, m_width,
                              m_height - m_audio_index);
            });
            break;

        case Traverse::CIRCLE_OUTWARDS:
            if (m_ci)
                disconnect(m_ci, 0, 0, 0);

            if (!m_ci)
            {
                m_ci = new AnimatedCircleItem(m_obj_color);
                m_ci->setImageHeight(m_height);
                m_ci->setImageWidth(m_width);
                m_ci->setCenter();
                m_scene->addItem(m_ci);
            }
            connect(this, &GV::audioIndexSet, m_ci,
                    [&]() { m_ci->setRadius(m_audio_index); });
            break;

        case Traverse::CIRCLE_INWARDS:
            if (m_ci)
                disconnect(m_ci, 0, 0, 0);

            if (!m_ci)
            {
                m_ci = new AnimatedCircleItem(m_obj_color);
                m_ci->setImageHeight(m_height);
                m_ci->setImageWidth(m_width);
                m_ci->setCenter();
                m_scene->addItem(m_ci);
            }
            connect(this, &GV::audioIndexSet, m_ci,
                    [&]() { m_ci->setRadius(m_sqrt - m_audio_index); });
            break;

        case Traverse::CLOCKWISE:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem(m_obj_color);
                m_li->setImageHeight(m_height);
                m_li->setImageWidth(m_width);
                m_li->setLength();
                m_scene->addItem(m_li);
            }
            connect(this, &GV::audioIndexSet, m_li,
                    [&]() { m_li->setAngle(m_audio_index); });
            break;

        case Traverse::ANTICLOCKWISE:
            if (m_li)
                disconnect(m_li, 0, 0, 0);

            if (!m_li)
            {
                m_li = new AnimatedLineItem(m_obj_color);
                m_li->setImageHeight(m_height);
                m_li->setImageWidth(m_width);
                m_li->setLength();
                m_scene->addItem(m_li);
            }
            connect(this, &GV::audioIndexSet, m_li,
                    [&]() { m_li->setAngle(360 - m_audio_index); });
            break;

        case Traverse::PATH:
            if (m_pathi)
                disconnect(m_pathi, 0, 0, 0);

            if (!m_pathi)
            {
                m_pathi = new AnimatedPathItem(m_obj_color);
                m_scene->addItem(m_pathi);
            }
            connect(this, &GV::audioIndexSet, m_pathi, [&]()
            { m_pathi->setPos(m_pathDrawnPixelsPos.at(m_audio_index)); });
    }
}

void
GV::setDuration(const double &s) noexcept
{
    m_duration_s = s;
}

// Set the image onto the graphicsview
void
GV::setPixmap(const QPixmap &pix) noexcept
{
    if (!pix)
        return;

    m_pi->setPixmap(pix);
    /*this->fitInView(m_pi);*/
    centerOn(m_pi);
    m_scene->setSceneRect(pix.rect());

    m_width  = pix.width();
    m_height = pix.height();

    m_sqrt = sqrt(m_width * m_width / 4.0 + m_height * m_height / 4.0);
}

QPixmap
GV::getPixmap() noexcept
{
    return m_pi->pixmap();
}

// Reset the positions of the object drawn on the graphicsview
void
GV::reset() noexcept
{
    if (m_li)
        m_li->reset();
    if (m_ci)
        m_ci->reset();
}

void
GV::mousePressEvent(QMouseEvent *e) noexcept
{
    if (e->button() == Qt::MouseButton::LeftButton)
    {
        if (m_draw_path_mode)
        {
            auto pos = mapToScene(e->pos());

            QPointF s = pos;
            m_painter_path.moveTo(s);
            m_path_item->setPath(m_painter_path);
            m_pathDrawnPixelsPos.push_back(s);
            return;
        }
        emit pixelClick(mapToScene(e->pos()));
    }

    QGraphicsView::mousePressEvent(e);
}

void
GV::mouseReleaseEvent(QMouseEvent *e) noexcept
{
    if (e->button() == Qt::MouseButton::LeftButton)
    {
        if (m_draw_path_mode)
        {
            setDrawPathMode(false);
            emit drawPathFinished();
        }
    }
    QGraphicsView::mouseReleaseEvent(e);
}

void
GV::mouseMoveEvent(QMouseEvent *e) noexcept
{
    if (m_draw_path_mode)
    {

        auto pos  = mapToScene(e->pos());
        auto posx = pos.x();
        auto posy = pos.y();
        if (posx >= 0 && posx < m_pi->pixmap().width() && posy >= 0
            && posy < m_pi->pixmap().height())
        {
            QPointF s = mapToScene(e->pos());
            m_painter_path.lineTo(s);
            m_path_item->setPath(m_painter_path);
            m_pathDrawnPixelsPos.emplace_back(s);
        }
    }
    QGraphicsView::mouseMoveEvent(e);
}

GV::~GV() {}

QVector<QPointF>
GV::getPathDrawnPos() noexcept
{
    return m_pathDrawnPixelsPos;
}

// Function to handle drag and drop event to open the file when it is dropped
// into the graphicsview
void
GV::dropEvent(QDropEvent *e) noexcept
{
    QString droppedFilePath = e->mimeData()->urls()[0].toLocalFile();
    e->acceptProposedAction();
    emit dropFile(droppedFilePath);
}

void
GV::dragEnterEvent(QDragEnterEvent *e) noexcept
{
    if (e->mimeData()->hasUrls())
    {
        e->setAccepted(true);
        this->update();
    }
}

void
GV::dragMoveEvent(QDragMoveEvent *e) noexcept
{
}

// Function to set the color of "object" (line or circle or path) drawn over the
// image
void
GV::setObjColor(const QString &color) noexcept
{
    if (QColor::isValidColorName(color))
        m_obj_color = QColor(color);
}

void
GV::clearDrawPath() noexcept
{
    if (m_path_item)
    {
        m_scene->removeItem(m_path_item);
        delete m_path_item;
        m_path_item = nullptr;

        m_scene->removeItem(m_pathi);
        delete m_pathi;
        m_pathi = nullptr;
        m_painter_path.clear();
    }
}

void
GV::clearPixmap() noexcept
{
    m_pi->setPixmap(QPixmap());
}

void
GV::wheelEvent(QWheelEvent *e) noexcept
{
    if (e->modifiers() & Qt::Modifier::CTRL)
    {
        int angle    = e->angleDelta().y();
        float factor = (angle > 0) ? 1.15 : 1 / 1.15;
        scale(factor, factor);
    }
    else
        QGraphicsView::wheelEvent(e);
}
