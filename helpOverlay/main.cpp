#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QToolButton>
#include <QAction>
#include <QSplitter>
#include <QCheckBox>
#include <QHeaderView>
#include <QMessageBox>
#include "HelpOverlay.h"

// ─── 模拟一个真实的数据管理应用 ───
class DataManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataManagerWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        setWindowTitle(QStringLiteral("数据管理平台 - DataManager v2.0"));
        resize(1200, 750);

        setupMenuBar();
        setupCentralWidget();
        setupDockWidgets();
        setupStatusBar();

        setStyleSheet(R"(
            QMainWindow { background-color: #f0f2f5; }
            QMenuBar { background: #2c3e50; color: white; padding: 2px; }
            QMenuBar::item { padding: 6px 14px; }
            QMenuBar::item:selected { background: #3498db; }
            QMenu { background: white; border: 1px solid #ddd; }
            QMenu::item { padding: 6px 30px; }
            QMenu::item:selected { background: #3498db; color: white; }
            QToolBar { background: #ffffff; border-bottom: 1px solid #e0e0e0; spacing: 4px; padding: 3px; }
            QToolBar QToolButton { padding: 5px 10px; border-radius: 3px; }
            QToolBar QToolButton:hover { background: #e8edf2; }
            QStatusBar { background: #2c3e50; color: #aaa; border-top: none; }
            QStatusBar QLabel { color: #ddd; padding: 0 8px; }
            QDockWidget { background: #ffffff; }
            QDockWidget::title { background: #e8edf2; padding: 6px 10px;
                border: 1px solid #d0d5db; font-weight: bold; }
            QTreeWidget, QListWidget, QTableWidget, QTextEdit {
                border: 1px solid #d0d5db; border-radius: 3px;
                background: white; alternate-background-color: #f7f9fc;
            }
            QGroupBox { font-weight: bold; border: 1px solid #d0d5db;
                border-radius: 5px; margin-top: 10px; padding: 10px 0 0 0; }
            QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }
            QPushButton { padding: 6px 16px; border-radius: 4px; }
            QLineEdit, QSpinBox, QComboBox { padding: 5px 8px; border: 1px solid #d0d5db;
                border-radius: 3px; background: white; }
            QLineEdit:focus, QSpinBox:focus { border-color: #3498db; }
            QComboBox::drop-down { border: none; width: 24px; }
        )");
    }

private:
    void setupMenuBar()
    {
        QMenu *fileMenu = menuBar()->addMenu(QStringLiteral("文件(&F)"));
        fileMenu->addAction(QStringLiteral("新建项目..."));
        fileMenu->addAction(QStringLiteral("打开项目..."));
        fileMenu->addSeparator();
        fileMenu->addAction(QStringLiteral("导入数据..."));
        fileMenu->addAction(QStringLiteral("导出数据..."));
        fileMenu->addSeparator();
        fileMenu->addAction(QStringLiteral("退出(&Q)"));

        QMenu *editMenu = menuBar()->addMenu(QStringLiteral("编辑(&E)"));
        editMenu->addAction(QStringLiteral("撤销"));
        editMenu->addAction(QStringLiteral("重做"));
        editMenu->addSeparator();
        editMenu->addAction(QStringLiteral("查找..."));
        editMenu->addAction(QStringLiteral("替换..."));

        QMenu *viewMenu = menuBar()->addMenu(QStringLiteral("视图(&V)"));
        viewMenu->addAction(QStringLiteral("数据面板"));
        viewMenu->addAction(QStringLiteral("属性面板"));
        viewMenu->addAction(QStringLiteral("日志面板"));

        QMenu *helpMenu = menuBar()->addMenu(QStringLiteral("帮助(&H)"));
        QAction *tourAction = helpMenu->addAction(QStringLiteral("功能引导"));
        helpMenu->addAction(QStringLiteral("关于"));
        connect(tourAction, &QAction::triggered, this, &DataManagerWindow::startGuidedTour);
    }

    void setupCentralWidget()
    {
        // 中央区域：表格 + 编辑区域
        QWidget *central = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(central);
        mainLayout->setContentsMargins(6, 6, 6, 6);

        // ── 顶部搜索栏 ──
        m_searchBar = new QWidget(central);
        m_searchBar->setObjectName(QStringLiteral("searchBar"));
        m_searchBar->setStyleSheet(QStringLiteral(
            "#searchBar { background: white; border-radius: 5px; padding: 8px; border: 1px solid #e0e0e0; }"
        ));
        QHBoxLayout *searchLayout = new QHBoxLayout(m_searchBar);
        searchLayout->setContentsMargins(8, 4, 8, 4);

        QLabel *searchIcon = new QLabel(QStringLiteral("🔍"));
        searchLayout->addWidget(searchIcon);

        m_searchInput = new QLineEdit(m_searchBar);
        m_searchInput->setObjectName(QStringLiteral("searchInput"));
        m_searchInput->setPlaceholderText(QStringLiteral("搜索项目名称、ID 或标签..."));
        m_searchInput->setFixedWidth(320);
        searchLayout->addWidget(m_searchInput);

        m_filterCombo = new QComboBox(m_searchBar);
        m_filterCombo->setObjectName(QStringLiteral("filterCombo"));
        m_filterCombo->addItems({QStringLiteral("全部类型"), QStringLiteral("数据集"),
                                 QStringLiteral("模型"), QStringLiteral("报告")});
        searchLayout->addWidget(m_filterCombo);

        searchLayout->addStretch();

        m_addBtn = new QPushButton(QStringLiteral("+ 新建项目"), m_searchBar);
        m_addBtn->setObjectName(QStringLiteral("addBtn"));
        m_addBtn->setStyleSheet(QStringLiteral(
            "QPushButton { background-color: #3498db; color: white;"
            "  font-weight: bold; padding: 6px 18px; border: none; }"
            "QPushButton:hover { background-color: #2980b9; }"
        ));
        searchLayout->addWidget(m_addBtn);

        mainLayout->addWidget(m_searchBar);

        // ── 主表格 ──
        m_dataTable = new QTableWidget(20, 5, central);
        m_dataTable->setObjectName(QStringLiteral("dataTable"));
        m_dataTable->setHorizontalHeaderLabels({
            QStringLiteral("ID"), QStringLiteral("项目名称"),
            QStringLiteral("类型"), QStringLiteral("状态"), QStringLiteral("更新时间")
        });
        m_dataTable->horizontalHeader()->setStretchLastSection(true);
        m_dataTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        m_dataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_dataTable->setAlternatingRowColors(true);
        m_dataTable->verticalHeader()->setVisible(false);

        // 填充测试数据
        QStringList types = {QStringLiteral("数据集"), QStringLiteral("模型"),
                             QStringLiteral("报告"), QStringLiteral("脚本")};
        QStringList statuses = {QStringLiteral("运行中"), QStringLiteral("已完成"),
                                QStringLiteral("待处理"), QStringLiteral("失败")};
        for (int i = 0; i < 20; ++i)
        {
            m_dataTable->setItem(i, 0, new QTableWidgetItem(QStringLiteral("PRJ-%1").arg(1000 + i, 4, 10, QChar('0'))));
            m_dataTable->setItem(i, 1, new QTableWidgetItem(QStringLiteral("数据分析项目 #%1").arg(i + 1)));
            m_dataTable->setItem(i, 2, new QTableWidgetItem(types[i % types.size()]));
            m_dataTable->setItem(i, 3, new QTableWidgetItem(statuses[i % statuses.size()]));
            m_dataTable->setItem(i, 4, new QTableWidgetItem(QStringLiteral("2026-07-%1 14:30").arg(10 + (i % 7), 2, 10, QChar('0'))));
        }
        mainLayout->addWidget(m_dataTable);

        // ── 底部编辑表单 ──
        m_editGroup = new QGroupBox(QStringLiteral("项目详情"), central);
        m_editGroup->setObjectName(QStringLiteral("editGroup"));
        QFormLayout *formLayout = new QFormLayout(m_editGroup);
        formLayout->setSpacing(8);

        m_nameEdit = new QLineEdit(m_editGroup);
        m_nameEdit->setObjectName(QStringLiteral("nameEdit"));
        m_nameEdit->setPlaceholderText(QStringLiteral("输入项目名称..."));
        formLayout->addRow(QStringLiteral("项目名称:"), m_nameEdit);

        m_typeCombo = new QComboBox(m_editGroup);
        m_typeCombo->setObjectName(QStringLiteral("typeCombo"));
        m_typeCombo->addItems({QStringLiteral("数据集"), QStringLiteral("模型"),
                               QStringLiteral("报告"), QStringLiteral("脚本")});
        formLayout->addRow(QStringLiteral("类型:"), m_typeCombo);

        QHBoxLayout *btnRow = new QHBoxLayout();
        m_saveBtn = new QPushButton(QStringLiteral("保存"), m_editGroup);
        m_saveBtn->setObjectName(QStringLiteral("saveBtn"));
        m_saveBtn->setStyleSheet(QStringLiteral(
            "QPushButton { background: #27ae60; color: white; font-weight: bold; } "
            "QPushButton:hover { background: #2ecc71; }"
        ));
        btnRow->addWidget(m_saveBtn);

        m_resetBtn = new QPushButton(QStringLiteral("重置"), m_editGroup);
        m_resetBtn->setObjectName(QStringLiteral("resetBtn"));
        m_resetBtn->setStyleSheet(QStringLiteral(
            "QPushButton { background: #e0e0e0; } QPushButton:hover { background: #ccc; }"
        ));
        btnRow->addWidget(m_resetBtn);
        btnRow->addStretch();
        formLayout->addRow(QStringLiteral(""), btnRow);

        mainLayout->addWidget(m_editGroup);
        setCentralWidget(central);
    }

    void setupDockWidgets()
    {
        // ── 左侧：数据目录树 ──
        m_navDock = new QDockWidget(QStringLiteral("数据目录"), this);
        m_navDock->setObjectName(QStringLiteral("navDock"));
        m_navDock->setMinimumWidth(200);
        m_navTree = new QTreeWidget(m_navDock);
        m_navTree->setObjectName(QStringLiteral("navTree"));
        m_navTree->setHeaderLabel(QStringLiteral("目录"));

        auto *rootItem = new QTreeWidgetItem(m_navTree, {QStringLiteral("DataManager")});
        auto *datasets = new QTreeWidgetItem(rootItem, {QStringLiteral("数据集")});
        datasets->addChild(new QTreeWidgetItem({QStringLiteral("用户行为数据")}));
        datasets->addChild(new QTreeWidgetItem({QStringLiteral("销售流水")}));
        datasets->addChild(new QTreeWidgetItem({QStringLiteral("日志分析数据")}));
        auto *models = new QTreeWidgetItem(rootItem, {QStringLiteral("模型")});
        models->addChild(new QTreeWidgetItem({QStringLiteral("推荐模型 v3.2")}));
        models->addChild(new QTreeWidgetItem({QStringLiteral("预测模型 v1.0")}));
        auto *reports = new QTreeWidgetItem(rootItem, {QStringLiteral("报告")});
        reports->addChild(new QTreeWidgetItem({QStringLiteral("周报 - 2026 W28")}));
        reports->addChild(new QTreeWidgetItem({QStringLiteral("月报 - 2026年7月")}));
        m_navTree->expandAll();

        m_navDock->setWidget(m_navTree);
        addDockWidget(Qt::LeftDockWidgetArea, m_navDock);

        // ── 右侧：属性面板 ──
        m_propDock = new QDockWidget(QStringLiteral("属性面板"), this);
        m_propDock->setObjectName(QStringLiteral("propDock"));
        m_propDock->setMinimumWidth(220);

        QWidget *propWidget = new QWidget(m_propDock);
        QVBoxLayout *propLayout = new QVBoxLayout(propWidget);

        QLabel *propTitle = new QLabel(QStringLiteral("项目属性"), propWidget);
        propTitle->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 14px; padding: 4px 0;"));
        propLayout->addWidget(propTitle);

        m_propList = new QListWidget(propWidget);
        m_propList->setObjectName(QStringLiteral("propList"));
        m_propList->addItem(QStringLiteral("创建者: Admin"));
        m_propList->addItem(QStringLiteral("创建时间: 2026-07-10"));
        m_propList->addItem(QStringLiteral("大小: 2.4 GB"));
        m_propList->addItem(QStringLiteral("标签: 生产环境, 高优先级"));
        m_propList->addItem(QStringLiteral("权限: 读写"));
        propLayout->addWidget(m_propList);

        QCheckBox *autoSync = new QCheckBox(QStringLiteral("启用自动同步"), propWidget);
        autoSync->setObjectName(QStringLiteral("autoSync"));
        propLayout->addWidget(autoSync);

        m_prioritySpin = new QSpinBox(propWidget);
        m_prioritySpin->setObjectName(QStringLiteral("prioritySpin"));
        m_prioritySpin->setPrefix(QStringLiteral("优先级: "));
        m_prioritySpin->setRange(1, 10);
        m_prioritySpin->setValue(5);
        propLayout->addWidget(m_prioritySpin);

        propLayout->addStretch();
        m_propDock->setWidget(propWidget);
        addDockWidget(Qt::RightDockWidgetArea, m_propDock);

        // ── 底部：日志输出 ──
        m_logDock = new QDockWidget(QStringLiteral("运行日志"), this);
        m_logDock->setObjectName(QStringLiteral("logDock"));
        m_logEdit = new QTextEdit(m_logDock);
        m_logEdit->setObjectName(QStringLiteral("logEdit"));
        m_logEdit->setReadOnly(true);
        m_logEdit->setStyleSheet(QStringLiteral(
            "QTextEdit { background: #1e1e1e; color: #d4d4d4; font-family: Consolas, monospace; }"
        ));
        m_logEdit->setPlainText(
            QStringLiteral("[10:30:15] [INFO]  服务已启动，端口 8080\n"
                           "[10:30:16] [INFO]  数据库连接成功 (MySQL 8.0)\n"
                           "[10:30:16] [INFO]  加载配置完成: 3 个数据源, 2 个模型\n"
                           "[10:30:17] [INFO]  缓存预热完成，耗时 1.2s\n"
                           "[10:30:20] [WARN]  磁盘使用率: 78%\n"
                           "[10:31:05] [INFO]  定时任务「数据同步」执行完成"));
        m_logDock->setWidget(m_logEdit);
        addDockWidget(Qt::BottomDockWidgetArea, m_logDock);
    }

    void setupStatusBar()
    {
        m_statusLabel = new QLabel(QStringLiteral("就绪"));
        m_statusLabel->setObjectName(QStringLiteral("statusLabel"));
        statusBar()->addWidget(m_statusLabel);

        m_connLabel = new QLabel(QStringLiteral("已连接 | 数据库: prod_main | 用户: Admin"));
        m_connLabel->setObjectName(QStringLiteral("connLabel"));
        statusBar()->addPermanentWidget(m_connLabel);
    }

    // ─── 启动引导 ───
    void startGuidedTour()
    {
        GuidedTourManager *tour = new GuidedTourManager(this);

        // 步骤 1: 菜单栏
        tour->addStep(
            nullptr,
            QStringLiteral("欢迎使用 DataManager v2.0"),
            QStringLiteral("这是一个功能强大的数据管理平台。本次引导将带你了解主要功能区域。\n\n"
                           "点击「下一步」开始了解各个模块。"),
            TourStep::Center
        );

        // 步骤 2: 左侧数据目录
        tour->addStep(
            m_navTree,
            QStringLiteral("数据目录"),
            QStringLiteral("左侧面板是「数据目录」树，这里以树形结构组织你的所有数据资源。\n\n"
                           "包含：「数据集」「模型」「报告」三大类，选中任意节点可查看详情。"),
            TourStep::Right
        );

        // 步骤 3: 搜索栏
        tour->addStep(
            m_searchBar,
            QStringLiteral("搜索与筛选"),
            QStringLiteral("顶部搜索栏支持按名称、ID 或标签进行全局搜索。\n"
                           "旁边的下拉菜单可筛选数据类型（全部/数据集/模型/报告）。\n\n"
                           "试试输入关键词搜索吧！"),
            TourStep::Bottom
        );

        // 步骤 4: 新建按钮
        tour->addStep(
            m_addBtn,
            QStringLiteral("创建新项目"),
            QStringLiteral("点击「+ 新建项目」按钮可以快速创建新的数据处理项目。\n\n"
                           "支持创建数据集项目、模型训练任务、分析报告等多种类型。"),
            TourStep::Bottom
        );

        // 步骤 5: 主数据表格
        tour->addStep(
            m_dataTable,
            QStringLiteral("项目列表"),
            QStringLiteral("中央区域是项目列表表格，展示所有项目的关键信息：\n"
                           "• ID：项目唯一编号\n"
                           "• 项目名称：项目的可读名称\n"
                           "• 类型：数据集/模型/报告/脚本\n"
                           "• 状态：运行中/已完成/待处理/失败\n"
                           "• 更新时间：最后修改时间\n\n"
                           "点击表头可以排序，双击某行可查看详情。"),
            TourStep::Top
        );

        // 步骤 6: 项目详情编辑区
        tour->addStep(
            m_editGroup,
            QStringLiteral("项目详情编辑"),
            QStringLiteral("选中表格中的某一行后，可以在下方「项目详情」区域编辑项目信息。\n\n"
                           "修改后点击绿色的「保存」按钮提交更改，或点击「重置」恢复原值。"),
            TourStep::Top
        );

        // 步骤 7: 右侧属性面板
        tour->addStep(
            m_propDock,
            QStringLiteral("属性面板"),
            QStringLiteral("右侧「属性面板」显示当前选中项目的详细属性信息：\n"
                           "• 创建者、创建时间等元信息\n"
                           "• 项目大小和存储路径\n"
                           "• 标签和权限配置\n\n"
                           "可以勾选「启用自动同步」让数据保持最新。"),
            TourStep::Left
        );

        // 步骤 8: 底部日志
        tour->addStep(
            m_logDock,
            QStringLiteral("运行日志"),
            QStringLiteral("底部面板是「运行日志」，实时显示系统运行状态。\n\n"
                           "日志按级别着色：INFO(信息)/WARN(警告)/ERROR(错误)。\n"
                           "你可以在日志中看到任务执行情况、数据库连接状态等信息。"),
            TourStep::Top
        );

        // 步骤 9: 状态栏
        tour->addStep(
            m_statusLabel,
            QStringLiteral("系统状态栏"),
            QStringLiteral("最底部是系统状态栏：\n"
                           "• 左侧显示当前操作状态\n"
                           "• 右侧显示数据库连接信息和当前用户\n\n"
                           "你可以随时查看连接状态或切换用户。"),
            TourStep::Top
        );

        // 步骤 10: 完成
        tour->addStep(
            m_searchInput,
            QStringLiteral("引导完成！"),
            QStringLiteral("你已经了解了 DataManager 的所有核心功能区域。\n\n"
                           "💡 提示：随时可以通过菜单「帮助 → 功能引导」重新查看此引导。\n\n"
                           "现在，开始探索你的数据吧！"),
            TourStep::Bottom
        );

        connect(tour, &GuidedTourManager::destroyed, tour, &QObject::deleteLater);
        tour->start();
    }

    // ─── Widget 指针（用于引导定位） ───
    QWidget *m_searchBar = nullptr;
    QLineEdit *m_searchInput = nullptr;
    QComboBox *m_filterCombo = nullptr;
    QPushButton *m_addBtn = nullptr;
    QTableWidget *m_dataTable = nullptr;
    QGroupBox *m_editGroup = nullptr;
    QLineEdit *m_nameEdit = nullptr;
    QComboBox *m_typeCombo = nullptr;
    QPushButton *m_saveBtn = nullptr;
    QPushButton *m_resetBtn = nullptr;
    QDockWidget *m_navDock = nullptr;
    QTreeWidget *m_navTree = nullptr;
    QDockWidget *m_propDock = nullptr;
    QListWidget *m_propList = nullptr;
    QSpinBox *m_prioritySpin = nullptr;
    QDockWidget *m_logDock = nullptr;
    QTextEdit *m_logEdit = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_connLabel = nullptr;
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("HelpOverlayDemo"));

    DataManagerWindow mainWindow;
    mainWindow.show();

    // 应用启动后自动弹出引导
    QTimer::singleShot(500, &mainWindow, [&mainWindow]() {
        // 通过菜单触发引导（或者直接创建一个引导实例）
        // 这里自动打开：找到 "帮助→功能引导" 并触发
        QMessageBox msgBox(&mainWindow);
        msgBox.setWindowTitle(QStringLiteral("首次使用"));
        msgBox.setText(QStringLiteral("欢迎首次使用 DataManager！\n\n"
                                       "是否需要查看功能引导来快速了解系统？"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.button(QMessageBox::Yes)->setText(QStringLiteral("开始引导"));
        msgBox.button(QMessageBox::No)->setText(QStringLiteral("跳过"));

        if (msgBox.exec() == QMessageBox::Yes)
        {
            // 调用 DataManagerWindow 的 startGuidedTour
            // 通过查找 "帮助" 菜单中的 "功能引导" action 触发
            QMenuBar *mb = mainWindow.menuBar();
            for (QAction *action : mb->actions())
            {
                if (action->menu())
                {
                    for (QAction *subAction : action->menu()->actions())
                    {
                        if (subAction->text() == QStringLiteral("功能引导"))
                        {
                            subAction->trigger();
                            return;
                        }
                    }
                }
            }
        }
    });

    return app.exec();
}
