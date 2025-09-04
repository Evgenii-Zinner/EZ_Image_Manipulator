#include "CropRectItem.hpp"
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QApplication>

CropRectItem::CropRectItem(const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsObject(parent), currentHandle(None), handleSize(8.0) // Size of the resize handles
      ,
      m_rect(rect) // Initialize member rectangle
      ,
      previousX(rect.x()) // Initialize previous values for custom signals
      ,
      previousY(rect.y()), previousWidth(rect.width()), previousHeight(rect.height()), m_keepAspectRatio(false) // Initialize new member
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true); // Enable hover events to change cursor
    setPos(rect.topLeft());     // Set the item's position to the top-left of the rect
}

QRectF CropRectItem::boundingRect() const
{
    // Return the bounding rectangle including handles
    qreal halfHandle = handleSize / 2.0;
    return m_rect.adjusted(-halfHandle, -halfHandle, halfHandle, halfHandle);
}

QPainterPath CropRectItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

QRectF CropRectItem::rect() const
{
    return m_rect;
}

void CropRectItem::setRect(const QRectF &r)
{
    if (m_rect == r)
        return;              // Avoid unnecessary updates
    prepareGeometryChange(); // Notify scene of impending geometry change
    m_rect = r;
    update(); // Request repaint

    // Emit custom signals if geometry has changed
    if (m_rect.x() != previousX)
    {
        emit xChanged();
        previousX = m_rect.x();
    }
    if (m_rect.y() != previousY)
    {
        emit yChanged();
        previousY = m_rect.y();
    }
    if (m_rect.width() != previousWidth)
    {
        emit widthChanged();
        previousWidth = m_rect.width();
    }
    if (m_rect.height() != previousHeight)
    {
        emit heightChanged();
        previousHeight = m_rect.height();
    }
}

void CropRectItem::setKeepAspectRatio(bool keep)
{
    if (m_keepAspectRatio != keep)
    {
        m_keepAspectRatio = keep;
        emit aspectRatioChanged(m_keepAspectRatio);
    }
}

void CropRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        currentHandle = getHandleAt(event->pos());
        if (currentHandle == None)
        {
            // If not on a handle, assume moving the item
            QGraphicsObject::mousePressEvent(event);
        }
        lastMousePos = event->pos();
    }
}

void CropRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (currentHandle != None)
    {
        qreal dx = event->pos().x() - lastMousePos.x();
        qreal dy = event->pos().y() - lastMousePos.y();

        QRectF newRect = m_rect; // Use member rectangle

        qreal originalAspectRatio = m_rect.width() / m_rect.height();

        switch (currentHandle)
        {
        case TopLeft:
            newRect.setTopLeft(newRect.topLeft() + QPointF(dx, dy));
            if (m_keepAspectRatio)
            {
                if (qAbs(dx) > qAbs(dy))
                {
                    newRect.setHeight(newRect.width() / originalAspectRatio);
                }
                else
                {
                    newRect.setWidth(newRect.height() * originalAspectRatio);
                }
            }
            break;
        case Top:
            newRect.setTop(newRect.top() + dy);
            if (m_keepAspectRatio)
            {
                newRect.setWidth(newRect.height() * originalAspectRatio);
            }
            break;
        case TopRight:
            newRect.setTopRight(newRect.topRight() + QPointF(dx, dy));
            if (m_keepAspectRatio)
            {
                if (qAbs(dx) > qAbs(dy))
                {
                    newRect.setHeight(newRect.width() / originalAspectRatio);
                }
                else
                {
                    newRect.setWidth(newRect.height() * originalAspectRatio);
                }
            }
            break;
        case Right:
            newRect.setRight(newRect.right() + dx);
            if (m_keepAspectRatio)
            {
                newRect.setHeight(newRect.width() / originalAspectRatio);
            }
            break;
        case BottomRight:
            newRect.setBottomRight(newRect.bottomRight() + QPointF(dx, dy));
            if (m_keepAspectRatio)
            {
                if (qAbs(dx) > qAbs(dy))
                {
                    newRect.setHeight(newRect.width() / originalAspectRatio);
                }
                else
                {
                    newRect.setWidth(newRect.height() * originalAspectRatio);
                }
            }
            break;
        case Bottom:
            newRect.setBottom(newRect.bottom() + dy);
            if (m_keepAspectRatio)
            {
                newRect.setWidth(newRect.height() * originalAspectRatio);
            }
            break;
        case BottomLeft:
            newRect.setBottomLeft(newRect.bottomLeft() + QPointF(dx, dy));
            if (m_keepAspectRatio)
            {
                if (qAbs(dx) > qAbs(dy))
                {
                    newRect.setHeight(newRect.width() / originalAspectRatio);
                }
                else
                {
                    newRect.setWidth(newRect.height() * originalAspectRatio);
                }
            }
            break;
        case Left:
            newRect.setLeft(newRect.left() + dx);
            if (m_keepAspectRatio)
            {
                newRect.setHeight(newRect.width() / originalAspectRatio);
            }
            break;
        case None:
            break;
        }
        setRect(newRect.normalized()); // Normalize and update member rectangle
        lastMousePos = event->pos();
    }
    else
    {
        QGraphicsObject::mouseMoveEvent(event);
    }
    updateCursor(getHandleAt(event->pos())); // Update cursor on move
}

void CropRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    currentHandle = None;
    QGraphicsObject::mouseReleaseEvent(event);
    updateCursor(None); // Reset cursor
}

void CropRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Draw the rectangle itself
    painter->setPen(QPen(Qt::white, 2, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(m_rect); // Draw member rectangle

    // Paint resize handles
    painter->setBrush(Qt::white);
    painter->setPen(Qt::black);

    qreal halfHandle = handleSize / 2.0;

    // Top-left
    painter->drawRect(m_rect.topLeft().x() - halfHandle, m_rect.topLeft().y() - halfHandle, handleSize, handleSize);
    // Top-middle
    painter->drawRect(m_rect.center().x() - halfHandle, m_rect.top() - halfHandle, handleSize, handleSize);
    // Top-right
    painter->drawRect(m_rect.topRight().x() - halfHandle, m_rect.topRight().y() - halfHandle, handleSize, handleSize);
    // Right-middle
    painter->drawRect(m_rect.right() - halfHandle, m_rect.center().y() - halfHandle, handleSize, handleSize);
    // Bottom-right
    painter->drawRect(m_rect.bottomRight().x() - halfHandle, m_rect.bottomRight().y() - halfHandle, handleSize, handleSize);
    // Bottom-middle
    painter->drawRect(m_rect.center().x() - halfHandle, m_rect.bottom() - halfHandle, handleSize, handleSize);
    // Bottom-left
    painter->drawRect(m_rect.bottomLeft().x() - halfHandle, m_rect.bottomLeft().y() - halfHandle, handleSize, handleSize);
    // Left-middle
    painter->drawRect(m_rect.left() - halfHandle, m_rect.center().y() - halfHandle, handleSize, handleSize);
}

CropRectItem::HandleType CropRectItem::getHandleAt(const QPointF &pos)
{
    qreal halfHandle = handleSize / 2.0;
    QRectF currentRect = m_rect; // Use member rectangle

    if (QRectF(currentRect.topLeft().x() - halfHandle, currentRect.topLeft().y() - halfHandle, handleSize, handleSize).contains(pos))
        return TopLeft;
    if (QRectF(currentRect.center().x() - halfHandle, currentRect.top() - halfHandle, handleSize, handleSize).contains(pos))
        return Top;
    if (QRectF(currentRect.topRight().x() - halfHandle, currentRect.topRight().y() - halfHandle, handleSize, handleSize).contains(pos))
        return TopRight;
    if (QRectF(currentRect.right() - halfHandle, currentRect.center().y() - halfHandle, handleSize, handleSize).contains(pos))
        return Right;
    if (QRectF(currentRect.bottomRight().x() - halfHandle, currentRect.bottomRight().y() - halfHandle, handleSize, handleSize).contains(pos))
        return BottomRight;
    if (QRectF(currentRect.center().x() - halfHandle, currentRect.bottom() - halfHandle, handleSize, handleSize).contains(pos))
        return Bottom;
    if (QRectF(currentRect.bottomLeft().x() - halfHandle, currentRect.bottomLeft().y() - halfHandle, handleSize, handleSize).contains(pos))
        return BottomLeft;
    if (QRectF(currentRect.left() - halfHandle, currentRect.center().y() - halfHandle, handleSize, handleSize).contains(pos))
        return Left;

    return None;
}

void CropRectItem::updateCursor(HandleType handle)
{
    Qt::CursorShape cursor = Qt::ArrowCursor;
    switch (handle)
    {
    case TopLeft:
    case BottomRight:
        cursor = Qt::SizeFDiagCursor;
        break;
    case Top:
    case Bottom:
        cursor = Qt::SizeVerCursor;
        break;
    case TopRight:
    case BottomLeft:
        cursor = Qt::SizeBDiagCursor;
        break;
    case Right:
    case Left:
        cursor = Qt::SizeHorCursor;
        break;
    case None:
        cursor = Qt::ArrowCursor; // Default cursor for moving
        break;
    }
    setCursor(cursor);
}

QVariant CropRectItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionHasChanged)
    {
        // Emit custom signals if geometry has changed
        if (pos().x() != previousX)
        {
            emit xChanged();
            previousX = pos().x();
        }
        if (pos().y() != previousY)
        {
            emit yChanged();
            previousY = pos().y();
        }
        if (m_rect.width() != previousWidth)
        {
            emit widthChanged();
            previousWidth = m_rect.width();
        }
        if (m_rect.height() != previousHeight)
        {
            emit heightChanged();
            previousHeight = m_rect.height();
        }
    }
    return QGraphicsObject::itemChange(change, value);
}
