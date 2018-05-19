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
#include <QtNetwork/QHostInfo>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QGroupBox>
#include <QtCore/QSettings>

Client::Client(QWidget* parent) : QDialog(parent), hostCombo(new QComboBox), portLineEdit(new QLineEdit)
                                  , getEventsButton(new QPushButton(tr("Get Events"))), connectToServerButton(
                new QPushButton(tr("Connect to server"))), sendEventButton(new QPushButton(tr("Send Event"))), socket(
                new QTcpSocket(this)) {


    add_loc_host_to_Combo();


    portLineEdit->setValidator(new QIntValidator(1, 65535, this));

    auto hostLabel = new QLabel(tr("&Server name:"));
    hostLabel->setBuddy(hostCombo);
    auto portLabel = new QLabel(tr("S&erver port:"));
    portLabel->setBuddy(portLineEdit);

    statusLabel = new QLabel(tr("Make sure you are connected to the server. "));

    getEventsButton->setDefault(true);
    getEventsButton->setEnabled(false);
    sendEventButton->setEnabled(false);

    auto quitButton = new QPushButton(tr("Quit"));

    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(getEventsButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(sendEventButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(connectToServerButton, QDialogButtonBox::ActionRole);


    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
    QGridLayout* mainLayout = nullptr;

    mainLayout = new QGridLayout(this);

    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostCombo, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    portLineEdit->setFocus();


    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::displayError);
    connect(connectToServerButton, &QAbstractButton::clicked,
            this, &Client::connectToServer);
    connect(sendEventButton, &QAbstractButton::clicked,
            this, &Client::sendEvent);
    connect(hostCombo, &QComboBox::editTextChanged,
            this, &Client::enableGetEventsButton);
    connect(portLineEdit, &QLineEdit::textChanged,
            this, &Client::enableGetEventsButton);
//    connect(, &QAbstractButton::clicked,
//            this, &Client::requestNewEvents);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);
    connect(socket, &QIODevice::readyRead, this, &Client::readResponse);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &Client::displayError);


}


void Client::sendEvent() {


    rrepro::Request request;
    rrepro::Event event;
    event.set_text("sdfsf");
    request.set_allocated_event(&event);
    request.set_kind(rrepro::Request::ADD);

    std::string output;
    request.SerializeToString(&output);
    std::cerr << "sendRequest with binary data: " << output;
    socket->write(output.data());


}

void Client::readResponse() {

    rrepro::Response response;


    QByteArray input = socket->readAll();

    response.ParseFromArray(input, input.size());

#ifdef degug_cerr
    std::cerr<<"Code "<<response.kind();
#endif


}

void Client::displayError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, tr("Events Client"), tr("The host was not found. Please check the "
                                                                    "host name and port settings."));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, tr("Events Client"), tr("The connection was refused by the peer. "
                                                                    "Make sure the Events server is running, "
                                                                    "and check that the host name and port "
                                                                    "settings are correct."));
            break;
        default:
            QMessageBox::information(this, tr("Events Client"),
                                     tr("The following error occurred: %1.").arg(socket->errorString()));
    }

    getEventsButton->setEnabled(true);
}

void Client::enableGetEventsButton() {

}

void Client::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    statusLabel->setText(tr("This examples requires that you run the "
                            "Fortune Server example as well."));

    enableGetEventsButton();
}

void Client::connectToServer() {
    socket->abort();
    socket->connectToHost(hostCombo->currentText(), portLineEdit->text().toInt());
}

void Client::add_loc_host_to_Combo() const {// find out name of this machine
    QString name = QHostInfo::localHostName();
    if (!name.isEmpty()) {
        hostCombo->addItem(name);
        QString domain = QHostInfo::localDomainName();
        if (!domain.isEmpty())
            hostCombo->addItem(name + QChar('.') + domain);
    }
    if (name != QLatin1String("localhost"))
        hostCombo->addItem(QString("localhost"));
}





