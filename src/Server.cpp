/*
 * Server.cpp
 *
 *  Created on: 2014-03-25
 *      Author: nhut_le
 */
#include "Server.hpp"

Server::Server(QString h, QString po, QString u, QString pa)
	: host(h)
	, port(po)
	, user(u)
	, pass(pa)
{}

QString Server::username(){
	return user;
}

QString Server::password(){
	return pass;
}

QString Server::url(){
	return ("http://"+ host + ":" + port);
}
