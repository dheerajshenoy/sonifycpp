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
    m_scene->addItem(m_ci);
    m_scene->addItem(m_li);
    this->setAcceptDrops(true);
    this->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    this->setResizeAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
    this->show();
}

// Get the current location in the audiodata array
void
GV::setAudioIndex(int index) noexcept
{
    m_audio_index = index;
    emit audioIndexSet();
}

// Function to handle draw path mode
void
GV::setDrawPathMode(bool enabled) noexcept
{
    m_draw_path_mode = enabled;

    if (enabled)
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
GV::setTraverse(const Traverse &mode) noexcept
{
    clearItems();

    auto clearItem = [&](QGraphicsItem *&item)
    {
        if (item)
        {
            if (item->scene() == m_scene)
                m_scene->removeItem(item);
            delete item;
            item = nullptr;
        }
    };

    switch (mode)
    {
        case Traverse::LEFT_TO_RIGHT:
        case Traverse::RIGHT_TO_LEFT:
        case Traverse::TOP_TO_BOTTOM:
        case Traverse::BOTTOM_TO_TOP:
        {
            m_li = new AnimatedLineItem(m_obj_color);
            m_scene->addItem(m_li);

            connect(this, &GV::audioIndexSet, m_li, [&]()
            {
                switch (mode)
                {
                    case Traverse::LEFT_TO_RIGHT:
                        m_li->setLine(m_audio_index, 0, m_audio_index,
                                      m_height);
                        break;

                    case Traverse::RIGHT_TO_LEFT:
                        m_li->setLine(m_width - m_audio_index, 0,
                                      m_width - m_audio_index, m_height);
                        break;

                    case Traverse::TOP_TO_BOTTOM:
                        m_li->setLine(0, m_audio_index, m_width, m_audio_index);
                        break;

                    case Traverse::BOTTOM_TO_TOP:
                        m_li->setLine(0, m_height - m_audio_index, m_width,
                                      m_height - m_audio_index);
                        break;

                    default:
                        break;
                }
            });

            break;
        }

        case Traverse::CIRCLE_OUTWARDS:
        case Traverse::CIRCLE_INWARDS:
        {
            m_ci = new AnimatedCircleItem(m_obj_color);
            m_ci->setImageHeight(m_height);
            m_ci->setImageWidth(m_width);
            m_ci->setCenter();
            m_scene->addItem(m_ci);

            connect(this, &GV::audioIndexSet, m_ci, [this, mode]()
            {
                int radius = (mode == Traverse::CIRCLE_OUTWARDS)
                                 ? m_audio_index
                                 : m_sqrt - m_audio_index;
                m_ci->setRadius(radius);
            });
            break;
        }

        case Traverse::CLOCKWISE:
        case Traverse::ANTICLOCKWISE:
        {
            m_li = new AnimatedLineItem(m_obj_color);
            m_li->setImageHeight(m_height);
            m_li->setImageWidth(m_width);
            m_li->setLength();
            m_scene->addItem(m_li);

            connect(this, &GV::audioIndexSet, m_li, [this, mode]()
            {
                m_li->setAngle(mode == Traverse::CLOCKWISE
                                   ? m_audio_index
                                   : 360 - m_audio_index);
            });
            break;
        }

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

        case Traverse::REGION:
        case Traverse::INSPECT:
            break;
    }
}

// Set the image onto the graphicsview
void
GV::setPixmap(const QPixmap &pix) noexcept
{
    if (pix.isNull())
        return;

    m_pi->setPixmap(pix);
    centerOn(m_pi);
    m_scene->setSceneRect(pix.rect());

    m_width  = pix.width();
    m_height = pix.height();
    m_sqrt   = std::sqrt(m_width * m_width / 4.0 + m_height * m_height / 4.0);
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
    if (e->button() == Qt::LeftButton)
    {
        QPointF scenePos = mapToScene(e->pos());

        if (m_draw_path_mode)
        {
            m_painter_path.moveTo(scenePos);
            m_path_item->setPath(m_painter_path);
            m_pathDrawnPixelsPos.push_back(scenePos);
            return;
        }

        emit pixelClick(scenePos);
    }

    QGraphicsView::mousePressEvent(e);
}

void
GV::mouseReleaseEvent(QMouseEvent *e) noexcept
{
    if (e->button() == Qt::LeftButton && m_draw_path_mode)
    {
        setDrawPathMode(false);
        emit drawPathFinished();
    }

    QGraphicsView::mouseReleaseEvent(e);
}

void
GV::mouseMoveEvent(QMouseEvent *e) noexcept
{
    if (m_draw_path_mode)
    {
        QPointF scenePos = mapToScene(e->pos());

        if (scenePos.x() >= 0 && scenePos.y() < m_width && scenePos.y() >= 0
            && scenePos.y() < m_height)
        {
            m_painter_path.lineTo(scenePos);
            m_path_item->setPath(m_painter_path);
            m_pathDrawnPixelsPos.emplace_back(scenePos);
        }
    }

    QGraphicsView::mouseMoveEvent(e);
}

// Function to handle drag and drop event to open the file when it is dropped
// into the graphicsview
void
GV::dropEvent(QDropEvent *e) noexcept
{
    if (!e->mimeData()->hasUrls())
        return;

    QString filePath = e->mimeData()->urls().first().toLocalFile();
    e->acceptProposedAction();
    emit dropFile(filePath);
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
    // No-op
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

    auto removeItemSafe = [&](QGraphicsItem *&item)
    {
        if (item)
        {
            m_scene->removeItem(item);
            delete item;
            item = nullptr;
        }
    };

    removeItemSafe(reinterpret_cast<QGraphicsItem *&>(m_path_item));
    removeItemSafe(reinterpret_cast<QGraphicsItem *&>(m_pathi));
    m_painter_path = QPainterPath();
    m_pathDrawnPixelsPos.clear();
}

void
GV::wheelEvent(QWheelEvent *e) noexcept
{
    if (e->modifiers() & Qt::ControlModifier)
    {
        float factor = (e->angleDelta().y() > 0) ? 1.15f : 1 / 1.15f;
        scale(factor, factor);
    }
    else
    {
        QGraphicsView::wheelEvent(e);
    }
}

void
GV::clearItems() noexcept
{
    for (QGraphicsItem *item : m_scene->items())
    {
        if (item != m_pi)
        {
            m_scene->removeItem(item);
            delete item;
        }
    }
}
