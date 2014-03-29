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
#include "InvokedApp.hpp"

class App : public QObject
{
    // Classes that inherit from QObject must have the Q_OBJECT macro so
    // the meta-object compiler (MOC) can add supporting code to the application.
    Q_OBJECT

public:
    static const QString m_author;
    static const QString m_appName;

	App(QObject *parent = 0);
	~App();

public slots:
	void onInvoke(const bb::system::InvokeRequest& invoke);

private:
	bb::system::InvokeManager *invokeManager;
	QPointer<InvokedApp> invokedApp;
	QPointer<Server> server;

	QPointer<Server> loadServer();
private slots:
	void registerServer();
};

#endif /* APP_HPP_ */
