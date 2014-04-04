/*
 * InvokedApp.hpp
 *
 *  Created on: 2014-03-27
 *      Author: nhut_le
 */

#ifndef INVOKEDAPP_HPP_
#define INVOKEDAPP_HPP_

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QString>
#include <QPointer>
#include <QSslError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <bb/cascades/AbstractPane>
#include <bb/data/JsonDataAccess>
#include "Server.hpp"

using namespace bb::cascades;
using namespace bb::data;

class InvokedApp : public QObject
{
    // Classes that inherit from QObject must have the Q_OBJECT macro so
    // the meta-object compiler (MOC) can add supporting code to the application.
    Q_OBJECT

public:
	InvokedApp(QPointer<Server> s);
	~InvokedApp();
	void initUI();
	void playOnServer(const QString& url);
	QString idFromUrl(const QString &url);
private:
	static const QString URLPATTERN;
	QPointer<Server> server;
	QPointer<QNetworkAccessManager> netManager;
	QString vidId;
	AbstractPane *root;
	JsonDataAccess jda;

	void getActivePlayers();
	void clearPlaylist();
	void queueItem(QString &listId);
	void openPlayer();
private slots:
	void onGetActivePlayersFinished();
	void onClearListFinished();
	void onQueueItemFinished();
	void onOpenPlayerFinished();
	void slotError(QNetworkReply::NetworkError err);
	void slotSslErrors(QList<QSslError> errs);
};


#endif /* INVOKEDAPP_HPP_ */
