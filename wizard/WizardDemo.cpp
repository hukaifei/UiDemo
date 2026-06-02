#include "WizardDemo.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

// ============================================================
// IntroPage - 欢迎页面
// ============================================================
IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(QStringLiteral("欢迎使用安装向导"));
    setSubTitle(QStringLiteral("本向导将引导您完成账户设置与偏好配置。"));

    auto *layout = new QVBoxLayout(this);
    auto *iconLabel = new QLabel(QStringLiteral("🚀"), this);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 64px; padding: 20px;");

    auto *descLabel = new QLabel(this);
    descLabel->setWordWrap(true);
    descLabel->setText(QStringLiteral(
        "此向导将帮助您完成以下几个步骤：\n\n"
        "  1. 填写用户信息 —— 输入您的基本资料\n"
        "  2. 配置参数选项 —— 选择适合您的方案\n"
        "  3. 确认信息 —— 核对所有已填写的内容\n"
        "  4. 完成设置 —— 保存并结束\n\n"
        "请点击「下一步」开始设置。"
    ));
    descLabel->setStyleSheet("font-size: 14px; color: #444; padding: 10px;");

    layout->addStretch();
    layout->addWidget(iconLabel);
    layout->addWidget(descLabel);
    layout->addStretch();
}

// ============================================================
// UserInfoPage - 用户信息页面
// ============================================================
UserInfoPage::UserInfoPage(QWidget *parent)
    : QWizardPage(parent), m_skipConfirm(false)
{
    setTitle(QStringLiteral("用户信息"));
    setSubTitle(QStringLiteral("请填写您的基本信息，带有 * 号的为必填项。"));

    auto *layout = new QVBoxLayout(this);

    auto *formGroup = new QGroupBox(QStringLiteral("基本信息"), this);
    auto *formLayout = new QFormLayout(formGroup);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(QStringLiteral("请输入您的姓名"));
    m_nameEdit->setClearButtonEnabled(true);
    formLayout->addRow(QStringLiteral("姓名 *:"), m_nameEdit);

    m_emailEdit = new QLineEdit(this);
    m_emailEdit->setPlaceholderText(QStringLiteral("例如: user@example.com"));
    m_emailEdit->setClearButtonEnabled(true);
    formLayout->addRow(QStringLiteral("邮箱 *:"), m_emailEdit);

    m_phoneEdit = new QLineEdit(this);
    m_phoneEdit->setPlaceholderText(QStringLiteral("例如: 13800138000"));
    m_phoneEdit->setClearButtonEnabled(true);
    QRegularExpression phoneRegex("^[0-9\\-\\+\\(\\) ]*$");
    m_phoneEdit->setValidator(new QRegularExpressionValidator(phoneRegex, this));
    formLayout->addRow(QStringLiteral("电话:"), m_phoneEdit);

    layout->addWidget(formGroup);

    auto *optionGroup = new QGroupBox(QStringLiteral("选项"), this);
    auto *optionLayout = new QVBoxLayout(optionGroup);

    auto *skipCheck = new QCheckBox(QStringLiteral("跳过确认页面，直接完成"), this);
    optionLayout->addWidget(skipCheck);

    layout->addWidget(optionGroup);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet("color: red; font-weight: bold;");
    m_errorLabel->setWordWrap(true);
    m_errorLabel->hide();
    layout->addWidget(m_errorLabel);

    layout->addStretch();

    registerField("userInfo.name*", m_nameEdit);
    registerField("userInfo.email*", m_emailEdit);
    registerField("userInfo.phone", m_phoneEdit);

    connect(m_nameEdit, &QLineEdit::textChanged, this, &UserInfoPage::validateFields);
    connect(m_emailEdit, &QLineEdit::textChanged, this, &UserInfoPage::validateFields);
    connect(skipCheck, &QCheckBox::toggled, [this](bool checked) {
        m_skipConfirm = checked;
    });
}

bool UserInfoPage::isComplete() const
{
    return !m_nameEdit->text().trimmed().isEmpty()
        && !m_emailEdit->text().trimmed().isEmpty()
        && m_emailEdit->text().contains('@')
        && m_emailEdit->text().contains('.');
}

