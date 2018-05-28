#define BOOST_TEST_MODULE module_test
#include <boost/test/included/unit_test.hpp>
#include <QApplication>
#include <iostream>
#include "Client.hpp"

namespace utf = boost::unit_test;


BOOST_AUTO_TEST_CASE( test_one )
{

  Client client(nullptr);
  BOOST_CHECK_EQUAL(client.isConnectedToServer(), false);
  client.connectToServer(QHostAddress("127.0.0.1"), 4888);
  BOOST_CHECK_EQUAL(client.isConnectedToServer(), true);

}






