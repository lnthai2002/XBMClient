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
#include <QNetworkReply>
#include <QRegExp>
#include <QStringList>
#include <QDir>
#include <QByteArray>
#include <QVariant>
#include <QMap>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/RadioGroup>

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

void InvokedApp::initUI(){
	QmlDocument *qml = QmlDocument::create("asset:///card.qml").parent(this);

	root = qml->createRootObject<AbstractPane>();
	Application::instance()->setScene(root);

	lblMsg = root->findChild<Label*>("lblMsg");
	indBusy = root->findChild<ActivityIndicator*>("indBusy");

	bool ok = QObject::connect(this, SIGNAL(finished()),
							   indBusy, SLOT(stop()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(getActivePlayersError()),
						indBusy, SLOT(stop()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(getActivePlayersFinished(QList<QVariant> &)),
						indBusy, SLOT(stop()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(clearListError()),
						indBusy, SLOT(stop()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(clearListFinished()),
						indBusy, SLOT(stop()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(queueItemError()),
						indBusy, SLOT(stop()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(queueItemFinished()),
						indBusy, SLOT(stop()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(openPlayerError()),
						indBusy, SLOT(stop()));
	Q_ASSERT(ok);
}

void InvokedApp::playOnServer(const QString &url){
	vidId = idFromUrl(url);
	getActivePlayers();
	bool ok = QObject::connect(this, SIGNAL(getActivePlayersFinished(QList<QVariant> &)),
							this, SLOT(showActions(QList<QVariant>&)));
	Q_ASSERT(ok);

	RadioGroup *rdgActions = root->findChild<RadioGroup*>("rdgActions");
	ok = QObject::connect(rdgActions, SIGNAL(selectedOptionChanged(bb::cascades::Option*)),
						this, SLOT(dispatch(bb::cascades::Option*)));
	Q_ASSERT(ok);
}

void InvokedApp::getActivePlayers(){
	indBusy->start();
	QVariant activePlayersQuery = jda.load(QDir::currentPath() + "/app/native/assets/JSON/getActivePlayers.json");

	if (jda.hasError()){
		qDebug() << "got error: " << jda.error().errorMessage();
		lblMsg->setText("App is corrupted, please reinstall");
		emit finished();
	} else {
		QNetworkRequest request;
		request.setUrl(QUrl(server->json_url()));
		request.setRawHeader("Content-Type", "application/json");

		QByteArray *activePlayersQueryByteArray = new QByteArray();
		jda.saveToBuffer(activePlayersQuery, activePlayersQueryByteArray);

		QPointer<QNetworkReply> response = netManager->post(request, *activePlayersQueryByteArray);

		bool ok = QObject::connect(response, SIGNAL(finished()),
									this, SLOT(onGetActivePlayersFinished()));
		Q_ASSERT(ok);
		delete activePlayersQueryByteArray;
	}
}

void InvokedApp::onGetActivePlayersFinished(){
	QNetworkReply *response = qobject_cast<QNetworkReply *>(sender());
	if (response != NULL &&	response->bytesAvailable() > 0 &&
		response->error() == QNetworkReply::NoError)
	{
		QVariant rep = jda.loadFromBuffer(response->readAll());

		QList<QVariant> activePlayers = rep.value<QVariantMap>()["result"].toList();

		emit getActivePlayersFinished(activePlayers);
	} else {
		qDebug() << "Error in getting playlists: " << response->readAll();
		lblMsg->setText("Can't get active playlist");
		emit getActivePlayersError();
	}
	response->deleteLater();
}

void InvokedApp::showActions(QList<QVariant>& activePlayers){
	RadioGroup *rdgActions = root->findChild<RadioGroup*>("rdgActions");
	bool free = true;
	if (activePlayers.isEmpty()){ //check for blank
		qDebug() << "no active player";
		lblMsg->setText("Nothing is playing on XBMC at the moment!");
		rdgActions->add(Option::create().objectName("optPlayNow").text("play now"));
	} else {
		QString currentPlayer;
		for(int i = 0; i < activePlayers.size(); i++){
			QMap<QString,QVariant> player = activePlayers.at(i).toMap();
			if (player["type"].value<QString>().compare("video") == 0) {
				qDebug() << "there is a video playing";
				free = false;
				lblMsg->setText("XBMC is playing video, do you want to ..");
				rdgActions->add(Option::create().objectName("optPlayNow").text("stop and play this"));
				rdgActions->add(Option::create().objectName("optQueue").text("queue this"));
				break;
			} else if(player["type"].value<QString>().compare("audio") == 0) {
				qDebug() << "there is an audio playing";
				free = false;
				lblMsg->setText("XBMC is playing audio, do you want to ..");
				rdgActions->add(Option::create().objectName("optPlayNow").text("stop and play this"));
				break;
			} else {
				currentPlayer = player["type"].value<QString>();
				break;
			}
		}
		if (free){ //there is something playing, but not audio or video
			qDebug() << currentPlayer << " is playing";
			lblMsg->setText("XBMC is playing " + currentPlayer + ", do you want to ..");
			rdgActions->add(Option::create().objectName("optPlayNow").text("stop and play this"));
		}
	}
}

void InvokedApp::dispatch(bb::cascades::Option* selectedOption){
	indBusy->start();
	bool ok;
	if (selectedOption->objectName() == "optPlayNow"){
		clearPlaylist();
		ok = QObject::connect(this, SIGNAL(clearListFinished()),
							this, SLOT(queueItem()));
		Q_ASSERT(ok);
		ok = QObject::connect(this, SIGNAL(queueItemFinished()),
							this, SLOT(openPlayer()));
		Q_ASSERT(ok);
		ok = QObject::connect(this, SIGNAL(openPlayerFinished()),
							this, SIGNAL(finished()));
		Q_ASSERT(ok);
	}else if (selectedOption->objectName() == "optQueue"){
		queueItem();
		ok = QObject::connect(this, SIGNAL(queueItemFinished()),
							this, SIGNAL(finished()));
		Q_ASSERT(ok);
	}
}

//Clear playlist 1
void InvokedApp::clearPlaylist(){
	indBusy->start();
	QByteArray clearList = "{\"jsonrpc\": \"2.0\", \"method\": \"Playlist.Clear\", \"params\":{\"playlistid\":1}, \"id\": 1}";

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	request.setRawHeader("Content-Type", "application/json");

	QPointer<QNetworkReply> response = netManager->post(request, clearList);
	bool ok = QObject::connect(response, SIGNAL(finished()),
						this, SLOT(onClearListFinished()));
	Q_ASSERT(ok);
}

void InvokedApp::onClearListFinished(){
	QNetworkReply *response = qobject_cast<QNetworkReply *>(sender());

	if (response != NULL &&	response->bytesAvailable() > 0 &&
		response->error() == QNetworkReply::NoError)
	{
		//use handle for addSong instead of handle for clearList
		bool res;
		Q_UNUSED(res);
		res = disconnect(response, SIGNAL(finished()),
			             this, SLOT(onClearListFinished()));
		Q_ASSERT(res);
		emit clearListFinished();
	} else {
		qDebug() << "In onClearListFinished error";
		lblMsg->setText("Error occurred while clearing playlist ..");
		qDebug() << response->readAll();
		emit clearListError();
	}
	response->deleteLater();
}

//Add to playlist 1
void InvokedApp::queueItem(){
	indBusy->start();
	//TODO: use jsonDataAccess to construct the request
	QByteArray addSong = "{\"jsonrpc\": \"2.0\", \"method\": \"Playlist.Add\", \"params\":{\"playlistid\":1, \"item\" :{ \"file\" : \"plugin://plugin.video.youtube/?action=play_video&videoid=";
	addSong.append(vidId);
	addSong.append("\"}}, \"id\" : 1}");

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	request.setRawHeader("Content-Type", "application/json");

	QPointer<QNetworkReply> response = netManager->post(request, addSong);
	bool res = connect(response, SIGNAL(finished()),
				  this, SLOT(onQueueItemFinished()));
	Q_ASSERT(res);
}

void InvokedApp::onQueueItemFinished(){
	QNetworkReply *response = qobject_cast<QNetworkReply *>(sender());

	if (response != NULL &&	response->bytesAvailable() > 0 &&
		response->error() == QNetworkReply::NoError)
	{
		bool res;
		Q_UNUSED(res);
		res = disconnect(response, SIGNAL(finished()),
						this, SLOT(onQueueItemFinished()));
		Q_ASSERT(res);
		emit queueItemFinished();
	} else {
		qDebug() << "In onAddSongFinished error";
		lblMsg->setText("Error occurred while adding item to playlist ..");
		qDebug() << response->readAll();
		emit queueItemError();
	}
	response->deleteLater();
}

//open player and start playing list 1
void InvokedApp::openPlayer(){
	indBusy->start();
	QByteArray openPlayer = "{\"jsonrpc\": \"2.0\", \"method\": \"Player.Open\", \"params\":{\"item\":{\"playlistid\":1, \"position\" : 0}}, \"id\": 1}";

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	request.setRawHeader("Content-Type", "application/json");

	QPointer<QNetworkReply> response = netManager->post(request, openPlayer);
	bool res = connect(response, SIGNAL(finished()),
			this, SLOT(onOpenPlayerFinished()));
	Q_ASSERT(res);
}

void InvokedApp::onOpenPlayerFinished(){
	QNetworkReply *response = qobject_cast<QNetworkReply *>(sender());
	if (response != NULL &&	response->bytesAvailable() > 0 &&
		response->error() == QNetworkReply::NoError)
	{
		emit openPlayerFinished();
	} else {
		qDebug() << "In onOpenPlayerFinished error";
		lblMsg->setText("Error occurred while starting playlist ..");
		qDebug() << response->readAll();
		emit openPlayerError();
	}
	response->deleteLater();
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

void InvokedApp::slotError(QNetworkReply::NetworkError err){

}

void InvokedApp::slotSslErrors(QList<QSslError> errs){

}
