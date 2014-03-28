/*
 * InvokedApp.cpp
 *
 *  Created on: 2014-03-27
 *      Author: nhut_le
 */
/*
 * app.cpp
 *
 *  Created on: 2014-03-25
 *      Author: nhut_le
 */
#include "InvokedApp.hpp"
#include <bb/data/JsonDataAccess>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegExp>

InvokedApp::InvokedApp(QPointer<Server> s)
	: server(s)
{
	// If any Q_ASSERT statement(s) indicate that the slot failed to connect to
	// the signal, make sure you know exactly why this has happened. This is not
	// normal, and will cause your app to stop working!!
/*
	bool connectResult;
	Q_UNUSED(connectResult);//avoid acompiler warning.

	connectResult = QObject::connect(invokeManager,
									SIGNAL(invoked(const bb::system::InvokeRequest&)),
									this, SLOT(playOnServer(const bb::system::InvokeRequest&)));

	// This is only available in Debug builds.
	Q_ASSERT(connectResult);

	switch(invokeManager->startupMode()) {
	case ApplicationStartupMode::LaunchApplication:
	{//Load layout
		QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

		registerServer();//TODO: get rid of this

		server = loadServer();
		qml->setContextProperty("server", server);

		AbstractPane *root = qml->createRootObject<AbstractPane>();
		Application::instance()->setScene(root);

		Button *btnSend = root->findChild<Button*>("btnSend");
		bool res = QObject::connect(btnSend,
									SIGNAL(clicked()),
									this,
									SLOT(sendUrlToYoutube()));
		Q_ASSERT(res);
		break;
	}
	case ApplicationStartupMode::InvokeApplication:
	{
		// If the application is invoked,
		// it must wait until it receives an invoked(..) signal
		// so that it can determine the UI that it needs to initialize

		break;
	}
	default:
	{	// What app is it and how did it get here?
		break;
	}
	}
*/
}

InvokedApp::~InvokedApp(){
	//TODO: Whatever appear in constructor must be destroyed
	delete server;
}

void InvokedApp::playOnServer(const bb::system::InvokeRequest& invoke){
	qDebug() << "Someone called me with action " << invoke.action() <<  " uri " << invoke.uri().toString();

/*
	QString vidId = "2E21VJe9fqc"; //list.at(0);
	//Need to send 3 jsons
	QByteArray clearList = "{\"jsonrpc\": \"2.0\", \"method\": \"Playlist.Clear\", \"params\":{\"playlistid\":1}, \"id\": 1}";
	//QString addSong = "{\"jsonrpc\": \"2.0\", \"method\": \"Playlist.Add\", \"params\":{\"playlistid\":1, \"item\" :{ \"file\" : \"plugin://plugin.video.youtube/?action=play_video&videoid=" + "\'" + vidId + "\'\"}}, \"id\" : 1}";
	QString openPlayer = "{\"jsonrpc\": \"2.0\", \"method\": \"Player.Open\", \"params\":{\"item\":{\"playlistid\":1, \"position\" : 0}}, \"id\": 1}";

//	JsonDataAccess jda;
//	QVariant list = jda.loadFromBuffer(clearList);

	QNetworkAccessManager *manager = new QNetworkAccessManager(this);

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	//request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
	request.setRawHeader("Content-Type", "application/json");

	QNetworkReply *reply = manager->post(request, clearList);

	connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
			this, SLOT(slotError(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
			this, SLOT(slotSslErrors(QList<QSslError>)));

	delete reply;
	delete manager;
*/
}

QString InvokedApp::idFromUrl(QString &url){
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




