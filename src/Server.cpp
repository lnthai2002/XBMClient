/*
 * Server.cpp
 *
 *  Created on: 2014-03-25
 *      Author: nhut_le
 */
#include "Server.hpp"

Server::Server(QString h, QString po, QString u, QString pa)
	: host_(h)
	, port_(po)
	, username_(u)
	, password_(pa)
{}

Server::~Server(){
	//TODO: destroy what?
}

QString Server::json_url(){
	return ("http://"+ username_ + ":" + password_ + "@" + host_ + ":" + port_ + "/jsonrpc");
}

QString Server::host(){
	return host_;
}

void Server::setHost(QString &h){
	host_ = h;
}

QString Server::port(){
	return port_;
}

void Server::setPort(QString &po){
	port_ = po;
}

QString Server::username(){
	return username_;
}

void Server::setUsername(QString &u){
	username_ = u;
}

QString Server::password(){
	return password_;
}

void Server::setPassword(QString &pa){
	password_ = pa;
}
