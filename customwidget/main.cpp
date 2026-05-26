#include <QApplication>
#include <QFile>
#include "TestWindow.h"
#include <QListView>
#include <QStringListModel>
#include <QTableView>
#include <QStandardItemModel>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile qssFile(":/style.css");
    if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(qssFile.readAll());
        qssFile.close();
    }

    TestWindow window;
    window.show();
    return app.exec();
}
