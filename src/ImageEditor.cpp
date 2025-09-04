#include "ImageEditor.hpp"
#include "WebPHandler.hpp"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtGui/QImageReader>
#include <QtGui/QImageWriter>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtGui/QTransform>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtCore/Qt>
#include <QtWidgets/QGroupBox>
#include <QtGui/QMouseEvent>
#include "CropRectItem.hpp"
#include "CropTool.hpp"
#include "OpenSaveTool.hpp"
#include "ResizeTool.hpp"
#include "RotateFlipTool.hpp"
#include "ZoomTool.hpp"

ImageEditor::ImageEditor(QWidget *parent)
    : QMainWindow(parent), scene(new QGraphicsScene(this)), view(new QGraphicsView(scene)), toolsDock(new QDockWidget(tr("Tools"), this)), zoomFactor(1.0)

{
    setupUI();
    setupToolsDock();

    setCentralWidget(view);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRenderHint(QPainter::SmoothPixmapTransform);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

ImageEditor::~ImageEditor()
{
    for (ImageTool *tool : m_imageTools)
    {
        delete tool;
    }
    m_imageTools.clear();
}

void ImageEditor::setupUI()
{
    setWindowTitle(tr("EZ Image Manipulator"));
    resize(1200, 800);
    setMinimumSize(800, 600);
}

void ImageEditor::updateDisplay()
{
    if (currentImage.isNull())
    {
        return;
    }

    scene->clear();

    // Scale image according to zoom factor
    QSize newSize = calculateZoomedSize();
    QGraphicsPixmapItem *item = scene->addPixmap(QPixmap::fromImage(currentImage.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

    // Center the image
    scene->setSceneRect(item->boundingRect());
    view->setSceneRect(item->boundingRect());
    view->centerOn(item);

    // Update window title
    updateTitle();
}

void ImageEditor::updateTitle()
{
    QString title = tr("EZ Image Manipulator");
    if (!m_currentFilePath.isEmpty())
    {
        title += " - " + QFileInfo(m_currentFilePath).fileName();
    }
    setWindowTitle(title);
}

QSize ImageEditor::calculateZoomedSize() const
{
    return QSize(
        qRound(currentImage.width() * zoomFactor),
        qRound(currentImage.height() * zoomFactor));
}

void ImageEditor::setupToolsDock()
{
    QWidget *toolsWidget = new QWidget(toolsDock);
    QVBoxLayout *mainLayout = new QVBoxLayout(toolsWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5); // Add some padding

    // Add OpenSaveTool
    OpenSaveTool *openSaveTool = new OpenSaveTool(this);
    openSaveTool->setImageEditor(this);
    mainLayout->addWidget(openSaveTool->getToolWidget());
    m_imageTools.append(openSaveTool);

    mainLayout->addSpacing(10); // Add some space after the main action buttons

    // Add ResizeTool
    ResizeTool *resizeTool = new ResizeTool(this);
    resizeTool->setImageEditor(this);
    mainLayout->addWidget(resizeTool->getToolWidget());
    m_imageTools.append(resizeTool);

    // Add RotateFlipTool
    RotateFlipTool *rotateFlipTool = new RotateFlipTool(this);
    rotateFlipTool->setImageEditor(this);
    mainLayout->addWidget(rotateFlipTool->getToolWidget());
    m_imageTools.append(rotateFlipTool);

    // Add ZoomTool
    ZoomTool *zoomTool = new ZoomTool(this);
    zoomTool->setImageEditor(this);
    mainLayout->addWidget(zoomTool->getToolWidget());
    m_imageTools.append(zoomTool);

    // Add CropTool
    CropTool *cropTool = new CropTool(this); // Parent to ImageEditor for ownership
    cropTool->setImageEditor(this);
    mainLayout->addWidget(cropTool->getToolWidget());
    m_imageTools.append(cropTool);

    mainLayout->addStretch(); // Push all groups to the top

    toolsWidget->setLayout(mainLayout);
    toolsDock->setWidget(toolsWidget);
    toolsDock->setFeatures(QDockWidget::NoDockWidgetFeatures); // Disable close button and other features
    addDockWidget(Qt::RightDockWidgetArea, toolsDock);
}