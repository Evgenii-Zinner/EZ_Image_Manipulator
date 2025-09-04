#include "CropTool.hpp"
#include "ImageEditor.hpp" // Include ImageEditor to access its scene and image
#include <QtWidgets/QMessageBox>
#include <QtGui/QImage>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView> // For sceneRect()
#include <QDebug> // Include for debugging

CropTool::CropTool(QObject* parent)
    : QObject(parent)
    , m_editor(nullptr)
    , m_cropGroup(nullptr)
    , m_startCropBtn(nullptr)
    , m_applyCropBtn(nullptr)
    , m_cancelCropBtn(nullptr)
    , m_cropAspectRatioCheckBox(nullptr)
    , m_widthSpinBox(nullptr)
    , m_heightSpinBox(nullptr)
    , m_widthLabel(nullptr)
    , m_heightLabel(nullptr)
    , m_cropOverlay(nullptr)
    , m_isCropping(false)
{
}

CropTool::~CropTool() {
    // Clean up QGraphicsItems if they are still in the scene
    // QGraphicsScene takes ownership of items added to it, so we only need to
    // ensure they are removed from the scene if they are not parented to the scene itself.
    // In this case, CropRectItem is parented to nullptr initially, then added to scene.
    // The darkOverlays are added directly to the scene.
    // If the scene is destroyed, it will clean up its items.
    // However, if this tool is destroyed before the scene, we should remove them.
    if (m_editor && m_editor->getGraphicsScene()) {
        if (m_cropOverlay) {
            m_editor->getGraphicsScene()->removeItem(m_cropOverlay);
            delete m_cropOverlay;
            m_cropOverlay = nullptr;
        }
        for (auto overlay : m_darkOverlays) {
            m_editor->getGraphicsScene()->removeItem(overlay);
            delete overlay;
        }
        m_darkOverlays.clear();
    }
}

QWidget* CropTool::getToolWidget() {
    if (!m_cropGroup) {
        m_cropGroup = new QGroupBox(tr("Crop"));
        QVBoxLayout* cropLayout = new QVBoxLayout(m_cropGroup);

        m_startCropBtn = new QPushButton(tr("Start Crop"));
        m_applyCropBtn = new QPushButton(tr("Apply Crop") + QString("\n") + tr("(Click & Drag on Image)"));
        m_cancelCropBtn = new QPushButton(tr("Cancel Crop"));
        m_cropAspectRatioCheckBox = new QCheckBox(tr("Maintain aspect ratio"));

        // Dimensions display
        QHBoxLayout* widthLayout = new QHBoxLayout();
        m_widthLabel = new QLabel(tr("Width:"));
        m_widthSpinBox = new QSpinBox();
        m_widthSpinBox->setRange(1, 10000); // Set appropriate range
        widthLayout->addWidget(m_widthLabel);
        widthLayout->addWidget(m_widthSpinBox);
        cropLayout->addLayout(widthLayout);

        QHBoxLayout* heightLayout = new QHBoxLayout();
        m_heightLabel = new QLabel(tr("Height:"));
        m_heightSpinBox = new QSpinBox();
        m_heightSpinBox->setRange(1, 10000); // Set appropriate range
        heightLayout->addWidget(m_heightLabel);
        heightLayout->addWidget(m_heightSpinBox);
        cropLayout->addLayout(heightLayout);

        connect(m_startCropBtn, &QPushButton::clicked, this, &CropTool::startCrop);
        connect(m_applyCropBtn, &QPushButton::clicked, this, &CropTool::applyCrop);
        connect(m_cancelCropBtn, &QPushButton::clicked, this, &CropTool::cancelCrop);
        connect(m_cropAspectRatioCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
            if (m_cropOverlay) {
                m_cropOverlay->setKeepAspectRatio(checked);
            }
        });

        // Connect spin boxes to update crop rect
        connect(m_widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &CropTool::updateCropRectFromSpinBoxes);
        connect(m_heightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &CropTool::updateCropRectFromSpinBoxes);

        cropLayout->addWidget(m_startCropBtn);
        cropLayout->addWidget(m_applyCropBtn);
        cropLayout->addWidget(m_cancelCropBtn);
        cropLayout->addWidget(m_cropAspectRatioCheckBox);
    }
    return m_cropGroup;
}

QString CropTool::getToolName() {
    return tr("Crop");
}

void CropTool::setImageEditor(ImageEditor* editor) {
    m_editor = editor;
}

