/*
 * Server.hpp
 *
 *  Created on: 2014-03-25
 *      Author: nhut_le
 */

#ifndef SERVER_HPP_
#define SERVER_HPP_
#include <QString>
#include <QObject>
#include <QMetaType>
class Server : public QObject{
	Q_OBJECT;
	Q_PROPERTY(QString name READ name WRITE setName() NOTIFY valueChanged);
	Q_PROPERTY(QString host READ host WRITE setHost() NOTIFY valueChanged);
	Q_PROPERTY(QString port READ port WRITE setPort() NOTIFY valueChanged);
	Q_PROPERTY(QString username READ username WRITE setUsername() NOTIFY valueChanged);
	Q_PROPERTY(QString password READ password WRITE setPassword() NOTIFY valueChanged);
public:
	Server(QString h, QString po, QString u, QString pa);
	virtual ~Server();
	QString json_url();
	QString name();
	QString host();
	QString port();
	QString username();
	QString password();
	void setName(QString &n);
	void setHost(QString &h);
	void setPort(QString &po);
	void setUsername(QString &u);
	void setPassword(QString &pa);
signals:
	void valueChanged(QString);

private:
	QString name_;
	QString host_;
	QString port_;
	QString username_;
	QString password_;
};

//typedef QHash<QString, QString>* Server;
//Q_DECLARE_METATYPE(Server)

#endif /* SERVER_HPP_ */
