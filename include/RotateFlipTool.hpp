#pragma once

#include "ImageTool.hpp"
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>

// Forward declaration
class ImageEditor;

class RotateFlipTool : public QObject, public ImageTool {
    Q_OBJECT

public:
    explicit RotateFlipTool(QObject* parent = nullptr);
    ~RotateFlipTool() override = default;

    // ImageTool interface
    QWidget* getToolWidget() override;
    QString getToolName() override;
    void setImageEditor(ImageEditor* editor) override;

private slots:
    void rotateLeft();
    void rotateRight();
    void flipHorizontal();
    void flipVertical();

private:
    ImageEditor* m_editor;
    QGroupBox* m_rotateFlipGroup;
};