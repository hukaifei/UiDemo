#ifndef DOCKWIDGETDEMO_H
#define DOCKWIDGETDEMO_H

#include <QMainWindow>

class QDockWidget;
class QListWidget;
class QTreeWidget;
class QTextEdit;
class QTableWidget;
class QLabel;
class QPushButton;
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onAddDockWidget();
    void onRemoveDockWidget();
    void onToggleDockWidget(QDockWidget *dock);
    void onLockLayout(bool locked);
    void onLogMessage(const QString &msg);
    void onResetLayout();

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    QDockWidget *createFileBrowser();
    QDockWidget *createPropertyPanel();
    QDockWidget *createOutputLog();
    QDockWidget *createToolPalette();

    // 中央控件
    QTextEdit *m_centralEdit;

    // 停靠窗口
    QDockWidget *m_fileDock;
    QDockWidget *m_propertyDock;
    QDockWidget *m_outputDock;
    QDockWidget *m_toolDock;

    // 动态创建的停靠窗口计数
    int m_dynamicDockCount = 0;
};

#endif // DOCKWIDGETDEMO_H
