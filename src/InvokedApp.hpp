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
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/Label>
#include <bb/cascades/Option>
#include <bb/cascades/RadioGroup>
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
signals:
	void getActivePlayersError();
	void getActivePlayersFinished(QList<QVariant> &);
	void clearListError();
	void clearListFinished();
	void queueItemError();
	void queueItemFinished();
	void openPlayerError();
	void openPlayerFinished();
	void getPlaylistError();
	void playlistAvailable(QList<QVariant> &);
	void finished();
private:
	static const QString URLPATTERN;
	QString vidId;
	JsonDataAccess jda;
	QPointer<Server> server;
	QPointer<QNetworkAccessManager> netManager;
	QPointer<AbstractPane> root;
	QPointer<Label> lblMsg;
	//QPointer<RadioGroup> rdgActions;
	QPointer<ActivityIndicator> indBusy;

	void getActivePlayers();
	void clearPlaylist();
	QString idFromUrl(const QString &url);
private slots:
	void dispatch(bb::cascades::Option* selectedOptions);
	void showActions(QList<QVariant>& activePlayers);
	void queueItem();
	void openPlayer();
	void onGetActivePlayersFinished();
	void onClearListFinished();
	void onQueueItemFinished();
	void onOpenPlayerFinished();
	void getPlaylist();
	void onGetPlaylistFinished();
	void showActive();
	void showBusy();
	void slotError(QNetworkReply::NetworkError err);
	void slotSslErrors(QList<QSslError> errs);
};


#endif /* INVOKEDAPP_HPP_ */
