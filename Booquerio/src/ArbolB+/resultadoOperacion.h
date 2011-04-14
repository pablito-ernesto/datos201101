/*
 * resultadoOperacion.h
 *
 *  Created on: 14/04/2011
 *      Author: martin
 */

#ifndef RESULTADOOPERACION_H_
#define RESULTADOOPERACION_H_
#include <string>

using namespace std;

class resultadoOperacion {
public:
	resultadoOperacion();
	virtual ~resultadoOperacion();
	int getCodigo();
	string getDescripcion();

private:

	/*
	 * CODIGO:
	 *  1 - ok
	 *  2 - duplicado
	 *  3 - inexistente
	 *  4 - sustituto
	 *
	 * */
	int codigo;
	string descripcion;

};

#endif /* RESULTADOOPERACION_H_ */
