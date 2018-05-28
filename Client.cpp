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
#include <iomanip>


Client::Client(QWidget* parent) : QDialog(parent), connectToServerButton(new QPushButton(tr("Connect to server")))
                                  , getEventsButton(new QPushButton(tr("Get Events"))), sendEventButton(
                new QPushButton(tr("Send Event"))), event_des_PlainTextEdit(new QPlainTextEdit), events_table(
                new QTableWidget), hostLineEdit(new QLineEdit), portLineEdit(new QLineEdit), socket(
                new QTcpSocket(this)) {

    in.setDevice(socket);

    setSigSlots();
    setUpGUI();
    setUpNetConf();

}

/*!
 * This function creates Request of ADD kind which stores passed event.
 * It writes serialized Request to underlying socket and increments sent_events counter.
 * @param event Event to be sent.
 */
void Client::sendEvent(const rrepro::Event& event) {

    //If Client is not connected return.
    if (!isConnectedToServer())
        return;

    rrepro::Request request;

    *request.mutable_event() = event;
    request.set_kind(rrepro::Request::ADD);

    std::string output;
    request.SerializeToString(&output);

#ifdef DEBUG_MODE
    std::cout << "Serialized message is:" << output;
    std::cout << "Size of output is: " << output.size() << std::endl;
    std::cout << "sendRequest with binary data: " << output;
#endif
    socket->write(output.data());
    ++sent_events;

}

void Client::sendEvent(rrepro::Event&& event) {

    rrepro::Request request;

    *request.mutable_event() = event;
    request.set_kind(rrepro::Request::ADD);

    std::string output;
    request.SerializeToString(&output);

#ifdef DEBUG_MODE
    std::cout << "Serialized message is:" << output;
    std::cout << "Size of output is: " << output.size() << std::endl;
    std::cout << "sendRequest with binary data: " << output;
#endif


    socket->write(output.data());
    ++sent_events;

}

/*!
 * This function slot read all arrivied data and parse it.
 * It expects new events from server so it will populate events_table with them.
 */
void Client::readResponse() {

    std::cout << "Reading data..." << std::endl;


    QByteArray data;
    data = socket->readAll();


    rrepro::Response response;


    response.ParseFromArray(data, data.size());


    auto e = response.events();

    unsigned counter{};


    std::cout << e.size() << std::endl;
    for (auto&& item : e) {

#ifdef DEBUG_MODE
        std::cout << "Item text: " << item.text() << std::endl;
#endif

        //this pointer will be freed on event_table destruction.
        QTableWidgetItem* text = new QTableWidgetItem(tr("%1").arg(QString::fromStdString(item.text())));
        events_table->setItem(counter, 0, text);
        //priority

        ++counter;
    }
#ifdef DEBUG_MODE
    std::cout << "End of displaying fetched response_byte..." << std::endl;
#endif
}

#ifdef DEBUG_MODE
void Client::displayError(QAbstractSocket::SocketError socketError) {

#ifdef DEBUG_MODE
    std::cout << "Error occured!" << std::endl;
#endif

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

}
#endif

/*!
 * This function saves the used network configuration so it could be used later on
 */
void Client::sessionOpened() {

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

}


/*!
 * This function tries to connect to the host with given address and port.
 * Before it connects it aborts current connection if any exists.
 *
 */
void Client::connectToServer(QHostAddress address, int port) {

    socket->abort();
    socket->connectToHost(address, port); //TODO time out support.



}


/*!
 * This slot creates and appropriately sets request instance.
 * Then it serializes the request and sends it with QIODevice::write.
 * Finally it enables getEventsButton.
 */
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


#ifdef DEBUG_MODE
/*!
 * Auxiliary function to displays all possible states in which a given tcp socket is.
 * This function uses QAbstractSocket::SocketState enum class to identify state which affected
 * the tcp socket. Displaying information about the current state is handled by qInfo() object.
 *
 */
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
#endif


/*!
 * This slot is invoked whenever &QAbstractSocket::connected signal is emitted.
 * It enables sendEventButton and getEventsButton buttons.
 */
void Client::onConnected() {

    std::cout << "Connection was established!" << std::endl;
    getEventsButton->setEnabled(true);
    sendEventButton->setEnabled(true);

}

/*!
 * This function serves to set UP all gui widgets in user interface.
 * It sets validation in portLineEdit so it could fit port format.
 * It sets QLabels next to host and port line edits.
 * It creates appropriate layouts to lay out all visible widgets.
 */
