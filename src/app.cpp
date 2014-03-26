/*
 * app.cpp
 *
 *  Created on: 2014-03-25
 *      Author: nhut_le
 */
#include "app.hpp"
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/TextField>
#include <QHash>
#include <QSettings>
using namespace bb::cascades;
App::App(QObject *parent)
    : QObject(parent)
	, m_author("Nhut Thai Le")
	, m_appName("XBMCClient")
{
    // Initialize the data model before the UI is loaded
    // and built so its ready to be used.
    loadServer();

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("_app", this);
    qml->setContextProperty("injection", this);

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    root->findChild<TextField*>("txtHost");
    Application::instance()->setScene(root);
}

void App::loadServer(){
	//TODO: right now only support 1 server, only 1 setting set. Need to allow multiple server latter, with one active
	// Get a handle on the settings object for this author and application.
	QSettings settings(m_author, m_appName);
	server = new Server(settings.value("host").toString()
						,settings.value("port").toString()
						,settings.value("username").toString()
						,settings.value("password").toString());
}