void CropTool::startCrop() {
    if (!m_editor || m_editor->getCurrentImage().isNull() || m_isCropping) {
        return;
    }

    m_isCropping = true;
    m_editor->getGraphicsScene()->clearSelection();

    // Clear any existing crop overlay and dark overlays
    if (m_cropOverlay) {
        m_editor->getGraphicsScene()->removeItem(m_cropOverlay);
        delete m_cropOverlay;
        m_cropOverlay = nullptr;
    }
    for (auto overlay : m_darkOverlays) {
        m_editor->getGraphicsScene()->removeItem(overlay);
        delete overlay;
    }
    m_darkOverlays.clear();

    // Create new CropRectItem
    m_cropOverlay = new CropRectItem(m_editor->getGraphicsScene()->sceneRect());
    m_editor->getGraphicsScene()->addItem(m_cropOverlay);
    
    // Connect signals from CropRectItem to update overlays and spin boxes
    connect(m_cropOverlay, &CropRectItem::xChanged, this, &CropTool::updateCropOverlaysFromItem);
    connect(m_cropOverlay, &CropRectItem::yChanged, this, &CropTool::updateCropOverlaysFromItem);
    connect(m_cropOverlay, &CropRectItem::widthChanged, this, &CropTool::updateSpinBoxesFromCropRect);
    connect(m_cropOverlay, &CropRectItem::heightChanged, this, &CropTool::updateSpinBoxesFromCropRect);

    // Set initial aspect ratio state
    if (m_cropAspectRatioCheckBox) {
        m_cropOverlay->setKeepAspectRatio(m_cropAspectRatioCheckBox->isChecked());
    }

    updateCropOverlays(); // Initial update of dark overlays
    updateSpinBoxesFromCropRect(); // Set initial spin box values
}

void CropTool::applyCrop() {
    if (!m_isCropping || !m_cropOverlay || !m_editor) {
        qDebug() << "Apply Crop: Pre-conditions not met.";
        return;
    }

    // Get crop rectangle from spin boxes (which represent original image dimensions)
    qreal cropWidth = m_widthSpinBox->value();
    qreal cropHeight = m_heightSpinBox->value();

    // The cropOverlay's position is in scene coordinates. We need to convert it to original image coordinates.
    // The top-left corner of the crop overlay in scene coordinates.
    QPointF cropTopLeftScene = m_cropOverlay->sceneBoundingRect().topLeft();
    qreal currentZoomFactor = m_editor->getZoomFactor();

    // Convert top-left corner to original image coordinates
    int imageX = qRound(cropTopLeftScene.x() / currentZoomFactor);
    int imageY = qRound(cropTopLeftScene.y() / currentZoomFactor);

    QRect imageRect(
        imageX,
        imageY,
        qRound(cropWidth),
        qRound(cropHeight)
    );
    qDebug() << "Apply Crop: imageRect (original image coords) =" << imageRect;

    QImage currentImage = m_editor->getCurrentImage();
    qDebug() << "Apply Crop: currentImage dimensions =" << currentImage.width() << "x" << currentImage.height();

    // Ensure the crop rectangle is within the bounds of the current image
    imageRect = imageRect.intersected(currentImage.rect());
    qDebug() << "Apply Crop: imageRect (intersected) =" << imageRect;

    if (imageRect.isEmpty()) {
        QMessageBox::warning(m_cropGroup, tr("Crop Error"), tr("Invalid crop area selected."));
        qDebug() << "Apply Crop: imageRect is empty.";
        return;
    }

    currentImage = currentImage.copy(imageRect);
    m_editor->setCurrentImage(currentImage); // Update the image in ImageEditor
    cancelCrop();
    m_editor->updateDisplay(); // Refresh the display
    qDebug() << "Apply Crop: Cropping applied successfully.";
}

void CropTool::cancelCrop() {
    if (m_cropOverlay) {
        if (m_editor && m_editor->getGraphicsScene()) {
            m_editor->getGraphicsScene()->removeItem(m_cropOverlay);
        }
        delete m_cropOverlay;
        m_cropOverlay = nullptr;
    }
    for (auto overlay : m_darkOverlays) {
        if (m_editor && m_editor->getGraphicsScene()) {
            m_editor->getGraphicsScene()->removeItem(overlay);
        }
        delete overlay;
    }
    m_darkOverlays.clear();
    
    m_isCropping = false;
    if (m_editor) {
        m_editor->updateDisplay(); // Refresh display to remove any remnants
    }
}

void CropTool::updateCropOverlaysFromItem() {
    updateCropOverlays();
}

