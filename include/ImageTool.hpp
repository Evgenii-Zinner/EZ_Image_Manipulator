#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/QString>

// Forward declaration to avoid circular dependency
class ImageEditor;

class ImageTool {
public:
    virtual ~ImageTool() = default;

    // Returns the UI widget for the tool to be added to the tools panel
    virtual QWidget* getToolWidget() = 0;

    // Returns the name of the tool
    virtual QString getToolName() = 0;

    // Sets the ImageEditor instance, allowing the tool to interact with it
    virtual void setImageEditor(ImageEditor* editor) = 0;

    // Optional: Called when the tool is activated (e.g., a tab is selected)
    virtual void activate() {}

    // Optional: Called when the tool is deactivated
    virtual void deactivate() {}
};