void UserInfoPage::validateFields()
{
    QStringList errors;

    if (m_nameEdit->text().trimmed().isEmpty())
        errors << QStringLiteral("姓名不能为空");
    else if (m_nameEdit->text().trimmed().length() < 2)
        errors << QStringLiteral("姓名至少需要2个字符");

    QString email = m_emailEdit->text().trimmed();
    if (email.isEmpty())
        errors << QStringLiteral("邮箱不能为空");
    else if (!email.contains('@') || !email.contains('.'))
        errors << QStringLiteral("邮箱格式不正确");

    if (errors.isEmpty()) {
        m_errorLabel->hide();
    } else {
        m_errorLabel->setText(QStringLiteral("⚠ ") + errors.join(QStringLiteral("；")));
        m_errorLabel->show();
    }

    emit completeChanged();
}

int UserInfoPage::nextId() const
{
    if (m_skipConfirm)
        return SetupWizard::Page_Complete;
    return SetupWizard::Page_Config;
}

// ============================================================
// ConfigPage - 配置页面
// ============================================================
ConfigPage::ConfigPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(QStringLiteral("参数配置"));
    setSubTitle(QStringLiteral("选择您的偏好设置与方案。"));

    auto *layout = new QVBoxLayout(this);

    auto *planGroup = new QGroupBox(QStringLiteral("方案选择"), this);
    auto *planLayout = new QVBoxLayout(planGroup);

    m_planCombo = new QComboBox(this);
    m_planCombo->addItem(QStringLiteral("基础版 —— 免费，5GB 存储"), 0);
    m_planCombo->addItem(QStringLiteral("专业版 —— ¥29/月，50GB 存储"), 1);
    m_planCombo->addItem(QStringLiteral("企业版 —— ¥99/月，500GB 存储"), 2);
    planLayout->addWidget(m_planCombo);

    layout->addWidget(planGroup);

    auto *featureGroup = new QGroupBox(QStringLiteral("功能选项"), this);
    auto *featureLayout = new QVBoxLayout(featureGroup);

    m_backupCheck = new QCheckBox(QStringLiteral("启用自动备份"), this);
    m_backupCheck->setChecked(true);
    featureLayout->addWidget(m_backupCheck);

    m_notifyCheck = new QCheckBox(QStringLiteral("开启邮件通知"), this);
    m_notifyCheck->setChecked(true);
    featureLayout->addWidget(m_notifyCheck);

    m_encryptCheck = new QCheckBox(QStringLiteral("启用数据加密"), this);
    featureLayout->addWidget(m_encryptCheck);

    layout->addWidget(featureGroup);

    auto *storageGroup = new QGroupBox(QStringLiteral("存储配额 (GB)"), this);
    auto *storageLayout = new QHBoxLayout(storageGroup);

    m_storageSpin = new QSpinBox(this);
    m_storageSpin->setRange(1, 1000);
    m_storageSpin->setValue(10);
    m_storageSpin->setSuffix(QStringLiteral(" GB"));
    storageLayout->addWidget(m_storageSpin);
    storageLayout->addStretch();

    layout->addWidget(storageGroup);

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setWordWrap(true);
    m_summaryLabel->setStyleSheet(
        "background-color: #eef7ff; border: 1px solid #b0d4f1; "
        "border-radius: 4px; padding: 10px; font-size: 13px; color: #2c3e50;");
    layout->addWidget(m_summaryLabel);

    layout->addStretch();

    registerField("config.plan", m_planCombo, "currentIndex");
    registerField("config.backup", m_backupCheck);
    registerField("config.notify", m_notifyCheck);
    registerField("config.encrypt", m_encryptCheck);
    registerField("config.storage", m_storageSpin);

    connect(m_planCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this]() { initializePage(); });
    connect(m_backupCheck, &QCheckBox::toggled, [this]() { initializePage(); });
    connect(m_notifyCheck, &QCheckBox::toggled, [this]() { initializePage(); });
    connect(m_encryptCheck, &QCheckBox::toggled, [this]() { initializePage(); });
    connect(m_storageSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            [this]() { initializePage(); });
}

