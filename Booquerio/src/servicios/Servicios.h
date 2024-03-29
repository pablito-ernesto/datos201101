/*
 * Servicios.h
 *
 *  Created on: 22/04/2011
 *      Author: pablo
 */

#ifndef SERVICIOS_H_
#define SERVICIOS_H_

#define ERROR_RUTA_INVALIDA -20
#define ERROR_RUTA_ARCHIVO_LIBROS -40
#define ERROR_RUTA_BMAS_PRIMARIO -50
#define ERROR_LIBRO_INEXISTENTE -60

#define INDICE_AUTORES 0
#define INDICE_EDITORIALES 1
#define INDICE_TITULOS 2
#define INDICE_PALABRAS 3

#define NOMBRE_HASH_TITULOS "hash_titulos"
#define NOMBRE_BMAS_AUTORES "bmas_autores"
#define NOMBRE_BMAS_EDITORIALES "bmas_editoriales"
#define NOMBRE_BMAS_PALABRAS "bmas_palabras"

#define TAMANIO_BLOQUE_BMAS 1024
#define TAMANIO_BLOQUE_BMAS_NUMERICO 1024


#include <string>
#include <stdlib.h>
#include "ListasIds.h"
#include "../parser/Parser.h"
#include "../archivos/Libro.h"
#include "../servicios/SinIndice.h"
#include "../archivos/ArchivoLibros.h"
#include "../hash/Hash.h"
#include "../ArbolB+/ArbolBMasAlfabetico.h"
#include "../ArbolB+/ArbolBMasNumerico.h"
#include "../ProcesadorConsultas/procesadorConsultas.h"

using namespace std;

class Servicios {
public:

	/*
	 * Resuelve el comando de toma de
	 * textos nuevos
	 * si la ruta no es absoluta lo resuelve
	 * como relativa segun el archivo "config.propiedades"
	 * y el path_libros
	 */
	static int tomarTexto(string ruta);
	/*
	 * Resuelve el comando procesamiento del indice
	 * por autores
	 */
	static int procesarAutores();
	/*
	 * Resuelve el comando procesamiento del indice
	 * por editoriales
	 */
	static int procesarEditoriales();
	/*
	 * Resuelve el comando procesamiento del indice
	 * por titulos
	 */
	static int procesarTitulos();
	/*
	 * Resuelve el comando procesamiento del indice
	 * por palabras
	 */
	static int procesarPalabras();

	/*
	 * Lista por pantalla los libros
	 * que estan almacenados esten o no
	 * indexados
	 */
	static int listarLibros();


	/*
	 *Recrea en un txt el libro
	 *solicitado
	 */
	static int obtenerLibro(string unId);


	/*
	 * Elimina un libro de los libros
	 * disponibles (reg variables, indices,etc)
	 */
	static int quitarArchivo(string unId);

	/*
	 * Lista las estructuras del indice de
	 * Autores
	 */
	static int verEstructuraAutor(string path);

	/*
	 * Lista las estructuras del indice de
	 * Editoriales
	 */
	static int verEstructuraEditorial(string path);

	/*
	 * Lista las estructuras del indice de
	 * Titulos
	 */
	static int verEstructuraTitulos(string path);

	/*
	 * Lista las estructuras del indice de
	 * Palabras
	 */
	static int verEstructuraPalabras(string path);

	static int verEstructuraPrimario(string path);

	static int agregarVarios(string cuantos);

	static int consultarEditorial(string editorialBuscada);

	static int consultarAutor(string autorBuscado);

	static int consultarTitulo(string tituloBuscado);

	static int consultarPalabras(string palabrasBuscadas);

	static int actualizarNormasInf();

	static int verEstructArchTerminos(string path);

	static int verEstructArchNormasInf(string path);

	static int verEstructArchOcurrPos(string path, unsigned int nroArbol);

	static int verEstructListasInvert(string path);



private:
	static void agregarAlHash(string nombreHash, string clave, unsigned int idNueva);
	static void agregarAlArbol(string nombreArbol, string clave, unsigned int idNueva);
	static void sacarDelHash(string nombreHash, string clave, unsigned int idLibro);
	static void sacarDelArbol(string nombreArbol, string clave, unsigned int idLibro);
	static int procesarLibro(int indice);
	static int agregarIndiceAutores(Libro *unLibro);
	static int agregarIndiceEditoriales(Libro *unLibro);
	static int agregarIndiceTitulos(Libro *unLibro);
	static int agregarIndicePalabras(Libro *unLibro);
	static int recuperarLibro(unsigned int idLibro, Libro **libro);

};

#endif /* SERVICIOS_H_ */
