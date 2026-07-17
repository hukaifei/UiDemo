#include <QApplication>
#include <QStyleFactory>
#include "DockWidgetDemo.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("DockWidgetDemo"));
    app.setStyleSheet(
        "QMainWindow { background-color: #f0f2f5; }"
        "QDockWidget { background-color: #ffffff; }"
        "QDockWidget::title { "
        "  background-color: #e8edf2; "
        "  padding: 6px 8px; "
        "  border: 1px solid #c8cdd3; "
        "  border-bottom: none; "
        "  font-weight: bold; "
        "}"
        "QMenuBar { background-color: #e8edf2; padding: 2px; }"
        "QMenuBar::item:selected { background-color: #3498db; color: white; border-radius: 3px; }"
        "QToolBar { background-color: #f8f9fa; border-bottom: 1px solid #ddd; spacing: 4px; }"
        "QToolBar QToolButton { padding: 4px 8px; border-radius: 3px; }"
        "QToolBar QToolButton:hover { background-color: #e0e0e0; }"
        "QStatusBar { background-color: #e8edf2; border-top: 1px solid #ccc; }"
        "QGroupBox { font-weight: bold; border: 1px solid #ccc; border-radius: 4px; margin-top: 8px; padding-top: 8px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }"
    );

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
