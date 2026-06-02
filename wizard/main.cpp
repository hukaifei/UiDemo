#include <QApplication>
#include <QMessageBox>
#include "WizardDemo.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("WizardDemo"));
    app.setStyleSheet(
        "QWizard { background-color: #f5f7fa; }"
        "QWizard QLabel#qt_calendar_yeardateedit { qproperty-alignment: 'AlignCenter'; }"
    );

    SetupWizard wizard;

    QObject::connect(&wizard, &QWizard::accepted, [&]() {
        QString name = wizard.field("userInfo.name").toString();
        QString plan = wizard.field("config.plan").toString();

        QMessageBox::information(
            &wizard,
            QStringLiteral("向导完成"),
            QStringLiteral("用户「%1」的设置已完成！\n\n"
                           "方案序号: %2\n\n"
                           "所有配置已保存。")
                .arg(name)
                .arg(plan)
        );
    });

    wizard.show();
    return app.exec();
}
