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
#include "Request_Response.pb.h"



class Client : public QDialog
{
Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);


private slots:

    void sendEvent();
    void readResponse();
    void displayError(QAbstractSocket::SocketError socketError);

    void displayState(QAbstractSocket::SocketState socketState);

    void enableGetEventsButton();
    void sessionOpened();

    void connectToServer();

private:


    QComboBox *hostCombo = nullptr;
    QLineEdit *portLineEdit = nullptr;
    QLabel *statusLabel = nullptr;
    QPushButton* getEventsButton = nullptr;
    QPushButton* sendEventButton = nullptr;
    QPushButton* connectToServerButton = nullptr;



    QDataStream in;
    QTcpSocket* socket = nullptr;

    QNetworkSession *networkSession = nullptr;

    void add_loc_host_to_Combo() const;
};



#endif //CLIENT_CLIENT_HPP
