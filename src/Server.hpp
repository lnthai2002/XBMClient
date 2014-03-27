/*
 * Server.hpp
 *
 *  Created on: 2014-03-25
 *      Author: nhut_le
 */

#ifndef SERVER_HPP_
#define SERVER_HPP_
#include <QString>
class Server : public QObject{
	Q_OBJECT;
	Q_PROPERTY(QString host READ host NOTIFY valueChanged);
	Q_PROPERTY(QString port READ port NOTIFY valueChanged);
	Q_PROPERTY(QString username READ username NOTIFY valueChanged);
	Q_PROPERTY(QString password READ password NOTIFY valueChanged);
public:
	Server(QString h, QString po, QString u, QString pa);
	virtual ~Server();
	QString json_url();
	QString host();
	QString port();
	QString username();
	QString password();
signals:
	void valueChanged(QString);

private:
	QString host_;
	QString port_;
	QString username_;
	QString password_;
};


#endif /* SERVER_HPP_ */
