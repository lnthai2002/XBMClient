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
QString Server::port(){
	return port_;
}
QString Server::username(){
	return username_;
}
QString Server::password(){
	return password_;
}