void Client::setUpGUI() {

    portLineEdit->setValidator(new QIntValidator(1, 65535, this));
    portLineEdit->setPlaceholderText("4888");
    hostLineEdit->setPlaceholderText("127.0.0.1");

    auto hostLabel = new QLabel(tr("&Server name:"));
    hostLabel->setBuddy(hostLineEdit);
    auto portLabel = new QLabel(tr("S&erver port:"));
    portLabel->setBuddy(portLineEdit);

    statusLabel = new QLabel(tr("Make sure you are connected to the server. "));

    getEventsButton->setDefault(true);
    getEventsButton->setEnabled(false);
    sendEventButton->setEnabled(false);

    quitButton = new QPushButton(tr("Quit"));

    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(getEventsButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(connectToServerButton, QDialogButtonBox::ActionRole);

    auto buttonBox2 = new QDialogButtonBox;
    buttonBox2->addButton(sendEventButton, QDialogButtonBox::ActionRole);


    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
    QGridLayout* mainLayout = nullptr;

    QGridLayout* event_creat_layout = nullptr;
    QGridLayout* connect_server_layout = nullptr;
    QGridLayout* event_table_layout = nullptr;


    mainLayout = new QGridLayout(this);
    connect_server_layout = new QGridLayout;
    event_creat_layout = new QGridLayout;
    event_table_layout = new QGridLayout;


    events_table->setRowCount(10);
    events_table->setColumnCount(4);

    event_table_layout->addWidget(events_table);

    event_creat_layout->addWidget(buttonBox2, 1, 0);
    event_creat_layout->addWidget(event_des_PlainTextEdit, 0, 0);

    event_des_PlainTextEdit->setWindowTitle("Event description");

    connect_server_layout->addWidget(hostLabel, 0, 0);
    connect_server_layout->addWidget(hostLineEdit, 0, 1);
    connect_server_layout->addWidget(portLabel, 1, 0);
    connect_server_layout->addWidget(portLineEdit, 1, 1);
    //connect_server_layout->addWidget(statusLabel, 2, 0, 1, 2); //TODO find a place to statusLable.
    connect_server_layout->addWidget(buttonBox, 3, 0, 1, 2);


    mainLayout->addLayout(event_creat_layout, 0, 1);
    mainLayout->addLayout(connect_server_layout, 0, 0);
    mainLayout->addLayout(event_table_layout, 0, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    portLineEdit->setFocus();

}

/*!
 * \brief This auxiliary function connects necessary signal and slots.
 * It connects underlying socket, and all buttons from user graphic inteface.
 */
void Client::setSigSlots() {

    #ifdef DEBUG_MODE
    connect(socket                , QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error)        , this , &Client::displayError);
    connect(socket                , QOverload<QAbstractSocket::SocketState>::of(&QAbstractSocket::stateChanged) , this , &Client::displayState);
    #endif
    connect(socket                , &QIODevice::readyRead                                                       , this , &Client::readResponse);
    connect(socket                , &QAbstractSocket::stateChanged                                              , this , []() { std::cout << "State change" << std::endl; });
    connect(socket                , &QAbstractSocket::connected                                                 , this , &Client::onConnected);
    connect(connectToServerButton , &QAbstractButton::clicked                                                   , this , &Client::onConnectToServer);
    connect(sendEventButton       , &QAbstractButton::clicked                                                   , this , &Client::onSendEvent);
    connect(getEventsButton       , &QAbstractButton::clicked                                                   , this , &Client::askForEvents);
    connect(quitButton            , &QAbstractButton::clicked                                                   , this , &QWidget::close);

}

/*!
 * \brief Set up all necessary network configuration from last saved session.
 */
void Client::setUpNetConf() {

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {

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

/*!
 * \brief This slot is called when sendEventButton is clicked.
 * It's task is to get all important data from GUI like IP address or PORT number
 * so it could sent to sendEvent function. It also add new created event to events table.
 */
void Client::onSendEvent() {

    rrepro::Event event;
    event.set_text(event_des_PlainTextEdit->toPlainText().toStdString());
    //event.Prio

    //add to table
    QTableWidgetItem* text = new QTableWidgetItem(tr("%1").arg(QString::fromStdString(event.text())));
    int rowCount = events_table->rowCount();

#ifdef DEBUG_MODE
    std::cout << "Row count is: " << rowCount << std::endl;
#endif

    events_table->setItem(rowCount, 0, text);
    events_table->update();

    sendEvent(std::move(event));

}

/*!
 * This function is slot invoked by connectToServerButton. It fetches PORT and IP
 * to the remote server from GUI line edits and tries to connect to this server.
 * Before it connects it aborts current connection if any exists.
 * Finally it enables sendEventButton and getEventsButton buttons.
 */
void Client::onConnectToServer() {

#ifdef DEBUG_MODE
    std::cerr << "onConnectToServer()" << std::endl;
#endif

    QHostAddress address;
    address.setAddress(hostLineEdit->placeholderText());
    auto port = portLineEdit->placeholderText().toInt();

    connectToServer(address, port);

}

/*!
 * \brief This function tells us wheter a client is connected to a server.
 * @return True or false depending on actual state of underlying socket.
 */
bool Client::isConnectedToServer() {

    return socket->isOpen();
}

/*!
 * \brief Returns the current amount of sent events.
 * @return Amount of sent events.
 */
size_t Client::sentEventsCount() {

    return sent_events;
}





