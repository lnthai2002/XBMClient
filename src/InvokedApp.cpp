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


//static constant
const QString InvokedApp::URLPATTERN = ".*(?:(?:youtu.be\\/)|(?:v\\/)|(?:\\/u\\/\\w\\/)|(?:embed\\/)|(?:watch\\?))\\??v?=?([^#\\&\\?]*).*";

InvokedApp::InvokedApp(QPointer<Server> s)
	: server(s)
	, netManager(new QNetworkAccessManager(this))
{

}

InvokedApp::~InvokedApp(){
	qDebug() << "Destructing invoked app";
	delete netManager;
	delete server;
	//rdgActions->removeAll();
}

void InvokedApp::initUI(){
	QmlDocument *qml = QmlDocument::create("asset:///card.qml").parent(this);

	root = qml->createRootObject<AbstractPane>();
	Application::instance()->setScene(root);

//	QObject::connect(root, SIGNAL(popTransitionEnded(bb::cascades::Page*)),
//	        this, SLOT(pagePopFinished(bb::cascades::Page*)));

	lblMsg = root->findChild<Label*>("lblMsg");
	rdgActions = root->findChild<RadioGroup*>("rdgActions");
	//rdgActions->removeAll();
	indBusy = root->findChild<ActivityIndicator*>("indBusy");

	bool ok = QObject::connect(this, SIGNAL(finished()),
							   qml, SLOT(deleteLater()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(getActivePlayersError()),
						this, SLOT(showActive()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(getActivePlayersFinished(QList<QVariant> &)),
						this, SLOT(showActive()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(clearListError()),
						this, SLOT(showActive()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(clearListFinished()),
						this, SLOT(showActive()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(queueItemError()),
						this, SLOT(showActive()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(queueItemFinished()),
						this, SLOT(showActive()));
	Q_ASSERT(ok);
	ok = QObject::connect(this, SIGNAL(openPlayerError()),
						this, SLOT(showActive()));
	Q_ASSERT(ok);
}

void InvokedApp::playOnServer(const QString &url){
	vidId = idFromUrl(url);
	getActivePlayers();
	bool ok = QObject::connect(this, SIGNAL(getActivePlayersFinished(QList<QVariant> &)),
							this, SLOT(showActions(QList<QVariant>&)));
	Q_ASSERT(ok);
	ok = QObject::connect(rdgActions, SIGNAL(selectedOptionChanged(bb::cascades::Option*)),
						this, SLOT(dispatch(bb::cascades::Option*)));
	Q_ASSERT(ok);
}

void InvokedApp::getActivePlayers(){
	showBusy();
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
		ok = QObject::connect(response, SIGNAL(error(QNetworkReply::NetworkError)),
							this, SLOT(slotError(QNetworkReply::NetworkError)));
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
	if (activePlayers.isEmpty()){ //check for blank
		lblMsg->setText("Nothing is playing on XBMC at the moment!");
		rdgActions->add(Option::create().objectName("optPlayNow").text("play now"));
	} else {
		for(int i = 0; i < activePlayers.size(); i++){
			QMap<QString,QVariant> player = activePlayers.at(i).toMap();
			if (player["type"].value<QString>().compare("video") == 0) {
				lblMsg->setText("XBMC is playing video, do you want to ..");
				break;
			} else if(player["type"].value<QString>().compare("audio") == 0) {
				lblMsg->setText("XBMC is playing audio, do you want to ..");
				break;
			} else {
				lblMsg->setText("XBMC is playing " + player["type"].value<QString>() + ", do you want to ..");
			}
		}
		rdgActions->add(Option::create().objectName("optPlayNow").text("stop and play this"));
		rdgActions->add(Option::create().objectName("optQueue").text("queue this"));
	}
	rdgActions->add(Option::create().objectName("optCancel").text("do nothing"));
}

void InvokedApp::dispatch(bb::cascades::Option* selectedOption){
	showBusy();
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
	}else{
		emit finished();
	}
}

//Clear playlist 1
void InvokedApp::clearPlaylist(){
	showBusy();
	QByteArray clearList = "{\"jsonrpc\": \"2.0\", \"method\": \"Playlist.Clear\", \"params\":{\"playlistid\":1}, \"id\": 1}";

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	request.setRawHeader("Content-Type", "application/json");

	QPointer<QNetworkReply> response = netManager->post(request, clearList);
	bool ok = QObject::connect(response, SIGNAL(finished()),
						this, SLOT(onClearListFinished()));
	Q_ASSERT(ok);
	ok = QObject::connect(response, SIGNAL(error(QNetworkReply::NetworkError)),
						this, SLOT(slotError(QNetworkReply::NetworkError)));
	Q_ASSERT(ok);
}

void InvokedApp::onClearListFinished(){
	QNetworkReply *response = qobject_cast<QNetworkReply *>(sender());

	if (response != NULL &&	response->bytesAvailable() > 0 &&
		response->error() == QNetworkReply::NoError)
	{
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
	showBusy();
	//TODO: use jsonDataAccess to construct the request
	QByteArray addSong = "{\"jsonrpc\": \"2.0\", \"method\": \"Playlist.Add\", \"params\":{\"playlistid\":1, \"item\" :{ \"file\" : \"plugin://plugin.video.youtube/?action=play_video&videoid=";
	addSong.append(vidId);
	addSong.append("\"}}, \"id\" : 1}");

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	request.setRawHeader("Content-Type", "application/json");

	QPointer<QNetworkReply> response = netManager->post(request, addSong);
	bool ok = connect(response, SIGNAL(finished()),
				  this, SLOT(onQueueItemFinished()));
	Q_ASSERT(ok);
	ok = QObject::connect(response, SIGNAL(error(QNetworkReply::NetworkError)),
						this, SLOT(slotError(QNetworkReply::NetworkError)));
	Q_ASSERT(ok);
}

void InvokedApp::onQueueItemFinished(){
	QNetworkReply *response = qobject_cast<QNetworkReply *>(sender());

	if (response != NULL &&	response->bytesAvailable() > 0 &&
		response->error() == QNetworkReply::NoError)
	{
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
	showBusy();
	QByteArray openPlayer = "{\"jsonrpc\": \"2.0\", \"method\": \"Player.Open\", \"params\":{\"item\":{\"playlistid\":1, \"position\" : 0}}, \"id\": 1}";

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	request.setRawHeader("Content-Type", "application/json");

	QPointer<QNetworkReply> response = netManager->post(request, openPlayer);
	bool ok = connect(response, SIGNAL(finished()),
			this, SLOT(onOpenPlayerFinished()));
	Q_ASSERT(ok);
	ok = QObject::connect(response, SIGNAL(error(QNetworkReply::NetworkError)),
						this, SLOT(slotError(QNetworkReply::NetworkError)));
	Q_ASSERT(ok);
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

void InvokedApp::getPlaylist(){
	showBusy();
	QByteArray getItems = "{\"jsonrpc\": \"2.0\", \"method\": \"Playlist.GetItems\", \"params\":{\"properties\": [ \"title\" ], \"playlistid\":1}, \"id\": 1}";

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	request.setRawHeader("Content-Type", "application/json");

	QPointer<QNetworkReply> response = netManager->post(request, getItems);
	bool ok = QObject::connect(response, SIGNAL(finished()),
						this, SLOT(onGetPlaylistFinished()));
	Q_ASSERT(ok);
	ok = QObject::connect(response, SIGNAL(error(QNetworkReply::NetworkError)),
						this, SLOT(slotError(QNetworkReply::NetworkError)));
	Q_ASSERT(ok);
}

void InvokedApp::onGetPlaylistFinished(){
	QNetworkReply *response = qobject_cast<QNetworkReply *>(sender());
	if (response != NULL &&	response->bytesAvailable() > 0 &&
		response->error() == QNetworkReply::NoError)
	{
		qDebug() << "playlist items: " <<  response->readAll();
		QVariant rep = jda.loadFromBuffer(response->readAll());

		QList<QVariant> items = rep.value<QVariantMap>()["result"].toList();
		emit playlistAvailable(items);
	} else {
		lblMsg->setText("Error occurred while getting playlist ..");
		qDebug() << response->readAll();
		emit getPlaylistError();
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

void InvokedApp::showBusy(){
	rdgActions->setEnabled(false);
	indBusy->start();
}

void InvokedApp::showActive(){
	indBusy->stop();
	rdgActions->setEnabled(true);
}

void InvokedApp::pagePopFinished(bb::cascades::Page* page){
	qDebug() << "pop page";
	delete page;
}

void InvokedApp::slotError(QNetworkReply::NetworkError err){
	lblMsg->setText("Network error!");
}

void InvokedApp::slotSslErrors(QList<QSslError> errs){

}
