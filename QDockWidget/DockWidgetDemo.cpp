#include "DockWidgetDemo.h"

#include <QDockWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QAction>
#include <QDateTime>
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralEdit(nullptr)
    , m_fileDock(nullptr)
    , m_propertyDock(nullptr)
    , m_outputDock(nullptr)
    , m_toolDock(nullptr)
{
    setupUi();
    setWindowTitle(QStringLiteral("QDockWidget 控件测试 Demo"));
    resize(1200, 750);
}

void MainWindow::setupUi()
{
    // ---- 中央控件 ----
    m_centralEdit = new QTextEdit(this);
    m_centralEdit->setReadOnly(false);
    m_centralEdit->setPlaceholderText(QStringLiteral(
        "这是一个 QDockWidget 功能测试 Demo。\n\n"
        "📌 功能介绍：\n"
        "  • 停靠窗口可以拖拽到主窗口的四个边缘（左、右、上、下）\n"
        "  • 可以将停靠窗口拖出成为独立浮动窗口\n"
        "  • 可以将多个停靠窗口叠加为选项卡（Tab）\n"
        "  • 双击标题栏可切换浮动/停靠状态\n"
        "  • 通过菜单「视图」可以显示/隐藏停靠窗口\n"
        "  • 通过菜单「停靠窗口」可以添加/删除动态停靠窗口\n"
        "  • 工具栏可以锁定/解锁停靠窗口布局\n"
        "\n💡 试试拖拽右侧的各个停靠窗口！"
    ));
    m_centralEdit->setStyleSheet(
        "QTextEdit { font-size: 14px; padding: 12px; border: 1px solid #ccc; border-radius: 4px; }"
    );
    setCentralWidget(m_centralEdit);

    // ---- 创建各个停靠窗口 ----
    m_fileDock = createFileBrowser();
    m_propertyDock = createPropertyPanel();
    m_outputDock = createOutputLog();
    m_toolDock = createToolPalette();

    // 左侧: 文件浏览器
    addDockWidget(Qt::LeftDockWidgetArea, m_fileDock);

    // 右侧: 属性面板
    addDockWidget(Qt::RightDockWidgetArea, m_propertyDock);

    // 底部: 输出日志
    addDockWidget(Qt::BottomDockWidgetArea, m_outputDock);

    // 将工具面板叠加到属性面板旁边
    tabifyDockWidget(m_propertyDock, m_toolDock);
    m_propertyDock->raise(); // 默认显示属性面板

    // ---- 菜单栏 ----
    setupMenuBar();

    // ---- 工具栏 ----
    setupToolBar();

    // ---- 状态栏 ----
    setupStatusBar();

    onLogMessage(QStringLiteral("QDockWidget Demo 已启动"));
}

