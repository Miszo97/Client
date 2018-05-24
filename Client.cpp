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
#include <QtNetwork/QNetworkConfigurationManager>

Client::Client(QWidget* parent) : QDialog(parent), hostCombo(new QComboBox), portLineEdit(new QLineEdit)
                                  , getEventsButton(new QPushButton(tr("Get Events"))), connectToServerButton(
                new QPushButton(tr("Connect to server"))), sendEventButton(new QPushButton(tr("Send Event"))), event_des_PlainTextEdit(new QPlainTextEdit), socket(
                new QTcpSocket(this)) {




    //<editor-fold desc="GUI">


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
    event_des_PlainTextEdit->setWindowTitle("Event description");

    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostCombo, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    portLineEdit->setFocus();
    //</editor-fold>


    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::displayError);
    connect(socket, QOverload<QAbstractSocket::SocketState>::of(&QAbstractSocket::stateChanged), this,
            &Client::displayState);
    connect(connectToServerButton, &QAbstractButton::clicked,
            this, &Client::connectToServer);
    connect(sendEventButton, &QAbstractButton::clicked,
            this, &Client::sendEvent);
    connect(hostCombo, &QComboBox::editTextChanged,
            this, &Client::enableGetEventsButton);
    connect(portLineEdit, &QLineEdit::textChanged,
            this, &Client::enableGetEventsButton);
    connect(getEventsButton, &QAbstractButton::clicked,
            this, &Client::askForEvents);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);
    connect(socket, &QIODevice::readyRead, this, &Client::readResponse);
    connect(socket, &QAbstractSocket::stateChanged, this, []() { std::cout << "State change" << std::endl; });
    connect(socket, &QAbstractSocket::connected, this, []() { std::cout << "Connected!" << std::endl; });


    std::cout << "BEFORE manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired" << std::endl;

    QNetworkConfigurationManager manager;

    std::cout << "manager.capabilities();" << manager.capabilities() << std::endl;
    std::cout << "QNetworkConfigurationManager::NetworkSessionRequired;"
              << QNetworkConfigurationManager::NetworkSessionRequired << std::endl;

    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        std::cout << "WORKS! manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired"
                  << std::endl;

        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, &QNetworkSession::opened, this, &Client::sessionOpened);

        getEventsButton->setEnabled(false);
        statusLabel->setText(tr("Opening network session."));
        networkSession->open();
    }



}


void Client::sendEvent() {


    rrepro::Request request;
    rrepro::Event* event = new rrepro::Event;
    event->set_text("Some funny text.");
    event->set_timestamp(12);
    //event->set_priority TODO set the priority

    request.set_allocated_event(event);
    request.set_kind(rrepro::Request::ADD);

    std::string output;
    request.SerializeToString(&output);
    std::cout << "sendRequest with binary data: " << output;
    socket->write(output.data());


}

void Client::readResponse() {

    rrepro::Response response;


    QByteArray input = socket->readAll();

    response.ParseFromArray(input, input.size());

#ifdef degug_cerr
    std::cerr<<"Code "<<response.kind()<<std::endl;
#endif


}

void Client::displayError(QAbstractSocket::SocketError socketError) {

    std::cout << "Error occured!" << std::endl;

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
    getEventsButton->setEnabled(true);
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


/*!
 * This function is slot invoked by connectToServerButton. It fetches PORT and IP
 * to the remote server from GUI line edits and tries to connect to this server.
 * Before it connects it aborts current connection if any exists.
 * Finally it enables sendEventButton and getEventsButton buttons.
 */
void Client::connectToServer() {

    QHostAddress address;
    address.setAddress("127.0.0.1");
    std::cerr << "connectToServer" << std::endl;
    socket->abort();
    socket->connectToHost(address, 4888);

    enableGetEventsButton();
    sendEventButton->setEnabled(true);
}

void Client::askForEvents() {

    rrepro::Request request;

    //Kind is GET to fetch all events.
    request.set_kind(rrepro::Request::GET);

    std::string output;
    request.SerializeToString(&output);
    std::cout << "sendRequest with binary data: " << output;
    socket->write(output.data());

    getEventsButton->setEnabled(false);



}




void Client::displayState(QAbstractSocket::SocketState socketState) {


    switch (socketState) {
        case QAbstractSocket::UnconnectedState    :
            std::cout << "The socket is not connected." << std::endl;
            break;
        case QAbstractSocket::HostLookupState    :
            std::cout << "The socket is performing a host name lookup." << std::endl;
            break;
        case QAbstractSocket::ConnectingState:
            std::cout << "The socket has started establishing a connection." << std::endl;
            break;
        case QAbstractSocket::ConnectedState:
            std::cout << "A connection is established." << std::endl;
            break;
        case QAbstractSocket::BoundState    :
            std::cout << "The socket is bound to an address and port." << std::endl;
            break;
        case QAbstractSocket::ClosingState        :
            std::cout << "The socket is about to close (data may still be waiting to be written)." << std::endl;
            break;
        case QAbstractSocket::ListeningState    :
            std::cout << "For internal use only." << std::endl;
            break;
        default:;
    }



}




