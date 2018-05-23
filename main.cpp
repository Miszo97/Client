#include <QApplication>
#include <iostream>
#include "Client.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationDisplayName(Client::tr("Events Client"));
    Client client;
    client.show();
    return app.exec();
}