void ConfigPage::initializePage()
{
    QString planText = m_planCombo->currentText();
    QString features;
    if (m_backupCheck->isChecked()) features += QStringLiteral("✅ 自动备份\n");
    else features += QStringLiteral("❌ 自动备份\n");

    if (m_notifyCheck->isChecked()) features += QStringLiteral("✅ 邮件通知\n");
    else features += QStringLiteral("❌ 邮件通知\n");

    if (m_encryptCheck->isChecked()) features += QStringLiteral("✅ 数据加密\n");
    else features += QStringLiteral("❌ 数据加密\n");

    m_summaryLabel->setText(QStringLiteral(
        "📋 当前配置预览:\n"
        "  方案: %1\n"
        "  存储配额: %2 GB\n"
        "%3")
        .arg(planText)
        .arg(m_storageSpin->value())
        .arg(features));
}

int ConfigPage::nextId() const
{
    return SetupWizard::Page_Confirm;
}

// ============================================================
// ConfirmPage - 确认页面
// ============================================================
ConfirmPage::ConfirmPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(QStringLiteral("确认信息"));
    setSubTitle(QStringLiteral("请仔细核对以下信息，确认无误后点击「完成」。"));

    auto *layout = new QVBoxLayout(this);

    m_summaryText = new QTextEdit(this);
    m_summaryText->setReadOnly(true);
    m_summaryText->setMinimumHeight(280);
    m_summaryText->setStyleSheet(
        "QTextEdit { background-color: #fafbfc; border: 1px solid #d0d7de; "
        "border-radius: 4px; font-size: 13px; padding: 8px; }");
    layout->addWidget(m_summaryText);
}

void ConfirmPage::initializePage()
{
    QString name = field("userInfo.name").toString();
    QString email = field("userInfo.email").toString();
    QString phone = field("userInfo.phone").toString();
    if (phone.isEmpty()) phone = QStringLiteral("（未填写）");

    int planIndex = field("config.plan").toInt();
    QStringList plans = {
        QStringLiteral("基础版 —— 免费，5GB 存储"),
        QStringLiteral("专业版 —— ¥29/月，50GB 存储"),
        QStringLiteral("企业版 —— ¥99/月，500GB 存储")
    };
    QString planText = (planIndex >= 0 && planIndex < plans.size()) ? plans[planIndex] : QStringLiteral("未知");

    bool backup = field("config.backup").toBool();
    bool notify = field("config.notify").toBool();
    bool encrypt = field("config.encrypt").toBool();
    int storage = field("config.storage").toInt();

    QString summary;
    summary += QStringLiteral("══════════════════════════\n");
    summary += QStringLiteral("        用户基本信息\n");
    summary += QStringLiteral("══════════════════════════\n");
    summary += QStringLiteral("  姓名: %1\n").arg(name);
    summary += QStringLiteral("  邮箱: %1\n").arg(email);
    summary += QStringLiteral("  电话: %1\n\n").arg(phone);

    summary += QStringLiteral("══════════════════════════\n");
    summary += QStringLiteral("        配置方案\n");
    summary += QStringLiteral("══════════════════════════\n");
    summary += QStringLiteral("  方案: %1\n").arg(planText);
    summary += QStringLiteral("  存储配额: %1 GB\n").arg(storage);
    summary += QStringLiteral("  自动备份: %1\n").arg(backup ? QStringLiteral("开启") : QStringLiteral("关闭"));
    summary += QStringLiteral("  邮件通知: %1\n").arg(notify ? QStringLiteral("开启") : QStringLiteral("关闭"));
    summary += QStringLiteral("  数据加密: %1\n\n").arg(encrypt ? QStringLiteral("开启") : QStringLiteral("关闭"));

    summary += QStringLiteral("══════════════════════════\n");
    summary += QStringLiteral("点击「完成」按钮提交设置。\n");
    summary += QStringLiteral("══════════════════════════\n");

    m_summaryText->setText(summary);
}

