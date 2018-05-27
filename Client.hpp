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
    explicit Client(QWidget *parent = nullptr);

    void sendEvent(const rrepro::Event&);
    void sendEvent(rrepro::Event&&);

    void connectToServer(QHostAddress, int);





private slots:


    void onSendEvent();
    void onConnectToServer();

    void askForEvents();
    void readResponse();
    void sessionOpened();
    void onConnected();


    /*!
     * /brief Auxilary function to display possible errors affecting given tcp socket.
     */
    void displayError(QAbstractSocket::SocketError socketError);

    /*!
 * /brief Auxilary function to display states in which tcpSocket can be.
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

};



#endif //CLIENT_CLIENT_HPP
