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
#include <bb/system/CardDoneMessage>

//static constant
const QString InvokedApp::URLPATTERN = ".*(?:(?:youtu.be\\/)|(?:v\\/)|(?:\\/u\\/\\w\\/)|(?:embed\\/)|(?:watch\\?))\\??v?=?([^#\\&\\?]*).*";

InvokedApp::InvokedApp(QPointer<Server> s, QPointer<bb::system::InvokeManager> i)
	: server(s)
	, invokeManager(i)
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
	RadioGroup *rdgActions = root->findChild<RadioGroup*>("rdgActions");
	bool ok = QObject::connect(rdgActions, SIGNAL(selectedOptionChanged(bb::cascades::Option*)),
								this, SLOT(dispatch(bb::cascades::Option*)));
	Q_ASSERT(ok);
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
	vidId = idFromUrl(url);
	getActivePlayers();

	//TODO: load waiting screen
}

void InvokedApp::getActivePlayers(){
	QVariant activePlayersQuery = jda.load(QDir::currentPath() + "/app/native/assets/JSON/getActivePlayers.json");

	if (jda.hasError()){
		qDebug() << "got error: " << jda.error().errorMessage();
		//TODO: raise exception
	} else {
		QNetworkRequest request;
		request.setUrl(QUrl(server->json_url()));
		request.setRawHeader("Content-Type", "application/json");

		QByteArray *activePlayersQueryByteArray = new QByteArray();
		jda.saveToBuffer(activePlayersQuery, activePlayersQueryByteArray);

		QPointer<QNetworkReply> response = netManager->post(request, *activePlayersQueryByteArray);

		bool res = QObject::connect(response, SIGNAL(finished()),
									this, SLOT(onGetActivePlayersFinished()));
		Q_ASSERT(res);
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

		RadioGroup *rdgActions = root->findChild<RadioGroup*>("rdgActions");
		bool ready = true;
		if (activePlayers.isEmpty()){ //check for blank
			qDebug() << "no active player";
			lblMsg->setText("Nothing is playing on XBMC at the moment!");
			rdgActions->add(Option::create()
							.objectName("optPlayNow")
							.text("play now"));
		}else{
			QString currentPlayer;
			for(int i = 0; i < activePlayers.size(); i++){
				QMap<QString,QVariant> player = activePlayers.at(i).toMap();
				if (player["type"].value<QString>().compare("video") == 0){
					qDebug() << "there is a video playing";
					ready = false;
					lblMsg->setText("XBMC is playing video, do you want to ..");
					rdgActions->add(Option::create()
									.objectName("optPlayNow")
									.text("stop and play this"));
					rdgActions->add(Option::create()
									.objectName("optQueue")
									.text("queue this"));
					break;
				}else if(player["type"].value<QString>().compare("audio") == 0){
					qDebug() << "there is an audio playing";
					ready = false;
					lblMsg->setText("XBMC is playing audio, do you want to ..");
					rdgActions->add(Option::create()
									.objectName("optPlayNow")
									.text("stop and play this"));
					break;
				}else{
					currentPlayer = player["type"].value<QString>();
					break;
				}
			}
			if (ready){ //there is something playing, but not audio or video
				qDebug() << currentPlayer << " is playing";
				ready = false;
				lblMsg->setText("XBMC is playing " + currentPlayer + ", do you want to ..");
				rdgActions->add(Option::create()
								.objectName("optPlayNow")
								.text("stop and play this"));
			}
		}
		rdgActions->add(Option::create()
						.objectName("optCancel")
						.text("leave it as is"));
	}
	else{
		qDebug() << "Error in getting playlists: " << response->readAll();
	}
	response->deleteLater();
}

void InvokedApp::dispatch(bb::cascades::Option* selectedOption){
	qDebug() << "options selected";

	if (selectedOption->objectName() == "optPlayNow"){
		clearPlaylist();
	}else if(selectedOption->objectName() == "optQueue"){
		QString l = "1";
		queueItem(l);
	}
	// close card
	CardDoneMessage message;
	message.setData(tr("Card: I am done. yay!"));
	message.setDataType("text/plain");
	message.setReason(tr("Success!"));
	invokeManager->sendCardDone(message);
}

//Clear playlist 1
void InvokedApp::clearPlaylist(){
	QByteArray clearList = "{\"jsonrpc\": \"2.0\", \"method\": \"Playlist.Clear\", \"params\":{\"playlistid\":1}, \"id\": 1}";

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	request.setRawHeader("Content-Type", "application/json");

	bool res;
	Q_UNUSED(res);

	QPointer<QNetworkReply> response = netManager->post(request, clearList);
	res = QObject::connect(response, SIGNAL(finished()),
						this, SLOT(onClearListFinished()));
	Q_ASSERT(res);
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
		QString l = "1";
		queueItem(l);
	} else {
		qDebug() << "In onClearListFinished error";
		lblMsg->setText("Error occurred while clearing playlist ..");
		qDebug() << response->readAll();
	}
	response->deleteLater();
}

//Add to playlist 1
void InvokedApp::queueItem(QString &listId){
	//TODO: use jsonDataAccess to construct the request
	QByteArray addSong = "{\"jsonrpc\": \"2.0\", \"method\": \"Playlist.Add\", \"params\":{\"playlistid\":1, \"item\" :{ \"file\" : \"plugin://plugin.video.youtube/?action=play_video&videoid=";
	addSong.append(vidId);
	addSong.append("\"}}, \"id\" : 1}");

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	request.setRawHeader("Content-Type", "application/json");

	bool res;
	Q_UNUSED(res);
	QPointer<QNetworkReply> response = netManager->post(request, addSong);
	res = connect(response, SIGNAL(finished()),
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
		openPlayer();
	} else {
		qDebug() << "In onAddSongFinished error";
		lblMsg->setText("Error occurred while adding item to playlist ..");
		qDebug() << response->readAll();
	}
	response->deleteLater();
}

//open player and start playing list 1
void InvokedApp::openPlayer(){
	QByteArray openPlayer = "{\"jsonrpc\": \"2.0\", \"method\": \"Player.Open\", \"params\":{\"item\":{\"playlistid\":1, \"position\" : 0}}, \"id\": 1}";

	QNetworkRequest request;
	request.setUrl(QUrl(server->json_url()));
	request.setRawHeader("Content-Type", "application/json");

	bool res;
	Q_UNUSED(res);
	QPointer<QNetworkReply> response = netManager->post(request, openPlayer);
	res = connect(response, SIGNAL(finished()),
			this, SLOT(onOpenPlayerFinished()));
	Q_ASSERT(res);
}

void InvokedApp::onOpenPlayerFinished(){
	QNetworkReply *response = qobject_cast<QNetworkReply *>(sender());
	if (response != NULL &&	response->bytesAvailable() > 0 &&
		response->error() == QNetworkReply::NoError)
	{
		//TODO: completed
	} else {
		qDebug() << "In onOpenPlayerFinished error";
		lblMsg->setText("Error occurred while starting playlist ..");
		qDebug() << response->readAll();
	}
	response->deleteLater();
}

void InvokedApp::slotError(QNetworkReply::NetworkError err){

}

void InvokedApp::slotSslErrors(QList<QSslError> errs){

}
