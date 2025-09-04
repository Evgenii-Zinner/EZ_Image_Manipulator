#pragma once

#include "ImageTool.hpp"
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>

// Forward declaration
class ImageEditor;

class OpenSaveTool : public QObject, public ImageTool {
    Q_OBJECT

public:
    explicit OpenSaveTool(QObject* parent = nullptr);
    ~OpenSaveTool() override = default;

    // ImageTool interface
    QWidget* getToolWidget() override;
    QString getToolName() override;
    void setImageEditor(ImageEditor* editor) override;

private slots:
    void openImage();
    void saveImage();
    void showImageInfo();

private:
    ImageEditor* m_editor;
    QGroupBox* m_openSaveGroup;
    QPushButton* m_openBtn;
    QPushButton* m_saveBtn;
    QPushButton* m_infoBtn;
};