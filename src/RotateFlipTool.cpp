#include "RotateFlipTool.hpp"
#include "ImageEditor.hpp"
#include <QtGui/QTransform>
#include <QtGui/QImage>

RotateFlipTool::RotateFlipTool(QObject *parent)
    : QObject(parent), m_editor(nullptr), m_rotateFlipGroup(nullptr)
{
}

QWidget *RotateFlipTool::getToolWidget()
{
    if (!m_rotateFlipGroup)
    {
        m_rotateFlipGroup = new QGroupBox(tr("Rotate & Flip"));
        QVBoxLayout *rotateFlipLayout = new QVBoxLayout(m_rotateFlipGroup);

        QHBoxLayout *rotateBtnsLayout = new QHBoxLayout();
        QPushButton *rotateLeftBtn = new QPushButton(tr("↺ 90°"));
        QPushButton *rotateRightBtn = new QPushButton(tr("↻ 90°"));
        connect(rotateLeftBtn, &QPushButton::clicked, this, &RotateFlipTool::rotateLeft);
        connect(rotateRightBtn, &QPushButton::clicked, this, &RotateFlipTool::rotateRight);
        rotateBtnsLayout->addWidget(rotateLeftBtn);
        rotateBtnsLayout->addWidget(rotateRightBtn);
        rotateFlipLayout->addLayout(rotateBtnsLayout);

        QPushButton *flipHorizontalBtn = new QPushButton(tr("Flip Horizontal"));
        QPushButton *flipVerticalBtn = new QPushButton(tr("Flip Vertical"));
        connect(flipHorizontalBtn, &QPushButton::clicked, this, &RotateFlipTool::flipHorizontal);
        connect(flipVerticalBtn, &QPushButton::clicked, this, &RotateFlipTool::flipVertical);
        rotateFlipLayout->addWidget(flipHorizontalBtn);
        rotateFlipLayout->addWidget(flipVerticalBtn);
    }
    return m_rotateFlipGroup;
}

QString RotateFlipTool::getToolName()
{
    return tr("Rotate & Flip");
}

void RotateFlipTool::setImageEditor(ImageEditor *editor)
{
    m_editor = editor;
}

void RotateFlipTool::rotateLeft()
{
    if (!m_editor || m_editor->getCurrentImage().isNull())
    {
        return;
    }
    QTransform transform;
    transform.rotate(-90);
    QImage rotatedImage = m_editor->getCurrentImage().transformed(transform);
    m_editor->setCurrentImage(rotatedImage);
    m_editor->updateDisplay();
}

void RotateFlipTool::rotateRight()
{
    if (!m_editor || m_editor->getCurrentImage().isNull())
    {
        return;
    }
    QTransform transform;
    transform.rotate(90);
    QImage rotatedImage = m_editor->getCurrentImage().transformed(transform);
    m_editor->setCurrentImage(rotatedImage);
    m_editor->updateDisplay();
}

void RotateFlipTool::flipHorizontal()
{
    if (!m_editor || m_editor->getCurrentImage().isNull())
    {
        return;
    }
    QImage flippedImage = m_editor->getCurrentImage().flipped(Qt::Horizontal);
    m_editor->setCurrentImage(flippedImage);
    m_editor->updateDisplay();
}

void RotateFlipTool::flipVertical()
{
    if (!m_editor || m_editor->getCurrentImage().isNull())
    {
        return;
    }
    QImage flippedImage = m_editor->getCurrentImage().flipped(Qt::Vertical);
    m_editor->setCurrentImage(flippedImage);
    m_editor->updateDisplay();
}