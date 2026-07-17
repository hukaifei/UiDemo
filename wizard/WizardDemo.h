#ifndef WIZARDDEMO_H
#define WIZARDDEMO_H

#include <QWizard>
class QLabel;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QRadioButton;
class QSpinBox;
class QTextEdit;

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit IntroPage(QWidget *parent = nullptr);
};

class UserInfoPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit UserInfoPage(QWidget *parent = nullptr);

    bool isComplete() const override;
    int nextId() const override;

private slots:
    void validateFields();

private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_phoneEdit;
    QLabel *m_errorLabel;
    bool m_skipConfirm;
};

class ConfigPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit ConfigPage(QWidget *parent = nullptr);

    void initializePage() override;
    int nextId() const override;

private:
    QComboBox *m_planCombo;
    QCheckBox *m_backupCheck;
    QCheckBox *m_notifyCheck;
    QCheckBox *m_encryptCheck;
    QSpinBox *m_storageSpin;
    QLabel *m_summaryLabel;
};

class ConfirmPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit ConfirmPage(QWidget *parent = nullptr);

    void initializePage() override;

private:
    QTextEdit *m_summaryText;
};

class CompletePage : public QWizardPage
{
    Q_OBJECT

public:
    explicit CompletePage(QWidget *parent = nullptr);
};

class SetupWizard : public QWizard
{
    Q_OBJECT

public:
    explicit SetupWizard(QWidget *parent = nullptr);

    enum { Page_Intro, Page_UserInfo, Page_Config, Page_Confirm, Page_Complete };

private:
    IntroPage *m_introPage;
    UserInfoPage *m_userInfoPage;
    ConfigPage *m_configPage;
    ConfirmPage *m_confirmPage;
    CompletePage *m_completePage;
};

#endif // WIZARDDEMO_H
