#pragma once

#include "ImageTool.hpp"
#include "CropRectItem.hpp"
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QList>
#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLabel>

// Forward declaration
class ImageEditor;

class CropTool : public QObject, public ImageTool {
    Q_OBJECT

public:
    explicit CropTool(QObject* parent = nullptr);
    ~CropTool() override;

    // ImageTool interface
    QWidget* getToolWidget() override;
    QString getToolName() override;
    void setImageEditor(ImageEditor* editor) override;

private slots:
    void startCrop();
    void applyCrop();
    void cancelCrop();
    void updateCropOverlaysFromItem();
    void updateSpinBoxesFromCropRect();
    void updateCropRectFromSpinBoxes();

private:
    void updateCropOverlays();

    ImageEditor* m_editor;
    QGroupBox* m_cropGroup;
    QPushButton* m_startCropBtn;
    QPushButton* m_applyCropBtn;
    QPushButton* m_cancelCropBtn;
    QCheckBox* m_cropAspectRatioCheckBox;
    QSpinBox* m_widthSpinBox;
    QSpinBox* m_heightSpinBox;
    QLabel* m_widthLabel;
    QLabel* m_heightLabel;

    CropRectItem* m_cropOverlay;
    QList<QGraphicsRectItem*> m_darkOverlays;
    bool m_isCropping;
};