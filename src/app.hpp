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
#include "Server.hpp"

using namespace bb::cascades;

class App : public QObject
{
    // Classes that inherit from QObject must have the Q_OBJECT macro so
    // the meta-object compiler (MOC) can add supporting code to the application.
    Q_OBJECT

    Q_PROPERTY(bb::cascades::DataModel* dataModel READ dataModel CONSTANT)

public:
    // Creates a new App object
	App(QObject *parent = 0);

	// destroys the App object
	~App();

	// Creates a new person object and saves it.
	Q_INVOKABLE bool addServer(const QString &host, const QString &port, const QString &user, const QString &pass);

	// Update settings
	Q_INVOKABLE bool updateServer(const QString &host, const QString &port, const QString &user, const QString &pass);

	Q_INVOKABLE void refreshServer();
private:
	void loadServer();

	Server *server;

	// for creating settings
	const QString m_author;
	const QString m_appName;
};



#endif /* APP_HPP_ */
