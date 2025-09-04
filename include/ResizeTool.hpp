#pragma once

#include "ImageTool.hpp"
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

// Forward declaration
class ImageEditor;

class ResizeTool : public QObject, public ImageTool {
    Q_OBJECT

public:
    explicit ResizeTool(QObject* parent = nullptr);
    ~ResizeTool() override = default;

    // ImageTool interface
    QWidget* getToolWidget() override;
    QString getToolName() override;
    void setImageEditor(ImageEditor* editor) override;

private slots:
    void resizeImage();

private:
    ImageEditor* m_editor;
    QGroupBox* m_resizeGroup;
    QSpinBox* m_widthSpinBox;
    QSpinBox* m_heightSpinBox;
    QCheckBox* m_aspectRatioCheckBox;
};