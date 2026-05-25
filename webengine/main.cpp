#include <QApplication>
#include <QFile>
#include "WebEngineDemo.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName(QStringLiteral("WebEngineDemo"));
    QApplication::setApplicationVersion(QStringLiteral("1.0.0"));
    QApplication::setOrganizationName(QStringLiteral("UITest"));

    QFile qssFile(":/style.css");
    if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(qssFile.readAll());
        qssFile.close();
    }

    WebEngineDemo window;
    window.show();

    return app.exec();
}
