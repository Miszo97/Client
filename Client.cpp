//
// Created by Artur Spek on 18/05/2018.
//

#include "Client.hpp"
#include "Request_Response.pb.h"
#include <QtWidgets/QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QtWidgets/QLabel>
#include <QtNetwork/QNetworkSession>
#include <QAbstractSocket>
#include <QMessageBox>

Client::Client(QWidget* parent) : QDialog(parent), hostCombo(new QComboBox), portLineEdit(new QLineEdit)
                                  , getFortuneButton(new QPushButton(tr("Get Fortune"))), socket(new QTcpSocket(this)) {
    socket->abort();
    socket->connectToHost(hostCombo->currentText(), portLineEdit->text().toInt());

    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &Client::displayError);


}

void Client::sendRequest() {

}

void Client::readResponse() {

}

void Client::displayError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, tr("Fortune Client"), tr("The host was not found. Please check the "
                                                                    "host name and port settings."));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, tr("Fortune Client"), tr("The connection was refused by the peer. "
                                                                    "Make sure the fortune server is running, "
                                                                    "and check that the host name and port "
                                                                    "settings are correct."));
            break;
        default:
            QMessageBox::information(this, tr("Fortune Client"),
                                     tr("The following error occurred: %1.").arg(socket->errorString()));
    }

    getFortuneButton->setEnabled(true);
}

void Client::enableGetFortuneButton() {

}

void Client::sessionOpened() {

}
