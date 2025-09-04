#include "ResizeTool.hpp"
#include "ImageEditor.hpp"
#include <QtWidgets/QMessageBox>
#include <QtGui/QImage>

ResizeTool::ResizeTool(QObject *parent)
    : QObject(parent), m_editor(nullptr), m_resizeGroup(nullptr), m_widthSpinBox(nullptr), m_heightSpinBox(nullptr), m_aspectRatioCheckBox(nullptr)
{
}

QWidget *ResizeTool::getToolWidget()
{
    if (!m_resizeGroup)
    {
        m_resizeGroup = new QGroupBox(tr("Resize"));
        QVBoxLayout *resizeLayout = new QVBoxLayout(m_resizeGroup);

        QHBoxLayout *widthLayout = new QHBoxLayout();
        QLabel *widthLabel = new QLabel(tr("Width:"));
        m_widthSpinBox = new QSpinBox();
        m_widthSpinBox->setRange(1, 10000);
        widthLayout->addWidget(widthLabel);
        widthLayout->addWidget(m_widthSpinBox);
        resizeLayout->addLayout(widthLayout);

        QHBoxLayout *heightLayout = new QHBoxLayout();
        QLabel *heightLabel = new QLabel(tr("Height:"));
        m_heightSpinBox = new QSpinBox();
        m_heightSpinBox->setRange(1, 10000);
        heightLayout->addWidget(heightLabel);
        heightLayout->addWidget(m_heightSpinBox);
        resizeLayout->addLayout(heightLayout);

        m_aspectRatioCheckBox = new QCheckBox(tr("Maintain aspect ratio"));
        m_aspectRatioCheckBox->setChecked(true); // Default to true
        resizeLayout->addWidget(m_aspectRatioCheckBox);

        // Connect signals for aspect ratio maintenance
        connect(m_widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value)
                {
            if (m_aspectRatioCheckBox->isChecked() && m_editor && !m_editor->getCurrentImage().isNull()) {
                double ratio = static_cast<double>(m_editor->getCurrentImage().width()) / m_editor->getCurrentImage().height();
                m_heightSpinBox->setValue(qRound(value / ratio));
            } });

        connect(m_heightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value)
                {
            if (m_aspectRatioCheckBox->isChecked() && m_editor && !m_editor->getCurrentImage().isNull()) {
                double ratio = static_cast<double>(m_editor->getCurrentImage().width()) / m_editor->getCurrentImage().height();
                m_widthSpinBox->setValue(qRound(value * ratio));
            } });

        QPushButton *applyResizeBtn = new QPushButton(tr("Apply Resize"));
        connect(applyResizeBtn, &QPushButton::clicked, this, &ResizeTool::resizeImage);
        resizeLayout->addWidget(applyResizeBtn);
    }
    return m_resizeGroup;
}

QString ResizeTool::getToolName()
{
    return tr("Resize");
}

void ResizeTool::setImageEditor(ImageEditor *editor)
{
    m_editor = editor;
    // Update spin box values when image changes
    connect(m_editor, &ImageEditor::imageChanged, this, [this]()
            {
        if (m_editor && !m_editor->getCurrentImage().isNull()) {
            m_widthSpinBox->setValue(m_editor->getCurrentImage().width());
            m_heightSpinBox->setValue(m_editor->getCurrentImage().height());
        } });
}

void ResizeTool::resizeImage()
{
    if (!m_editor || m_editor->getCurrentImage().isNull())
    {
        return;
    }

    QSize newSize(m_widthSpinBox->value(), m_heightSpinBox->value());
    QImage resizedImage = m_editor->getCurrentImage().scaled(newSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_editor->setCurrentImage(resizedImage);
    m_editor->updateDisplay();
}