void CropTool::updateCropOverlays() {
    if (!m_isCropping || !m_cropOverlay || !m_editor || !m_editor->getGraphicsScene()) {
        return;
    }
    
    QGraphicsScene* scene = m_editor->getGraphicsScene();
    QRectF sceneRect = scene->sceneRect();
    QRectF rect = m_cropOverlay->sceneBoundingRect(); // Use sceneBoundingRect() to get the rect in scene coordinates
    
    // Ensure darkOverlays list is correctly sized
    if (m_darkOverlays.size() != 4) {
        for (auto overlay : m_darkOverlays) {
            scene->removeItem(overlay);
            delete overlay;
        }
        m_darkOverlays.clear();
        for (int i = 0; i < 4; ++i) {
            auto overlay = scene->addRect(sceneRect, Qt::NoPen, QBrush(QColor(0, 0, 0, 127)));
            m_darkOverlays.append(overlay);
        }
    }

    // Top
    m_darkOverlays[0]->setRect(QRectF(sceneRect.left(), sceneRect.top(),
                                   sceneRect.width(), rect.top() - sceneRect.top()));
    // Bottom
    m_darkOverlays[1]->setRect(QRectF(sceneRect.left(), rect.bottom(),
                                   sceneRect.width(), sceneRect.bottom() - rect.bottom()));
    // Left
    m_darkOverlays[2]->setRect(QRectF(sceneRect.left(), rect.top(),
                                   rect.left() - sceneRect.left(), rect.height()));
    // Right
    m_darkOverlays[3]->setRect(QRectF(rect.right(), rect.top(),
                                   sceneRect.right() - rect.right(), rect.height()));
}

void CropTool::updateSpinBoxesFromCropRect() {
    if (!m_cropOverlay || !m_widthSpinBox || !m_heightSpinBox || !m_editor) return;

    // Block signals to prevent recursive calls when updating spin boxes
    m_widthSpinBox->blockSignals(true);
    m_heightSpinBox->blockSignals(true);

    qreal currentZoomFactor = m_editor->getZoomFactor();
    QRectF cropRectScene = m_cropOverlay->sceneBoundingRect();

    // Convert scene dimensions to original image dimensions
    m_widthSpinBox->setValue(qRound(cropRectScene.width() / currentZoomFactor));
    m_heightSpinBox->setValue(qRound(cropRectScene.height() / currentZoomFactor));

    m_widthSpinBox->blockSignals(false);
    m_heightSpinBox->blockSignals(false);
}

void CropTool::updateCropRectFromSpinBoxes() {
    if (!m_cropOverlay || !m_widthSpinBox || !m_heightSpinBox || !m_editor) return;

    // Block signals from CropRectItem to prevent recursive calls
    m_cropOverlay->blockSignals(true);

    qreal newWidthOriginal = m_widthSpinBox->value();
    qreal newHeightOriginal = m_heightSpinBox->value();

    qreal currentZoomFactor = m_editor->getZoomFactor();

    // Convert original image dimensions to scene dimensions
    qreal newWidthScene = newWidthOriginal * currentZoomFactor;
    qreal newHeightScene = newHeightOriginal * currentZoomFactor;

    QRectF currentRectScene = m_cropOverlay->sceneBoundingRect();

    if (m_cropAspectRatioCheckBox->isChecked()) {
        // Maintain aspect ratio based on original image dimensions
        qreal currentAspectRatio = currentRectScene.width() / currentRectScene.height();
        if (sender() == m_widthSpinBox) {
            newHeightScene = newWidthScene / currentAspectRatio;
            newHeightOriginal = newWidthOriginal / currentAspectRatio;
            m_heightSpinBox->blockSignals(true);
            m_heightSpinBox->setValue(qRound(newHeightOriginal));
            m_heightSpinBox->blockSignals(false);
        } else if (sender() == m_heightSpinBox) {
            newWidthScene = newHeightScene * currentAspectRatio;
            newWidthOriginal = newHeightOriginal * currentAspectRatio;
            m_widthSpinBox->blockSignals(true);
            m_widthSpinBox->setValue(qRound(newWidthOriginal));
            m_widthSpinBox->blockSignals(false);
        }
    }

    // Update the CropRectItem's rectangle in scene coordinates
    // Keep the top-left corner fixed for now, or adjust based on desired behavior
    m_cropOverlay->setRect(QRectF(currentRectScene.topLeft(), QSizeF(newWidthScene, newHeightScene)));

    m_cropOverlay->blockSignals(false);

    updateCropOverlays(); // Update dark overlays after rect change
}
