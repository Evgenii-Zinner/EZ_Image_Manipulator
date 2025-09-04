#pragma once

#include "ImageTool.hpp"
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>

// Forward declaration
class ImageEditor;

class ZoomTool : public QObject, public ImageTool {
    Q_OBJECT

public:
    explicit ZoomTool(QObject* parent = nullptr);
    ~ZoomTool() override = default;

    // ImageTool interface
    QWidget* getToolWidget() override;
    QString getToolName() override;
    void setImageEditor(ImageEditor* editor) override;

private slots:
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoom100();
    void resetZoom();

private:
    ImageEditor* m_editor;
    QGroupBox* m_zoomGroup;
};