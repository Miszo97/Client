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
#include "Request_Response.pb.h"



class Client : public QDialog
{
Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);


private slots:

    void sendEvent();
    void askForEvents();
    void readResponse();
    void sessionOpened();
    void connectToServer();
    void connection_established();


    void displayError(QAbstractSocket::SocketError socketError);
    void displayState(QAbstractSocket::SocketState socketState);

private:


    QLineEdit *hostLineEdit = nullptr;
    QLineEdit *portLineEdit = nullptr;
    QLabel *statusLabel = nullptr;
    QPlainTextEdit* event_des_PlainTextEdit = nullptr;


    /*!
     * Push buttons allowing user to invoke essential slots.
     *
     *  Button name           | Second Header
     *  -------------         | -------------
     *  getEventsButton       | void askForEvents();
     *  sendEventButton       | void sendEvent();
     *  connectToServerButton | void connectToServer();
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

};



#endif //CLIENT_CLIENT_HPP
