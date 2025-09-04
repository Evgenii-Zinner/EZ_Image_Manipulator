#include "ZoomTool.hpp"
#include "ImageEditor.hpp"
#include <QtGui/QImage>
#include <QtWidgets/QGraphicsView> // For viewport()

ZoomTool::ZoomTool(QObject *parent)
    : QObject(parent), m_editor(nullptr), m_zoomGroup(nullptr)
{
}

QWidget *ZoomTool::getToolWidget()
{
    if (!m_zoomGroup)
    {
        m_zoomGroup = new QGroupBox(tr("Zoom"));
        QVBoxLayout *zoomLayout = new QVBoxLayout(m_zoomGroup);

        QHBoxLayout *zoomBtnsLayout = new QHBoxLayout();
        QPushButton *zoomInBtn = new QPushButton(tr("🔍 +"));
        QPushButton *zoomOutBtn = new QPushButton(tr("🔍 -"));
        QPushButton *zoomFitBtn = new QPushButton(tr("Fit"));
        QPushButton *zoom100Btn = new QPushButton(tr("100%"));
        connect(zoomInBtn, &QPushButton::clicked, this, &ZoomTool::zoomIn);
        connect(zoomOutBtn, &QPushButton::clicked, this, &ZoomTool::zoomOut);
        connect(zoomFitBtn, &QPushButton::clicked, this, &ZoomTool::zoomFit);
        connect(zoom100Btn, &QPushButton::clicked, this, &ZoomTool::zoom100);
        zoomBtnsLayout->addWidget(zoomInBtn);
        zoomBtnsLayout->addWidget(zoomOutBtn);
        zoomBtnsLayout->addWidget(zoomFitBtn);
        zoomBtnsLayout->addWidget(zoom100Btn);
        zoomLayout->addLayout(zoomBtnsLayout);
    }
    return m_zoomGroup;
}

QString ZoomTool::getToolName()
{
    return tr("Zoom");
}

void ZoomTool::setImageEditor(ImageEditor *editor)
{
    m_editor = editor;
}

void ZoomTool::zoomIn()
{
    if (!m_editor)
        return;
    qreal currentZoomFactor = m_editor->getZoomFactor();
    currentZoomFactor = qMin(5.0f, currentZoomFactor * 1.2f);
    m_editor->setZoomFactor(currentZoomFactor); // Assuming ImageEditor has setZoomFactor
    m_editor->updateDisplay();
}

void ZoomTool::zoomOut()
{
    if (!m_editor)
        return;
    qreal currentZoomFactor = m_editor->getZoomFactor();
    currentZoomFactor = qMax(0.1f, currentZoomFactor / 1.2f);
    m_editor->setZoomFactor(currentZoomFactor); // Assuming ImageEditor has setZoomFactor
    m_editor->updateDisplay();
}

void ZoomTool::zoomFit()
{
    if (!m_editor || m_editor->getCurrentImage().isNull())
    {
        return;
    }

    // Calculate zoom factor to fit the image within the view
    qreal hScale = static_cast<qreal>(m_editor->getGraphicsView()->viewport()->width()) / m_editor->getCurrentImage().width();   // Assuming getGraphicsView()
    qreal vScale = static_cast<qreal>(m_editor->getGraphicsView()->viewport()->height()) / m_editor->getCurrentImage().height(); // Assuming getGraphicsView()
    m_editor->setZoomFactor(qMin(hScale, vScale));                                                                               // Assuming ImageEditor has setZoomFactor
    m_editor->updateDisplay();
}

void ZoomTool::zoom100()
{
    if (!m_editor)
        return;
    m_editor->setZoomFactor(1.0f); // Assuming ImageEditor has setZoomFactor
    m_editor->updateDisplay();
}

void ZoomTool::resetZoom()
{
    if (!m_editor)
        return;
    m_editor->setZoomFactor(1.0f); // This is now effectively 100% zoom
    m_editor->updateDisplay();
}