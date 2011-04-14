/*
 * Registro.cpp
 *
 *  Created on: 05/04/2011
 *      Author: rjc
 */

#include "Registro.h"

Registro::Registro(string unString) {
  this->miString= unString;
}

Registro::Registro(unsigned int atribEntero, unsigned int unaReferencia) {
	this->atribEnteros.push_back(atribEntero);
	this->referencias.push_back(unaReferencia);
}

Registro::Registro(string unString, unsigned int atribEntero) {
	this->miString= unString;
	this->atribEnteros.push_back(atribEntero);
}

string Registro::getString(){
	return this->miString;
}

bool Registro::buscarAtribEntero(unsigned int atribEnteroBuscado){
	list<unsigned int>::iterator it= this->atribEnteros.begin();
	bool encontrado= false;
	while (it!=this->atribEnteros.end()&&!encontrado) {
		if (*it==atribEnteroBuscado) encontrado= true;
		it++;
	}
	return encontrado;
}

list<unsigned int>* Registro::getAtributosEnteros(){
	return &this->atribEnteros;
}

list<unsigned int>* Registro::getReferencias(){
	return &this->referencias;
}

void Registro::agregarAtribEntero (unsigned int atribEntero){
  this->atribEnteros.push_back(atribEntero);
}

void Registro::agregarReferencia(unsigned int ref) {
  this->referencias.push_back(ref);
}


Registro::~Registro() {

}