// ============================================================
// 菜单栏
// ============================================================
void MainWindow::setupMenuBar()
{
    // ---- 视图菜单 ----
    QMenu *viewMenu = menuBar()->addMenu(QStringLiteral("视图(&V)"));

    QAction *toggleFileAct = m_fileDock->toggleViewAction();
    toggleFileAct->setShortcut(QKeySequence("Ctrl+1"));
    viewMenu->addAction(toggleFileAct);

    QAction *togglePropAct = m_propertyDock->toggleViewAction();
    togglePropAct->setShortcut(QKeySequence("Ctrl+2"));
    viewMenu->addAction(togglePropAct);

    QAction *toggleOutputAct = m_outputDock->toggleViewAction();
    toggleOutputAct->setShortcut(QKeySequence("Ctrl+3"));
    viewMenu->addAction(toggleOutputAct);

    QAction *toggleToolAct = m_toolDock->toggleViewAction();
    toggleToolAct->setShortcut(QKeySequence("Ctrl+4"));
    viewMenu->addAction(toggleToolAct);

    viewMenu->addSeparator();

    QAction *resetAct = viewMenu->addAction(QStringLiteral("重置布局(&R)"));
    resetAct->setShortcut(QKeySequence("Ctrl+R"));
    connect(resetAct, &QAction::triggered, this, &MainWindow::onResetLayout);

    // ---- 停靠窗口菜单 ----
    QMenu *dockMenu = menuBar()->addMenu(QStringLiteral("停靠窗口(&D)"));

    QAction *addDockAct = dockMenu->addAction(QStringLiteral("添加动态停靠窗口(&A)"));
    addDockAct->setShortcut(QKeySequence("Ctrl+D"));
    connect(addDockAct, &QAction::triggered, this, &MainWindow::onAddDockWidget);

    QAction *removeDockAct = dockMenu->addAction(QStringLiteral("移除最后添加的停靠窗口(&M)"));
    connect(removeDockAct, &QAction::triggered, this, &MainWindow::onRemoveDockWidget);

    // ---- 帮助菜单 ----
    QMenu *helpMenu = menuBar()->addMenu(QStringLiteral("帮助(&H)"));
    QAction *aboutAct = helpMenu->addAction(QStringLiteral("关于(&A)"));
    connect(aboutAct, &QAction::triggered, [this]() {
        QMessageBox::about(this,
            QStringLiteral("关于 QDockWidget Demo"),
            QStringLiteral("QDockWidget 控件测试 Demo\n\n"
                           "演示了 QDockWidget 的常用功能：\n"
                           "  - 多区域停靠\n"
                           "  - 浮动窗口\n"
                           "  - 选项卡叠加\n"
                           "  - 动态创建/销毁\n"
                           "  - 布局锁定\n"
                           "  - 嵌套停靠"));
    });
}

// ============================================================
// 工具栏
// ============================================================
void MainWindow::setupToolBar()
{
    QToolBar *toolbar = addToolBar(QStringLiteral("主工具栏"));
    toolbar->setMovable(false);

    QAction *addAct = toolbar->addAction(
        style()->standardIcon(QStyle::SP_FileDialogNewFolder),
        QStringLiteral("添加停靠窗口"));
    QAction *removeAct = toolbar->addAction(
        style()->standardIcon(QStyle::SP_TrashIcon),
        QStringLiteral("移除停靠窗口"));
    QAction *resetAct = toolbar->addAction(
        style()->standardIcon(QStyle::SP_BrowserReload),
        QStringLiteral("重置布局"));

    QAction *lockAct = toolbar->addAction(QStringLiteral("🔒 锁定布局"));
    lockAct->setCheckable(true);

    connect(addAct, &QAction::triggered, this, &MainWindow::onAddDockWidget);
    connect(removeAct, &QAction::triggered, this, &MainWindow::onRemoveDockWidget);
    connect(resetAct, &QAction::triggered, this, &MainWindow::onResetLayout);
    connect(lockAct, &QAction::toggled, this, &MainWindow::onLockLayout);

    // 帮助按钮
    toolbar->addSeparator();
    QAction *helpAct = toolbar->addAction(
        style()->standardIcon(QStyle::SP_MessageBoxQuestion),
        QStringLiteral("帮助"));
    connect(helpAct, &QAction::triggered, [this]() {
        QMessageBox::information(this,
            QStringLiteral("操作提示"),
            QStringLiteral("📌 停靠窗口操作技巧：\n\n"
                           "1. 拖拽标题栏 → 移动停靠窗口位置\n"
                           "2. 拖出主窗口 → 变为浮动窗口\n"
                           "3. 拖到另一个停靠窗口上 → 叠加为选项卡\n"
                           "4. 双击标题栏 → 切换停靠/浮动\n"
                           "5. 点击右上角 X → 关闭停靠窗口\n"
                           "6. Ctrl+1~4 → 快速切换显示\n"
                           "7. Ctrl+D → 添加新停靠窗口\n"
                           "8. Ctrl+R → 重置布局"));
    });
}

