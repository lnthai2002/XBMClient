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
#include <QNetworkReply>
#include <QRegExp>
#include <QStringList>

//static constant
const QString InvokedApp::URLPATTERN = ".*(?:(?:youtu.be\\/)|(?:v\\/)|(?:\\/u\\/\\w\\/)|(?:embed\\/)|(?:watch\\?))\\??v?=?([^#\\&\\?]*).*";

InvokedApp::InvokedApp(QPointer<Server> s)
	: server(s)
	, netManager(new QNetworkAccessManager(this))

{
}

InvokedApp::~InvokedApp(){
	delete netManager;
	delete server;
}

QString InvokedApp::idFromUrl(const QString &url){
	QRegExp rx(URLPATTERN);
	rx.setPatternSyntax(QRegExp::RegExp2);
	rx.indexIn(url, 0);

/*TODO: move this to a test
	QString test[12];
	test[0] = "https://www.youtube.com/watch?v=2E21VJe9fqc";
	test[1] = "http://www.youtube.com/sandalsResorts#p/c/54B8C800269D7C1B/0/FJUvudQsKCM";	//fail
	test[2] = "http://www.youtube.com/user/Scobleizer#p/u/1/1p3vcRhsYGo";
	test[3] = "http://youtu.be/NLqAF9hrVbY";
	test[4] = "http://www.youtube.com/embed/NLqAF9hrVbY";
	test[5] = "https://www.youtube.com/embed/NLqAF9hrVbY";
	test[6] = "http://www.youtube.com/v/NLqAF9hrVbY?fs=1&hl=en_US";
	test[7] = "http://www.youtube.com/watch?v=NLqAF9hrVbY";
	test[8] = "http://www.youtube.com/user/Scobleizer#p/u/1/1p3vcRhsYGo";
	test[9] = "http://www.youtube.com/ytscreeningroom?v=NRHVzbJVx8I";						//fail
	test[10] = "http://www.youtube.com/user/Scobleizer#p/u/1/1p3vcRhsYGo";
	test[11] = "http://www.youtube.com/watch?v=JYArUl0TzhA&feature=featured";
	int t = 0;
	while (t < test->size()){
		rx.indexIn(test[t], 0);
		qDebug() << "found " << rx.cap(1) << " in " << test[t];
		t++;
	}
*/
	return rx.cap(1);
}

void InvokedApp::playOnServer(const QString &url){
	QString vidId = idFromUrl(url);

	//Need to send 3 jsons
	clearList = "{\"jsonrpc\": \"2.0\", \"method\": \"Playlist.Clear\", \"params\":{\"playlistid\":1}, \"id\": 1}";
	addSong = "{\"jsonrpc\": \"2.0\", \"method\": \"Playlist.Add\", \"params\":{\"playlistid\":1, \"item\" :{ \"file\" : \"plugin://plugin.video.youtube/?action=play_video&videoid=";
	addSong.append(vidId);
	addSong.append("\"}}, \"id\" : 1}");
	openPlayer = "{\"jsonrpc\": \"2.0\", \"method\": \"Player.Open\", \"params\":{\"item\":{\"playlistid\":1, \"position\" : 0}}, \"id\": 1}";

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	request.setRawHeader("Content-Type", "application/json");

	bool res;
	Q_UNUSED(res);
	res = connect(netManager, SIGNAL(finished(QNetworkReply*)),
	              this, SLOT(onClearListFinished()));
	Q_ASSERT(res);

	netReply = netManager->post(request, clearList);
/*
	connect(netReply, SIGNAL(error(QNetworkReply::NetworkError)),
			this, SLOT(slotError(QNetworkReply::NetworkError)));
	connect(netReply, SIGNAL(sslErrors(QList<QSslError>)),
			this, SLOT(slotSslErrors(QList<QSslError>)));
*/
}

void InvokedApp::onClearListFinished(){
	if (netReply != NULL &&	netReply->bytesAvailable() > 0 &&
		netReply->error() == QNetworkReply::NoError)
	{
		//JsonDataAccess jda;
		//QVariant list = jda.loadFromBuffer(netReply->readAll());
		QNetworkRequest request;
		request.setUrl(QUrl(server->json_url()));
		request.setRawHeader("Content-Type", "application/json");

		//use handle for addSong instead of handle for clearList
		bool res;
		Q_UNUSED(res);
		res = disconnect(netManager, SIGNAL(finished(QNetworkReply*)),
			             this, SLOT(onClearListFinished()));
		Q_ASSERT(res);
		res = connect(netManager, SIGNAL(finished(QNetworkReply*)),
				this, SLOT(onAddSongFinished()));
		Q_ASSERT(res);

		while (!netReply->isFinished())
			qDebug() << "waiting for reply from clearing list";
		delete netReply;
		netReply = netManager->post(request, addSong);
	}
	else
	{
		qDebug() << "In onClearListFinished error";
		qDebug() << netReply->readAll();
		noError = false;
		netReply->deleteLater();
	}
}

void InvokedApp::onAddSongFinished(){
	if (netReply != NULL &&	netReply->bytesAvailable() > 0 &&
		netReply->error() == QNetworkReply::NoError)
	{
		QNetworkRequest request;
		request.setUrl(QUrl(server->json_url()));
		request.setRawHeader("Content-Type", "application/json");

		bool res;
		Q_UNUSED(res);
		res = disconnect(netManager, SIGNAL(finished(QNetworkReply*)),
						this, SLOT(onAddSongFinished()));
		Q_ASSERT(res);
		res = connect(netManager, SIGNAL(finished(QNetworkReply*)),
				this, SLOT(onOpenPlayerFinished()));
		Q_ASSERT(res);

		while (!netReply->isFinished())
			qDebug() << "waiting for reply from adding song";
		delete netReply;
		netReply = netManager->post(request, openPlayer);
	}
	else
	{
		qDebug() << "In onAddSongFinished error";
		qDebug() << netReply->readAll();
		noError = false;
		netReply->deleteLater();
	}
}

void InvokedApp::onOpenPlayerFinished(){
	if (netReply != NULL &&	netReply->bytesAvailable() > 0 &&
		netReply->error() == QNetworkReply::NoError)
	{
		while (!netReply->isFinished())
			qDebug() << "waiting for reply from opening player";
		//delete netReply;
		netReply->deleteLater();
	}
	else
	{
		qDebug() << "In onOpenPlayerFinished error";
		qDebug() << netReply->readAll();
		noError = false;
		netReply->deleteLater();
	}
}

void InvokedApp::slotError(QNetworkReply::NetworkError err){

}

void InvokedApp::slotSslErrors(QList<QSslError> errs){

}


