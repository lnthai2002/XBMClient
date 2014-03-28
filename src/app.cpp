/*
 * app.cpp
 *
 *  Created on: 2014-03-25
 *      Author: nhut_le
 */
#include "app.hpp"
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Button>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/TextField>
#include <bb/data/JsonDataAccess>
#include <bb/system/ApplicationStartupMode>
#include <QHash>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegExp>
using namespace bb::cascades;
using namespace bb::system;

// Static constants used for storing Settings
const QString App::m_author = "Nhut Thai Le";
const QString App::m_appName= "XBMCClient";

App::App(QObject *parent)
    : QObject(parent)
	, invokeManager(new InvokeManager(this))
{
	server = loadServer();

	// If any Q_ASSERT statement(s) indicate that the slot failed to connect to
	// the signal, make sure you know exactly why this has happened. This is not
	// normal, and will cause your app to stop working!!
	bool connectResult;
	Q_UNUSED(connectResult);//avoid acompiler warning.

	connectResult = QObject::connect(invokeManager,
			SIGNAL(invoked(const bb::system::InvokeRequest&)),
			this, SLOT(onInvoke(const bb::system::InvokeRequest&)));

	// This is only available in Debug builds.
	Q_ASSERT(connectResult);

	switch(invokeManager->startupMode()) {
		case ApplicationStartupMode::LaunchApplication:
		{	//Load UI layout
			QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
			qml->setContextProperty("server", server);

			AbstractPane *root = qml->createRootObject<AbstractPane>();
			Application::instance()->setScene(root);

			Button *btnSend = root->findChild<Button*>("btnSave");
			bool res = QObject::connect(btnSend, SIGNAL(clicked()),
										this, SLOT(registerServer()));
			Q_ASSERT(res);
			break;
		}
		case ApplicationStartupMode::InvokeApplication:
		{	// If the application is invoked,
			// it must wait until it receives an invoked(..) signal
			// so that it can determine the UI that it needs to initialize
			invokedApp = new InvokedApp(server);
			break;
		}
		default:
		{	// What app is it and how did it get here?
			break;
		}
	}
}

App::~App(){
	//TODO: Whatever appear in constructor must be destroyed
	delete server;
	delete invokedApp;

}

void App::registerServer(){
	//TODO: taken from user
	QSettings settings(m_author, m_appName);
	settings.setValue("host", "192.168.1.2");
	settings.setValue("port", "8000");
	settings.setValue("username", "nhut_le");
	settings.setValue("password", "Jytcx4qr");
	settings.sync();
}

QPointer<Server> App::loadServer(){
	//TODO: right now only support 1 server, only 1 setting set. Need to allow multiple server latter, with one active
	// Get a handle on the settings object for this author and application.
	QSettings settings(m_author, m_appName);
	return new Server(settings.value("host").toString()
						,settings.value("port").toString()
						,settings.value("username").toString()
						,settings.value("password").toString());
}

void App::onInvoke(const bb::system::InvokeRequest& invoke){
	invokedApp->playOnServer(invoke);
}

QString App::idFromUrl(QString &url){
	url = "https://www.youtube.com/watch?v=2E21VJe9fqc";
	QString id = "";
	//QRegExp rx("^.*((youtu.be\/)|(v\/)|(\/u\/\w\/)|(embed\/)|(watch\?))\??v?=?([^#\&\?]*).*");
	QRegExp rx("^?*v=?*");
	rx.setPatternSyntax(QRegExp::Wildcard);

	QStringList list;
	int pos = 0;

	qDebug() << "Hello";
	qDebug() << rx.indexIn(url, 0);

	while ((pos = rx.indexIn(url, pos)) != -1) {
	 //list << rx.cap(1);
		qDebug() << rx.cap(0);
		qDebug() << rx.cap(1);
	 pos += rx.matchedLength();
	}
	return id;
}
