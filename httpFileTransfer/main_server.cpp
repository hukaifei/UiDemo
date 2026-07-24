#include <QApplication>
#include "HttpFileServer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    HttpFileServer server;
    server.show();
    return app.exec();
}
