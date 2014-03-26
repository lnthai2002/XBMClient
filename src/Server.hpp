/*
 * Server.hpp
 *
 *  Created on: 2014-03-25
 *      Author: nhut_le
 */

#ifndef SERVER_HPP_
#define SERVER_HPP_

class Server : public QObject{
	Q_OBJECT;
public:
	Server(QString h, QString po, QString u, QString pa);
	virtual ~Server();
	QString url();
	QString host;
	QString port;
	QString username;
	QString password;
};


#endif /* SERVER_HPP_ */
