//
// Created by Artur Spek on 18/05/2018.
//

#ifndef CLIENT_CLIENT_HPP
#define CLIENT_CLIENT_HPP


#include <QDataStream>
#include <QDialog>
#include <QTcpSocket>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtNetwork/QNetworkSession>
#include <QtWidgets/QPlainTextEdit>
#include <QTableWidget>
#include "Request_Response.pb.h"



class Client : public QDialog
{
Q_OBJECT

public:

    /// \brief Create Client instance.
    explicit Client(QWidget *parent = nullptr);

    /// \brief Send single Event to connected Server. Does nothing when there is no active connection.
    void sendEvent(const rrepro::Event&);

    /// \brief Overloaded sendEvent function getting r-value as a parameter.
    void sendEvent(rrepro::Event&&);

    /// \brief Connect client to a active remote server.
    void connectToServer(QHostAddress, int);

    /// \brief This function tells us wheter a client is connected to a server.
    bool isConnectedToServer();

    /// \brief Returns the current amount of sent events.
    size_t sentEventsCount();


private slots:


    /// \brief This slot is invoked whenever a user clicks sendEventButton.
    void onSendEvent();

    /// \brief This slot is invoked whenever a user clicks connectToServerButton.
    void onConnectToServer();

    /// \brief This slot is invoked whenever a user clicks getEventsButton.
    void askForEvents();

    /// \brief This slot is invoked whenever &QIODevice::readyRead signal is emitted.
    void readResponse();

    /// \brief This slot is invoked whenever &QNetworkSession::opened signal is emitted.
    void sessionOpened();

    /// \brief This slot is invoked whenever &QAbstractSocket::connected signal is emitted.
    void onConnected();


    /*!
     * \brief Auxiliary function to display possible errors affecting given tcp socket.
     * \note This function is defined only when DEBUG_MODE macro is defined.
     */
#ifdef DEBUG_MODE
    void displayError(QAbstractSocket::SocketError socketError);
#endif

    /*!
    * \brief Auxiliary function to display states in which tcpSocket can be.
    * \note This function is defined only when DEBUG_MODE macro is defined.
    */
#ifdef DEBUG_MODE
    void displayState(QAbstractSocket::SocketState socketState);
#endif

private:


    QLineEdit *hostLineEdit = nullptr;
    QLineEdit *portLineEdit = nullptr;
    QLabel *statusLabel = nullptr;
    QPlainTextEdit* event_des_PlainTextEdit = nullptr;
    QTableWidget* events_table = nullptr;


    /*!
     * Push buttons allowing user to invoke essential slots.
     *
     *  Button name           | Slot name
     *  -------------         | -------------
     *  getEventsButton       | void askForEvents();
     *  sendEventButton       | void onSendEvent();
     *  connectToServerButton | void onConnected();
     *  quitButton            | Widget::close()
     */
    QPushButton* getEventsButton = nullptr;
    QPushButton* sendEventButton = nullptr;
    QPushButton* connectToServerButton = nullptr;
    QPushButton* quitButton = nullptr;


    std::multimap<rrepro::Event_Priority, rrepro::Event> events;
    QDataStream in;
    QTcpSocket* socket = nullptr;
    QNetworkSession *networkSession = nullptr;

    /// \brief This function serves to set UP all gui widgets in user interface.
    inline void setUpGUI();
    inline void setSigSlots();
    inline void setUpNetConf();

    size_t sent_events = {};
    bool connected;

};



#endif //CLIENT_CLIENT_HPP
