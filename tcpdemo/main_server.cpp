#include "TcpServer.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    TcpServer server;
    server.show();
    
    return app.exec();
}
