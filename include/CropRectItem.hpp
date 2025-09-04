#pragma once

#include <QtWidgets/QGraphicsObject>
#include <QtGui/QMouseEvent>
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtGui/QPainterPath> // Added for shape()

class CropRectItem : public QGraphicsObject {
    Q_OBJECT

public:
    enum HandleType {
        None,
        TopLeft,
        Top,
        TopRight,
        Right,
        BottomRight,
        Bottom,
        BottomLeft,
        Left
    };

    explicit CropRectItem(const QRectF& rect, QGraphicsItem* parent = nullptr);

    // Override boundingRect and shape for collision detection and painting
    QRectF boundingRect() const override;
    QPainterPath shape() const override; // Added override

    // Custom rect accessors
    QRectF rect() const;
    void setRect(const QRectF& r);

    void setKeepAspectRatio(bool keep);
    bool keepAspectRatio() const { return m_keepAspectRatio; }

signals:
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();
    void aspectRatioChanged(bool);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override; // Added override
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    HandleType getHandleAt(const QPointF& pos);
    QRectF getHandleRect(HandleType handle);
    void updateCursor(HandleType handle);

    HandleType currentHandle;
    QPointF lastMousePos;
    qreal handleSize;
    QRectF m_rect; // Store the rectangle as a member variable
    bool m_keepAspectRatio; // New member to control aspect ratio

    // Store previous position and size to detect changes for custom signals
    qreal previousX;
    qreal previousY;
    qreal previousWidth;
    qreal previousHeight;
};