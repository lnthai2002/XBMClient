/*
 * app.hpp
 *
 *  Created on: 2014-03-25
 *      Author: nhut_le
 */

#ifndef APP_HPP
#define APP_HPP

#include <QObject>
#include <QString>
#include <QPointer>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeManager>
#include "Server.hpp"

class App : public QObject
{
    // Classes that inherit from QObject must have the Q_OBJECT macro so
    // the meta-object compiler (MOC) can add supporting code to the application.
    Q_OBJECT

public:
    // Creates a new App object
	App(QObject *parent = 0);

	// destroys the App object
	~App();

public slots:
	void onInvoke(const bb::system::InvokeRequest& invoke);
	void sendUrlToYoutube();

private:
	void registerServer();
	QPointer<Server> loadServer();
	QString idFromUrl(QString &url);

	bb::system::InvokeManager *invokeManager;
	QPointer<Server> server;
	// for creating settings
	const QString m_author;
	const QString m_appName;
};



#endif /* APP_HPP_ */