// ============================================================
// 状态栏
// ============================================================
void MainWindow::setupStatusBar()
{
    statusBar()->showMessage(QStringLiteral("就绪 - 拖拽停靠窗口试试吧！"));
}

// ============================================================
// 文件浏览器停靠窗口
// ============================================================
QDockWidget *MainWindow::createFileBrowser()
{
    QDockWidget *dock = new QDockWidget(QStringLiteral("文件浏览器"), this);
    dock->setObjectName("FileBrowserDock");
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setFeatures(QDockWidget::DockWidgetMovable
                      | QDockWidget::DockWidgetClosable
                      | QDockWidget::DockWidgetFloatable);
    dock->setMinimumWidth(220);

    // 使用 QTreeWidget 模拟文件树
    QTreeWidget *tree = new QTreeWidget(dock);
    tree->setHeaderLabels({ QStringLiteral("名称"), QStringLiteral("大小"), QStringLiteral("类型") });
    tree->header()->setStretchLastSection(false);
    tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    // 根节点
    QTreeWidgetItem *root = new QTreeWidgetItem(tree, { QStringLiteral("项目文件") });
    root->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));

    QTreeWidgetItem *src = new QTreeWidgetItem(root, { QStringLiteral("src"), "", QStringLiteral("文件夹") });
    src->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
    src->addChild(new QTreeWidgetItem({ QStringLiteral("main.cpp"), QStringLiteral("1.2 KB"), QStringLiteral("C++源文件") }));
    src->addChild(new QTreeWidgetItem({ QStringLiteral("MainWindow.cpp"), QStringLiteral("8.5 KB"), QStringLiteral("C++源文件") }));
    src->addChild(new QTreeWidgetItem({ QStringLiteral("MainWindow.h"), QStringLiteral("1.8 KB"), QStringLiteral("C++头文件") }));

    QTreeWidgetItem *res = new QTreeWidgetItem(root, { QStringLiteral("resources"), "", QStringLiteral("文件夹") });
    res->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
    res->addChild(new QTreeWidgetItem({ QStringLiteral("style.css"), QStringLiteral("2.3 KB"), QStringLiteral("CSS文件") }));
    res->addChild(new QTreeWidgetItem({ QStringLiteral("logo.png"), QStringLiteral("15.6 KB"), QStringLiteral("图片文件") }));

    QTreeWidgetItem *docs = new QTreeWidgetItem(root, { QStringLiteral("docs"), "", QStringLiteral("文件夹") });
    docs->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
    docs->addChild(new QTreeWidgetItem({ QStringLiteral("README.md"), QStringLiteral("0.8 KB"), QStringLiteral("Markdown") }));

    root->addChild(new QTreeWidgetItem({ QStringLiteral("CMakeLists.txt"), QStringLiteral("0.5 KB"), QStringLiteral("CMake文件") }));
    root->addChild(new QTreeWidgetItem({ QStringLiteral("DockWidgetDemo.qrc"), QStringLiteral("0.2 KB"), QStringLiteral("资源文件") }));

    root->setExpanded(true);

    dock->setWidget(tree);
    return dock;
}

