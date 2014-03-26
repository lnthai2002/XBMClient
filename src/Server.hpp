/*
 * Server.hpp
 *
 *  Created on: 2014-03-25
 *      Author: nhut_le
 */

#ifndef SERVER_HPP_
#define SERVER_HPP_

class Server {
public:
	Server(QString h, QString po, QString u, QString pa);
	virtual ~Server();
	QString url();
	QString username();
	QString password();
private:
	QString host;
	QString port;
	QString user;
	QString pass;
};


#endif /* SERVER_HPP_ */
