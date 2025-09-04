#pragma once

#include <QtWidgets/QMainWindow>
#include <QtGui/QImage>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QDockWidget>
#include <QtCore/QString>
#include <QtWidgets/QGraphicsRectItem>
#include <QtCore/QRectF>
#include <QtCore/QList>
#include <QtCore/QSize>
#include <memory>
#include <QtCore/QPointF>
#include <CropRectItem.hpp>
#include <ImageTool.hpp> // New include

class ImageEditor : public QMainWindow {
    Q_OBJECT

signals:
    void imageChanged(); // New signal

public:
    explicit ImageEditor(QWidget* parent = nullptr);
    ~ImageEditor() override;

    // New public methods for tools to interact with
    QGraphicsScene* getGraphicsScene() const { return scene; }
    QImage getCurrentImage() const { return currentImage; }
    void setCurrentImage(const QImage& image) { currentImage = image; emit imageChanged(); } // Emit signal here
    void updateDisplay(); // Already exists, but ensure it's public
    qreal getZoomFactor() const { return zoomFactor; }
    QString getCurrentFilePath() const { return m_currentFilePath; }
    void setCurrentFilePath(const QString& path) { m_currentFilePath = path; }
    void setZoomFactor(qreal factor) { zoomFactor = factor; }
    QGraphicsView* getGraphicsView() const { return view; }

private slots:

private:
    void setupUI();
    void setupToolsDock();
    void updateTitle();
    bool saveWebP(const QString& filename, int quality = 90);
    bool maybeSave();
    void setImage(const QImage& newImage);
    QSize calculateZoomedSize() const;

    // UI Elements
    QGraphicsScene* scene;
    QGraphicsView* view;
    QDockWidget* toolsDock;

    // Image state
    QImage originalImage;
    QImage currentImage;
    QString m_currentFilePath;
    
    // View state
    float zoomFactor;

    QList<ImageTool*> m_imageTools;
};