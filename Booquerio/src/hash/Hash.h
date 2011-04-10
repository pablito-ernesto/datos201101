/*
 * Hash.h
 *
 *  Created on: 09/04/2011
 *      Author: pablo
 */

#ifndef HASH_H_
#define HASH_H_

typedef unsigned int elemLista;

#define EXTENCION_TABLA ".table"
#define TAMANIO_TABLA 8
#define BYTES_TABLA TAMANIO_TABLA*sizeof(elemLista)

#include <string.h>
#include <fstream>
#include "../util/Parametros.h"
#include "../util/Util.h"
using namespace std;



class Hash {
public:
	Hash();
	virtual ~Hash();
	/*
	 * Crea un hash con el @nombre
	 */
	int crear(string nombre);
	/*
	 * Abre un hash con el @nombre
	 */
	int abrir(string nombre);
	/*
	 * Inserta un registro es pre que este
	 * abierto el hash, retorna el resutado de la
	 * operacion
	 */
	void insertar(void *registro);

	/*
	 * Borra del hash el @que
	 * si lo borra retorna ok
	 * si no lo encontro retorna
	 * error
	 *
	 */
	void borrar(string que);
	/*
	 * busca en el hash la clave @que
	 * y devuelve el registro, si no encuentra la
	 * clave devuelve null
	 */
	int buscar(string que);
private:
	fstream archivoTabla;
	string nombre;
	elemLista tamanioLista;
	elemLista *tabla;
	unsigned int offsetUltimaBusqueda;
	/*
	 * Retorna el nro de elemento en la lista
	 */
	int hasheo(string key);

};

#endif /* HASH_H_ */