// ============================================================
// CompletePage - 完成页面
// ============================================================
CompletePage::CompletePage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(QStringLiteral("设置完成"));
    setSubTitle(QStringLiteral("您已成功完成所有设置！"));

    auto *layout = new QVBoxLayout(this);

    auto *iconLabel = new QLabel(QStringLiteral("✅"), this);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 64px; padding: 20px;");

    auto *msgLabel = new QLabel(this);
    msgLabel->setWordWrap(true);
    msgLabel->setText(QStringLiteral(
        "恭喜！您的账户已经成功配置完毕。\n\n"
        "您现在可以开始使用各项服务了。\n\n"
        "如需修改设置，您可以随时在设置页面进行调整。\n\n"
        "感谢您的使用！"
    ));
    msgLabel->setAlignment(Qt::AlignCenter);
    msgLabel->setStyleSheet("font-size: 14px; color: #333; padding: 10px;");

    layout->addStretch();
    layout->addWidget(iconLabel);
    layout->addWidget(msgLabel);
    layout->addStretch();
}

// ============================================================
// SetupWizard - 主向导窗口
// ============================================================
SetupWizard::SetupWizard(QWidget *parent)
    : QWizard(parent)
{
    setWindowTitle(QStringLiteral("自定义向导控件 Demo"));
    resize(620, 520);

    setWizardStyle(QWizard::ModernStyle);

    setOption(QWizard::IndependentPages, false);
    setOption(QWizard::NoBackButtonOnStartPage, true);
    setOption(QWizard::HaveHelpButton, true);
    setOption(QWizard::HelpButtonOnRight, true);

    QPixmap bg(1, 1);
    bg.fill(QColor("#4682B4"));
    setPixmap(QWizard::BannerPixmap, bg.scaled(620, 62, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QPixmap logoPix(1, 1);
    logoPix.fill(QColor("#f5f7fa"));
    setPixmap(QWizard::LogoPixmap, logoPix.scaled(120, 520, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    m_introPage = new IntroPage(this);
    m_userInfoPage = new UserInfoPage(this);
    m_configPage = new ConfigPage(this);
    m_confirmPage = new ConfirmPage(this);
    m_completePage = new CompletePage(this);

    setPage(Page_Intro, m_introPage);
    setPage(Page_UserInfo, m_userInfoPage);
    setPage(Page_Config, m_configPage);
    setPage(Page_Confirm, m_confirmPage);
    setPage(Page_Complete, m_completePage);

    setStartId(Page_Intro);

    connect(this, &QWizard::helpRequested, [this]() {
        int id = currentId();
        QString helpText;
        switch (id) {
        case Page_Intro:
            helpText = QStringLiteral("这是安装向导的起始页面，介绍了向导的步骤。点击下一步继续。");
            break;
        case Page_UserInfo:
            helpText = QStringLiteral("请填写姓名和邮箱（必填项）。您可以选择跳过后续确认页面直接进入完成。");
            break;
        case Page_Config:
            helpText = QStringLiteral("选择您的方案和功能选项。左侧会实时显示当前配置的预览。");
            break;
        case Page_Confirm:
            helpText = QStringLiteral("请仔细核对所有信息。确认无误后点击「完成」提交。");
            break;
        case Page_Complete:
            helpText = QStringLiteral("所有设置已完成！点击「完成」关闭向导。");
            break;
        default:
            helpText = QStringLiteral("无帮助信息。");
            break;
        }
        QMessageBox::information(this, QStringLiteral("帮助"), helpText);
    });

    setButtonText(QWizard::NextButton, QStringLiteral("下一步 →"));
    setButtonText(QWizard::BackButton, QStringLiteral("← 上一步"));
    setButtonText(QWizard::FinishButton, QStringLiteral("完成 ✓"));
    setButtonText(QWizard::CancelButton, QStringLiteral("取消"));
    setButtonText(QWizard::HelpButton, QStringLiteral("帮助 ?"));
}