// ============================================================
// 属性面板停靠窗口
// ============================================================
QDockWidget *MainWindow::createPropertyPanel()
{
    QDockWidget *dock = new QDockWidget(QStringLiteral("属性面板"), this);
    dock->setObjectName("PropertyPanelDock");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetMovable
                      | QDockWidget::DockWidgetClosable
                      | QDockWidget::DockWidgetFloatable);
    dock->setMinimumWidth(250);

    QWidget *container = new QWidget(dock);
    QVBoxLayout *layout = new QVBoxLayout(container);

    // 组件属性
    QGroupBox *compGroup = new QGroupBox(QStringLiteral("组件属性"), container);
    QFormLayout *compForm = new QFormLayout(compGroup);
    QLabel *nameLabel = new QLabel(QStringLiteral("MainWindow"));
    compForm->addRow(QStringLiteral("名称:"), nameLabel);
    QLabel *typeLabel = new QLabel(QStringLiteral("QMainWindow"));
    compForm->addRow(QStringLiteral("类型:"), typeLabel);
    QLabel *sizeLabel = new QLabel(QStringLiteral("1200 x 750"));
    compForm->addRow(QStringLiteral("尺寸:"), sizeLabel);
    layout->addWidget(compGroup);

    // 停靠窗口设置
    QGroupBox *dockGroup = new QGroupBox(QStringLiteral("停靠窗口设置"), container);
    QFormLayout *dockForm = new QFormLayout(dockGroup);

    QComboBox *areaCombo = new QComboBox();
    areaCombo->addItems({ QStringLiteral("所有区域"),
                          QStringLiteral("仅左/右"),
                          QStringLiteral("仅上/下"),
                          QStringLiteral("无(仅浮动)") });
    dockForm->addRow(QStringLiteral("允许区域:"), areaCombo);

    QComboBox *featureCombo = new QComboBox();
    featureCombo->addItems({ QStringLiteral("全部(默认)"),
                             QStringLiteral("可移动+可关闭"),
                             QStringLiteral("仅可移动"),
                             QStringLiteral("不可移动") });
    dockForm->addRow(QStringLiteral("功能限制:"), featureCombo);
    layout->addWidget(dockGroup);

    // 布局信息
    QGroupBox *layoutGroup = new QGroupBox(QStringLiteral("当前布局信息"), container);
    QVBoxLayout *layoutInfoLayout = new QVBoxLayout(layoutGroup);
    QLabel *dockCountLabel = new QLabel(QStringLiteral("停靠窗口数量: 4"));
    layoutInfoLayout->addWidget(dockCountLabel);
    QLabel *floatLabel = new QLabel(QStringLiteral("浮动窗口数量: 0"));
    layoutInfoLayout->addWidget(floatLabel);
    QLabel *tabLabel = new QLabel(QStringLiteral("选项卡组数量: 1"));
    layoutInfoLayout->addWidget(tabLabel);
    layout->addWidget(layoutGroup);

    layout->addStretch();
    dock->setWidget(container);
    return dock;
}

// ============================================================
// 输出日志停靠窗口
// ============================================================
QDockWidget *MainWindow::createOutputLog()
{
    QDockWidget *dock = new QDockWidget(QStringLiteral("输出日志"), this);
    dock->setObjectName("OutputLogDock");
    dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetMovable
                      | QDockWidget::DockWidgetClosable
                      | QDockWidget::DockWidgetFloatable);

    QTextEdit *logEdit = new QTextEdit(dock);
    logEdit->setReadOnly(true);
    logEdit->setObjectName("logEdit");
    logEdit->setStyleSheet(
        "QTextEdit { background-color: #1e1e1e; color: #d4d4d4; "
        "font-family: 'Consolas', 'Courier New', monospace; font-size: 12px; }"
    );
    logEdit->append(QStringLiteral("[%1] [INFO] 应用程序启动")
                    .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    logEdit->append(QStringLiteral("[%1] [INFO] 已创建 %2 个停靠窗口")
                    .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                    .arg(4));
    logEdit->append(QStringLiteral("[%1] [INFO] 初始化完成，等待用户操作...")
                    .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));

    dock->setWidget(logEdit);
    return dock;
}

