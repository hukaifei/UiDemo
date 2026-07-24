#include <QApplication>
#include "HttpFileClient.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    HttpFileClient client;
    client.show();
    return app.exec();
}
