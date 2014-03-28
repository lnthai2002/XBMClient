/*
 * InvokedApp.hpp
 *
 *  Created on: 2014-03-27
 *      Author: nhut_le
 */

#ifndef INVOKEDAPP_HPP_
#define INVOKEDAPP_HPP_

#include <bb/system/InvokeRequest>
#include <QObject>
#include <QString>
#include <QPointer>
#include "Server.hpp"

class InvokedApp : public QObject
{
    // Classes that inherit from QObject must have the Q_OBJECT macro so
    // the meta-object compiler (MOC) can add supporting code to the application.
    Q_OBJECT

public:
	InvokedApp(QPointer<Server> s);
	~InvokedApp();
	void playOnServer(const bb::system::InvokeRequest& invoke);

private:
	QPointer<Server> server;

	QString idFromUrl(QString &url);
};


#endif /* INVOKEDAPP_HPP_ */