// ============================================================
// 工具面板停靠窗口
// ============================================================
QDockWidget *MainWindow::createToolPalette()
{
    QDockWidget *dock = new QDockWidget(QStringLiteral("工具面板"), this);
    dock->setObjectName("ToolPaletteDock");
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setFeatures(QDockWidget::DockWidgetMovable
                      | QDockWidget::DockWidgetClosable
                      | QDockWidget::DockWidgetFloatable);
    dock->setMinimumWidth(200);

    QWidget *container = new QWidget(dock);
    QVBoxLayout *layout = new QVBoxLayout(container);

    QLabel *titleLabel = new QLabel(QStringLiteral("快捷操作"), container);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; padding: 4px;");
    layout->addWidget(titleLabel);

    QPushButton *btn1 = new QPushButton(QStringLiteral("📋 复制选中文本"), container);
    QPushButton *btn2 = new QPushButton(QStringLiteral("📄 新建文档"), container);
    QPushButton *btn3 = new QPushButton(QStringLiteral("💾 保存"), container);
    QPushButton *btn4 = new QPushButton(QStringLiteral("🔍 查找替换"), container);
    QPushButton *btn5 = new QPushButton(QStringLiteral("⚙ 设置"), container);

    layout->addWidget(btn1);
    layout->addWidget(btn2);
    layout->addWidget(btn3);
    layout->addWidget(btn4);
    layout->addWidget(btn5);
    layout->addStretch();

    connect(btn1, &QPushButton::clicked, [this]() {
        onLogMessage(QStringLiteral("复制选中文本 - 按钮被点击"));
    });
    connect(btn2, &QPushButton::clicked, [this]() {
        m_centralEdit->clear();
        onLogMessage(QStringLiteral("新建文档 - 已清空编辑区"));
    });
    connect(btn3, &QPushButton::clicked, [this]() {
        onLogMessage(QStringLiteral("保存 - 模拟保存操作完成"));
    });
    connect(btn4, &QPushButton::clicked, [this]() {
        onLogMessage(QStringLiteral("查找替换 - 功能面板已打开"));
    });
    connect(btn5, &QPushButton::clicked, [this]() {
        onLogMessage(QStringLiteral("设置 - 设置窗口已打开"));
    });

    dock->setWidget(container);
    return dock;
}

// ============================================================
// 槽函数
// ============================================================

void MainWindow::onAddDockWidget()
{
    m_dynamicDockCount++;

    QDockWidget *dock = new QDockWidget(
        QStringLiteral("动态窗口 #%1").arg(m_dynamicDockCount), this);
    dock->setObjectName(QStringLiteral("DynamicDock_%1").arg(m_dynamicDockCount));
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setFeatures(QDockWidget::DockWidgetMovable
                      | QDockWidget::DockWidgetClosable
                      | QDockWidget::DockWidgetFloatable);
    dock->setAttribute(Qt::WA_DeleteOnClose);

    // 内容
    QWidget *widget = new QWidget(dock);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    QLabel *label = new QLabel(
        QStringLiteral("这是动态创建的第 %1 个停靠窗口\n\n"
                       "编号: %1\n"
                       "创建时间: %2\n"
                       "特性: 可停靠 / 可浮动 / 可关闭\n\n"
                       "提示: 关闭此窗口会自动销毁")
            .arg(m_dynamicDockCount)
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")),
        widget);
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    layout->addWidget(label);
    dock->setWidget(widget);

    // 添加到右侧区域
    addDockWidget(Qt::RightDockWidgetArea, dock);

    // 同步到视图菜单
    QMenu *viewMenu = nullptr;
    for (QAction *action : menuBar()->actions()) {
        if (action->menu() && action->text().contains(QStringLiteral("视图"))) {
            viewMenu = action->menu();
            break;
        }
    }
    if (viewMenu) {
        viewMenu->addAction(dock->toggleViewAction());
    }

    // 添加到主菜单下的「停靠窗口」菜单
    QMenu *dockMenu = nullptr;
    for (QAction *action : menuBar()->actions()) {
        if (action->menu() && action->text().contains(QStringLiteral("停靠窗口"))) {
            dockMenu = action->menu();
            break;
        }
    }
    if (dockMenu) {
        QAction *closeAct = new QAction(
            QStringLiteral("关闭动态窗口 #%1").arg(m_dynamicDockCount), this);
        connect(closeAct, &QAction::triggered, dock, &QDockWidget::close);
        dockMenu->addAction(closeAct);
    }

    onLogMessage(QStringLiteral("已添加动态停靠窗口 #%1").arg(m_dynamicDockCount));
    statusBar()->showMessage(
        QStringLiteral("已添加: 动态窗口 #%1").arg(m_dynamicDockCount), 3000);
}

