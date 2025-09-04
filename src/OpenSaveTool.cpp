#include "OpenSaveTool.hpp"
#include "ImageEditor.hpp"
#include "WebPHandler.hpp" // For WebP encoding/decoding
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QStandardPaths>
#include <QtCore/QFileInfo>

OpenSaveTool::OpenSaveTool(QObject *parent)
    : QObject(parent), m_editor(nullptr), m_openSaveGroup(nullptr), m_openBtn(nullptr), m_saveBtn(nullptr), m_infoBtn(nullptr)
{
}

QWidget *OpenSaveTool::getToolWidget()
{
    if (!m_openSaveGroup)
    {
        m_openSaveGroup = new QGroupBox(tr("File Operations"));
        QVBoxLayout *layout = new QVBoxLayout(m_openSaveGroup);

        m_openBtn = new QPushButton(tr("Open Image"));
        connect(m_openBtn, &QPushButton::clicked, this, &OpenSaveTool::openImage);
        layout->addWidget(m_openBtn);

        m_saveBtn = new QPushButton(tr("Save Image"));
        connect(m_saveBtn, &QPushButton::clicked, this, &OpenSaveTool::saveImage);
        layout->addWidget(m_saveBtn);

        m_infoBtn = new QPushButton(tr("Image Info"));
        connect(m_infoBtn, &QPushButton::clicked, this, &OpenSaveTool::showImageInfo);
        layout->addWidget(m_infoBtn);
    }
    return m_openSaveGroup;
}

QString OpenSaveTool::getToolName()
{
    return tr("File Operations");
}

void OpenSaveTool::setImageEditor(ImageEditor *editor)
{
    m_editor = editor;
}

void OpenSaveTool::openImage()
{
    if (!m_editor)
        return;

    QString lastDirectory = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    // If ImageEditor has a current file path, use its directory as the last directory
    if (!m_editor->getCurrentFilePath().isEmpty())
    {
        lastDirectory = QFileInfo(m_editor->getCurrentFilePath()).path();
    }

    QString fileName = QFileDialog::getOpenFileName(m_openSaveGroup,
                                                    tr("Open Image"), lastDirectory,
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp *.webp);;All Files (*)"));

    if (fileName.isEmpty())
    {
        return;
    }

    QImage loadedImage;
    if (fileName.endsWith(".webp", Qt::CaseInsensitive))
    {
        loadedImage = WebPHandler::decode(fileName);
    }
    else
    {
        loadedImage.load(fileName);
    }

    if (loadedImage.isNull())
    {
        QMessageBox::warning(m_openSaveGroup, tr("Error"), tr("Could not load image."));
        return;
    }

    m_editor->setCurrentImage(loadedImage);
    m_editor->setCurrentFilePath(fileName); // Assuming ImageEditor has setCurrentFilePath
    // m_editor->setLastDirectory(QFileInfo(fileName).path()); // Assuming ImageEditor has setLastDirectory
    // Update spin boxes in ImageEditor if they exist and are accessible
    // This part needs careful consideration. If spin boxes are part of ResizeTool,
    // OpenSaveTool shouldn't directly access them. ImageEditor should coordinate.
    // For now, I'll assume ImageEditor will handle updating its UI elements after image load.
    m_editor->updateDisplay();
}

void OpenSaveTool::saveImage()
{
    if (!m_editor || m_editor->getCurrentImage().isNull())
    {
        return;
    }

    QString currentFilePath = m_editor->getCurrentFilePath();
    QString lastDirectory = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (!currentFilePath.isEmpty())
    {
        lastDirectory = QFileInfo(currentFilePath).path();
    }

    QString suggestedName = currentFilePath.isEmpty() ? lastDirectory + "/untitled.webp" : QFileInfo(currentFilePath).path() + "/" + QFileInfo(currentFilePath).baseName() + ".webp";

    QString fileName = QFileDialog::getSaveFileName(m_openSaveGroup,
                                                    tr("Save Image"), suggestedName,
                                                    tr("WebP Image (*.webp);;PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp)"));

    if (fileName.isEmpty())
    {
        return;
    }

    bool success;
    if (fileName.endsWith(".webp", Qt::CaseInsensitive))
    {
        success = WebPHandler::encode(m_editor->getCurrentImage(), fileName, 90);
    }
    else
    {
        success = m_editor->getCurrentImage().save(fileName);
    }

    if (!success)
    {
        QMessageBox::warning(m_openSaveGroup, tr("Error"), tr("Could not save image."));
    }
    else
    {
        m_editor->setCurrentFilePath(fileName); // Update current file path after saving
    }
}

void OpenSaveTool::showImageInfo()
{
    if (!m_editor || m_editor->getCurrentImage().isNull())
    {
        return;
    }

    QImage currentImage = m_editor->getCurrentImage();
    QString info = tr("Dimensions: %1 x %2\n").arg(currentImage.width()).arg(currentImage.height());
    info += tr("Format: %1\n").arg(currentImage.format());
    info += tr("Depth: %1 bits\n").arg(currentImage.depth());

    QMessageBox::information(m_openSaveGroup, tr("Image Information"), info);
}