void MainWindow::onRemoveDockWidget()
{
    // 查找最后一个动态创建的停靠窗口
    QList<QDockWidget *> docks = findChildren<QDockWidget *>();
    for (int i = docks.size() - 1; i >= 0; --i) {
        QDockWidget *dock = docks[i];
        if (dock->objectName().startsWith("DynamicDock_")) {
            QString name = dock->windowTitle();
            dock->close();
            dock->deleteLater();
            onLogMessage(QStringLiteral("已移除: %1").arg(name));
            statusBar()->showMessage(QStringLiteral("已移除: %1").arg(name), 3000);
            return;
        }
    }
    onLogMessage(QStringLiteral("[WARN] 没有可移除的动态停靠窗口"));
    statusBar()->showMessage(QStringLiteral("没有可移除的动态停靠窗口"), 3000);
}

void MainWindow::onToggleDockWidget(QDockWidget *dock)
{
    if (!dock) return;
    dock->setVisible(!dock->isVisible());
    onLogMessage(QStringLiteral("%1 - 已%2")
                 .arg(dock->windowTitle())
                 .arg(dock->isVisible() ? QStringLiteral("显示") : QStringLiteral("隐藏")));
}

void MainWindow::onLockLayout(bool locked)
{
    QList<QDockWidget *> docks = findChildren<QDockWidget *>();
    for (QDockWidget *dock : docks) {
        if (locked) {
            dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
        } else {
            dock->setFeatures(QDockWidget::DockWidgetMovable
                              | QDockWidget::DockWidgetClosable
                              | QDockWidget::DockWidgetFloatable);
        }
    }
    onLogMessage(QStringLiteral("布局已%1").arg(locked ? QStringLiteral("锁定") : QStringLiteral("解锁")));
    statusBar()->showMessage(
        QStringLiteral("布局已%1").arg(locked ? QStringLiteral("锁定") : QStringLiteral("解锁")), 3000);
}

void MainWindow::onLogMessage(const QString &msg)
{
    // 查找日志输出控件
    QTextEdit *logEdit = findChild<QTextEdit *>("logEdit");
    if (logEdit) {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        logEdit->append(QStringLiteral("[%1] %2").arg(timestamp).arg(msg));
    }
}

void MainWindow::onResetLayout()
{
    // 移除所有动态创建的停靠窗口（名称不含以下固定名称的）
    QStringList fixedNames = { "FileBrowserDock", "PropertyPanelDock",
                               "OutputLogDock", "ToolPaletteDock" };
    QList<QDockWidget *> docks = findChildren<QDockWidget *>();
    for (QDockWidget *dock : docks) {
        if (!fixedNames.contains(dock->objectName())) {
            removeDockWidget(dock);
            dock->deleteLater();
        }
    }

    // 确保四个固有停靠窗口可见
    m_fileDock->setVisible(true);
    m_propertyDock->setVisible(true);
    m_outputDock->setVisible(true);
    m_toolDock->setVisible(true);

    // 恢复默认位置
    removeDockWidget(m_fileDock);
    removeDockWidget(m_propertyDock);
    removeDockWidget(m_outputDock);
    removeDockWidget(m_toolDock);

    addDockWidget(Qt::LeftDockWidgetArea, m_fileDock);
    addDockWidget(Qt::RightDockWidgetArea, m_propertyDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_outputDock);

    addDockWidget(Qt::RightDockWidgetArea, m_toolDock);
    tabifyDockWidget(m_propertyDock, m_toolDock);
    m_propertyDock->raise();

    m_dynamicDockCount = 0;

    onLogMessage(QStringLiteral("布局已重置为默认"));
    statusBar()->showMessage(QStringLiteral("布局已重置"), 3000);
}
