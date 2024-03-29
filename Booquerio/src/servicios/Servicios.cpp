/*
 * Servicios.cpp
 *
 *  Created on: 22/04/2011
 *      Author: pablo
 */
#define TAMANIO_BLOQUE 4096
#include "Servicios.h"
#include "../hash/Hash.h"
#include "../util/Util.h"
#include "../ProcesadorConsultas/archivoNormasInf.h"



int Servicios::tomarTexto(string ruta)
{

//parsear el libro
	Parser *unParser = new Parser();
	cout << "Procesando Libro...." << endl;
	if (unParser->parsear(ruta) != EXITOSO)
	{
		string rutaLibros = Parametros().getParametro(CARPETA_LIBROS);
		ruta = rutaLibros + ruta;
		if (unParser->parsear(ruta) != EXITOSO) return ERROR_RUTA_INVALIDA;
	}

	Libro *unLibro = unParser->getLibro();

	//cout << unLibro->toString() << endl; //BORRAR SOLO PARA PROBAR LA IMPRESION



	//cout << "parsie el libro" << endl;


//meter el libro en el archivo de registros variables
	string rutaArcLibros = Parametros().getParametro(ARCHIVO_LIBROS);

	if (rutaArcLibros == "") return ERROR_RUTA_ARCHIVO_LIBROS;

	ArchivoLibros *archivo = new ArchivoLibros(rutaArcLibros);

	//cout << "creo archivo de libros" << endl;



	unsigned int offset = 0;
	offset = archivo->agregarLibro(unLibro);
	delete archivo;

	//cout << "agrego el libro" << endl;



	string pathArbolPrimario = Parametros().getParametro(CARPETA_DATOS);
	pathArbolPrimario += NOMBRE_BMAS_PRIMARIO;

	if (pathArbolPrimario == "") return ERROR_RUTA_BMAS_PRIMARIO;

	ArbolBMasNumerico *arbolP = new ArbolBMasNumerico(pathArbolPrimario, TAMANIO_BLOQUE_BMAS_NUMERICO);
	//cout << "id: "<< unLibro->getId() << endl;
	arbolP->insertarNumerico(unLibro->getId(),offset);

	delete arbolP;

	//cout << "agrego al indice primario" << endl;


	/*******************AGREGADO ENTREGA II*************************/

	//crear el arbol de parlabras
	string pathArbolPalabras = Parametros().getParametro(CARPETA_DATOS);
	pathArbolPalabras += NOMBRE_BMAS_PALABRAS;
	pathArbolPalabras += "_" + Util().UIntToString(unLibro->getId());

	ArbolBMasAlfabetico *arbolPal = new ArbolBMasAlfabetico(pathArbolPalabras, TAMANIO_BLOQUE_BMAS_PALABRAS);

	map<string, list<int> > mapa = unParser->obtenerPalabras2();
	map<string, list<int> >::iterator itMap;
	string clave;
	unsigned int idLista;
	unsigned int posPal;
	string mejorP;
	unsigned int vieja,nueva,mejor = 0;

	//cout << "llego a insertar las cosas al arbol" << endl;
	//cin.get();

	///////////////////////  agrego la funcionalidad para guardar normas infinito   ///////

	string pathArbolPrim = Parametros().getParametro(CARPETA_DATOS);
	pathArbolPrim+= NOMBRE_BMAS_PRIMARIO;
	ArbolBMasNumerico* arbolPrimario = new ArbolBMasNumerico(pathArbolPrim,
			TAMANIO_BLOQUE_BMAS_NUMERICO);

	resultadoOperacion result(OK);
	Registro* regActual = arbolPrimario->buscarRegistroNumerico(0, &result);
	unsigned int N = 1;
	while (regActual != NULL) {
		regActual = arbolPrimario->siguiente();
		N++;
	}


	delete arbolPrimario;


	double acumuladoNormaInf=0;

	for (itMap = mapa.begin(); itMap != mapa.end(); itMap++)
	{
		clave  = itMap->first;
		posPal = itMap->second.front();
		nueva = itMap->second.size();

		list<unsigned int> listaIds;

		double aij = itMap->second.size();
		string rutahash= Parametros().getParametro(CARPETA_DATOS);
		rutahash+=NOMBRE_HASH_PALABRAS;
		Hash hash(rutahash);
		Registro* registro= hash.buscar(itMap->first);

		double ni= 1;


		if (registro!= NULL){

			offset= registro->getAtributosEnteros()->front();
			ListasIds().obtenerListaIds(offset, &listaIds);
			ni= listaIds.size();

		}

		double pesoGlobal= log10((double)N/(double)ni);
		double peso= aij*pesoGlobal;
		acumuladoNormaInf+= pow(peso,2);

		ListasIds().agregarPosPalabra(&idLista,posPal,true);
		arbolPal->insertar(clave,idLista);

		list<int>::iterator itPos = itMap->second.begin();
		itPos++; //para salir del front por que ya se cargo antes
		for ( ; itPos != itMap->second.end() ; itPos++)

		{
			posPal = *itPos;
			ListasIds().agregarPosPalabra(&idLista,posPal,false);

		}

		delete registro;
	}
	double normaInfinito= sqrt(acumuladoNormaInf);
	//cout<<normaInfinito<<endl;
	archivoNormasInf archivoNormas;
	archivoNormas.escribirNorma(normaInfinito);

	delete arbolPal;
/**********************************************borrar***************************************************/
//	list<unsigned int> *lista = new list<unsigned int>();
//	ListasIds().obtenerListaIds(1,lista);
//
//	for (list<unsigned int>::iterator it = lista->begin(); it != lista->end() ; it ++)
//		cout << *it << endl;





	/*******************AGREGADO ENTREGA II*************************/


	delete unParser;


	//agregar el libro a las listas
	SinIndice *listas = SinIndice().getInstancia();

	//ProcesadorConsultas().actualizarPesos();

	unsigned int id = unLibro->getId();

	delete unLibro;

	return listas->agregarLibroPendienteATodasLasListas(id);

	//cout << "agrego a las listas no procesadas" << endl;

}

int Servicios::procesarAutores()
{
	return procesarLibro(INDICE_AUTORES);
}


int Servicios::procesarTitulos()
{
	return procesarLibro(INDICE_TITULOS);
}


int Servicios::procesarEditoriales()
{
	return procesarLibro(INDICE_EDITORIALES);
}



int Servicios::procesarPalabras()
{
	return procesarLibro(INDICE_PALABRAS);
}


int Servicios::listarLibros()
{

	string rutaArcLibros = Parametros().getParametro(ARCHIVO_LIBROS);

	if (rutaArcLibros == "") return ERROR_RUTA_ARCHIVO_LIBROS;
	ArchivoLibros *archivo = new ArchivoLibros(rutaArcLibros);
	//list<Libro> *lista = archivo->recuperacionComprensiva(); //en vez de usar el metodo de hernan
	//uso el metodo siguiente del primario

	list<Libro> *lista = new list<Libro>;
	string pathArbolPrimario = Parametros().getParametro(CARPETA_DATOS);
	pathArbolPrimario += NOMBRE_BMAS_PRIMARIO;

	ArbolBMasNumerico* arbolPrimario = new ArbolBMasNumerico(pathArbolPrimario,TAMANIO_BLOQUE_BMAS_NUMERICO);
	//para iniciar la recuperasion secuencial busco primero el id 0 = el primero
	resultadoOperacion resultadoBus(OK);
	Registro* registroObtenido = arbolPrimario->buscarRegistroNumerico(0,&resultadoBus);

	unsigned int unId;
	Libro* unLibro = NULL;

	//armo las lista de libros con el metodo siguiente del arbol
	while (	registroObtenido != NULL){
		unId = registroObtenido->getAtributosEnteros()->front();
		recuperarLibro(unId, &unLibro);
		//agrego un libro a la lista de libros
		lista->push_back(*unLibro);
		registroObtenido = arbolPrimario->siguiente();
	}

	delete arbolPrimario;

	cout << "Listado del libros: " << endl;
	unsigned int i = 1;

	for (list<Libro>::iterator it = lista->begin(); it != lista->end(); it++)
	{
		cout<<"**************************  "<< i << "  **************************" << endl;
		cout << (*it).toString() << endl;
		i++;
	}
	cout << "Fin listado del libros: " << endl;

	delete lista;
	return 0;
}

int Servicios::obtenerLibro(string unId)
{
	unsigned int id = atoi(unId.c_str());
	fstream libro;
	string rutaSalida = Parametros().getParametro(CARPETA_SALIDA);
	string arcLibros = Parametros().getParametro(ARCHIVO_LIBROS);

	if (arcLibros == "") return ERROR_RUTA_ARCHIVO_LIBROS;

 	ArchivoLibros *archivo = new ArchivoLibros(arcLibros);
	Libro *unLibro = NULL;
	//intento recuperar el libro
	int resultadoRecuperacion = 0;
	resultadoRecuperacion = recuperarLibro(id,&unLibro);
	if( resultadoRecuperacion == ERROR_LIBRO_INEXISTENTE){
		return resultadoRecuperacion;
	};
// BORRAR
//	Parser *unParser = new Parser();
//	unParser->parsear("./archivos/libros/Justin Somper - Vampiratas 3 - Emboscada en el Océano.txt");
//	Libro *unLibro = unParser->getLibro();
//BORRAR

	string nombreArchivo = rutaSalida + unLibro->getTitulo();
	nombreArchivo += ".txt";

	libro.open(nombreArchivo.c_str(), ios::binary | ios::out);

	unsigned int tamanio;
	char* buff;
	string linea;

//autor
	linea = "Autor: ";
	linea += unLibro->getAutor();
	linea += "\n";
//editorial
	linea += "Editorial: ";
	linea += unLibro->getEditorial();
	linea += "\n";
//titulo
	linea += "Titulo: ";
	linea += unLibro->getTitulo();
	linea += "\n";
//texto
	linea += "\n\n...\n";
	linea += unLibro->getTexto();
	linea += "\n";

	tamanio = linea.size();
	buff= new char[tamanio];
	memcpy(buff,linea.c_str(),tamanio);
	libro.write(buff,tamanio);
	delete [] buff;

	libro.close();

	archivo->~ArchivoLibros();
	unLibro->~Libro();

	return 0;
}

int Servicios::quitarArchivo(string unId)
{
	unsigned int id = atoi(unId.c_str());
	unsigned int offset = 0;

	cout<<"Eliminando libro "<<id<<"..."<<endl;

	//recuperar el offset del primario

	string pathArbolPrimario = Parametros().getParametro(CARPETA_DATOS);
	pathArbolPrimario += NOMBRE_BMAS_PRIMARIO;

	if (pathArbolPrimario == "") return ERROR_RUTA_BMAS_PRIMARIO;

	ArbolBMasNumerico *arbolP = new ArbolBMasNumerico(pathArbolPrimario, TAMANIO_BLOQUE_BMAS_NUMERICO);

	resultadoOperacion resultado(OK);

	Registro* registro = arbolP->buscarRegistroNumerico(id, &resultado);

	//TODO ver si esta recuperando bien el offset
	if (registro && resultado.getDescripcion() == "ENCONTRADO")
		offset = registro->getReferencias()->front();
	else
		return ERROR_LIBRO_INEXISTENTE;


//quitar de el archivo de registros variables

	string arcLibros = Parametros().getParametro(ARCHIVO_LIBROS);
	ArchivoLibros *archivo = new ArchivoLibros(arcLibros);
	Libro *libro = archivo->recuperarLibro(offset);
//	cout <<"Autor: "<< libro->getAutor() <<endl;
//	cout <<"Editorial: "<< libro->getEditorial() <<endl;
//	cout <<"Titulo: "<< libro->getTitulo() <<endl;
//	cout <<"Id: "<< libro->getId() <<endl;
//	cout <<"Cant.palabras: " << libro->getCantPalabras() << endl;
//	cout << "Presione ENTER para continuar...." << endl;
//	cin.get();

//	cout << "SACO EL LIBRO DEL ARCHIVO DE REGISTROS VARIABLES" << endl;
	archivo->suprimirLibro(offset);


//quitar del primario

	//cout << "QUITO DEL PRIMARIO EL ID" << endl;

	arbolP->eliminarNumerico(libro->getId());

	delete arbolP;

//quitar de los indices

	//cout << "SACO DE LOS INDICES" << endl;

	//cout << "SACO DE AUTORES" << endl;
	sacarDelArbol(NOMBRE_BMAS_AUTORES,libro->getAutor(),libro->getId());
	//cout << "Presione ENTER para continuar...." << endl;
	//cin.get();

	//cout << "SACO DE EDITORIALES" << endl;
	sacarDelArbol(NOMBRE_BMAS_EDITORIALES,libro->getEditorial(),libro->getId());
	///cout << "Presione ENTER para continuar...." << endl;
	//cin.get();

	//cout << "SACO DE TITULOS" << endl;
	sacarDelHash(NOMBRE_HASH_TITULOS,libro->getTitulo(),libro->getId());
	//cout << "Presione ENTER para continuar...." << endl;
	//cin.get();

	//cout << "SACO PALABRAS" << endl;
	//cout << "Procesando la quita del indice..." << endl;
	set<string> *palabras = libro->getListaPalabras();

	for (set<string>::iterator it = palabras->begin(); it != palabras->end(); it++)
	{
		sacarDelHash(NOMBRE_HASH_PALABRAS, *it ,libro->getId());
	}


//quitar de la lista de libros sin procesar
	//cout << "SACO DE LOS INDICES SIN PROCESAR" <<endl;
	SinIndice *listas  = SinIndice().getInstancia();
	listas->sacarLibroDeTodasLasListas(id);

	delete archivo;

	/*******************AGREGADO ENTREGA II*************************/


	string pathArbolPalabras = Parametros().getParametro(CARPETA_DATOS);
	pathArbolPalabras += NOMBRE_BMAS_PALABRAS;
	pathArbolPalabras += "_" + Util().UIntToString(id);

	resultadoOperacion resultadoOp(OK);


	ArbolBMasAlfabetico *arbolPal = new ArbolBMasAlfabetico(pathArbolPalabras, TAMANIO_BLOQUE_BMAS_PALABRAS);



	//recorrer el arbol de palabras
	Registro *reg = arbolPal->buscarRegistro("a",&resultadoOp);
	unsigned int lista;

	while (reg != NULL)
	{
		lista = reg->getReferenciai(1);
		ListasIds().eliminarLista(lista);
		reg = arbolPal->siguiente();
	}

	arbolPal->~ArbolBMasAlfabetico();

	Util().eliminarArchivo(pathArbolPalabras);

	/*******************AGREGADO ENTREGA II*************************/


	return 0;


}

int Servicios::verEstructuraAutor(string path)
{
	string pathExport;
	if (path == "")
	{
		pathExport = Parametros().getParametro(CARPETA_SALIDA);
		pathExport += NOMBRE_BMAS_AUTORES;
	}
	else
	{
		pathExport = path;
	}

	string pathArbol = Parametros().getParametro(CARPETA_DATOS);
	pathArbol += NOMBRE_BMAS_AUTORES;

	ArbolBMasAlfabetico* arbolA = new ArbolBMasAlfabetico(pathArbol,TAMANIO_BLOQUE_BMAS);
	arbolA->exportar(pathExport);
	delete arbolA;
	return 0;
}

int Servicios::verEstructuraEditorial(string path)
{
	string pathExport;
	if (path == "")
	{
		pathExport = Parametros().getParametro(CARPETA_SALIDA);
		pathExport += NOMBRE_BMAS_EDITORIALES;
	}
	else
	{
		pathExport = path;
	}
	string pathArbol = Parametros().getParametro(CARPETA_DATOS);
	pathArbol += NOMBRE_BMAS_EDITORIALES;

	ArbolBMasAlfabetico* arbolE = new ArbolBMasAlfabetico(pathArbol,TAMANIO_BLOQUE_BMAS);
	arbolE->exportar(pathExport);
	delete arbolE;
	return 0;

}

int Servicios::verEstructuraPrimario(string path)
{
	string pathExport;
	if (path == "")
	{
		pathExport = Parametros().getParametro(CARPETA_SALIDA);
		pathExport += NOMBRE_BMAS_PRIMARIO;
	}
	else
	{
		pathExport = path;
	}
	string pathArbol = Parametros().getParametro(CARPETA_DATOS);
	pathArbol += NOMBRE_BMAS_PRIMARIO;

	ArbolBMasNumerico* arbolE = new ArbolBMasNumerico(pathArbol,TAMANIO_BLOQUE_BMAS);
	arbolE->exportar(pathExport);
	delete arbolE;
	return 0;


}

int Servicios::verEstructuraTitulos(string path)
{

	string pathExport;
	if (path == "")
	{
		pathExport = Parametros().getParametro(CARPETA_SALIDA);
		pathExport += NOMBRE_HASH_TITULOS;
	}
	else
	{
		pathExport = path;
	}
	string pathHash = Parametros().getParametro(CARPETA_DATOS);
	pathHash += NOMBRE_HASH_TITULOS;

	Hash * hash_titulos=new Hash(pathHash);

	hash_titulos->mostrar2(pathExport);
	delete hash_titulos;

	return 0;
}

int Servicios::verEstructuraPalabras(string path)
{
	string pathExport;
	if (path == "")
	{
		pathExport = Parametros().getParametro(CARPETA_SALIDA);
		pathExport += NOMBRE_HASH_PALABRAS;
	}
	else
	{
		pathExport = path;
	}
	string pathHash = Parametros().getParametro(CARPETA_DATOS);
	pathHash += NOMBRE_HASH_PALABRAS;

	Hash * hash_palabras=new Hash(pathHash);

	hash_palabras->mostrar2(pathExport);
	delete hash_palabras;

	return 0;
}



int Servicios::procesarLibro(int indice)
{
	int error = 0;
	SinIndice *listas = SinIndice().getInstancia();

	list<unsigned int> *lista;

	switch(indice)
	{
		case INDICE_AUTORES :lista = listas->getPendientesAutores();break;
		case INDICE_EDITORIALES:lista = listas->getPendientesEditoriales();break;
		case INDICE_TITULOS:lista = listas->getPendientesTitulos();break;
		case INDICE_PALABRAS:lista = listas->getPendientesPalabras();break;
	}

	for (list<unsigned int>::iterator it = lista->begin(); it != lista->end(); it++)
	{
		Libro *libro = 0;
		int error = recuperarLibro((*it), &libro);
		cout <<"Autor: "<< libro->getAutor() <<endl;
		cout <<"Editorial: "<< libro->getEditorial() <<endl;
		cout <<"Titulo: "<< libro->getTitulo() <<endl;
		cout <<"Id: "<< libro->getId() <<endl;
		cout <<"Cant.palabras: " << libro->getCantPalabras() << endl;
		//cout << "Presione ENTER para continuar...." << endl;
		//cin.get();

		switch(indice)
		{
			case INDICE_AUTORES :error = agregarIndiceAutores(libro);break;
			case INDICE_EDITORIALES:error = agregarIndiceEditoriales(libro);break;
			case INDICE_TITULOS:error = agregarIndiceTitulos(libro);break;
			case INDICE_PALABRAS:error = agregarIndicePalabras(libro);break;
		}

		delete libro;
	}

//limpia la lista de ids pendientes de procesar
	switch(indice)
	{
		case INDICE_AUTORES :error = listas->limpiarListaAutores();break;
		case INDICE_EDITORIALES:error = listas->limpiarListaEditoriales();break;
		case INDICE_TITULOS:error = listas->limpiarListaTitulos();break;
		case INDICE_PALABRAS:error = listas->limpiarListaPalabras();break;
	}

	return error;
}


int Servicios::agregarIndiceAutores(Libro *unLibro)
{
	//logica pertenece al arbol.
	agregarAlArbol(NOMBRE_BMAS_AUTORES, unLibro->getAutor(), unLibro->getId());

	return 0;
}

int Servicios::agregarIndiceEditoriales(Libro *unLibro)
{
	//logica pertenece al arbol.
	agregarAlArbol(NOMBRE_BMAS_EDITORIALES, unLibro->getEditorial(), unLibro->getId());

	return 0;
}

int Servicios::agregarIndiceTitulos(Libro *unLibro)
{
	agregarAlHash(NOMBRE_HASH_TITULOS,unLibro->getTitulo(),unLibro->getId());

	return 0;
}

int Servicios::agregarIndicePalabras(Libro *unLibro)
{
//cout<<unLibro->getPalabras()<<endl;cin.get();
	set<string> *palabras = unLibro->getListaPalabras();
	set<string>::iterator it = palabras->begin();

	for ( ; it != palabras->end(); it++)
	{
		//cout << "PALABRA A INDEXAR =" << "-"<<*it <<"-"<<endl;if ((*it)==" ") continue;
		agregarAlHash(NOMBRE_HASH_PALABRAS, *it ,unLibro->getId());
	}

	delete palabras;
	return 0;
}

void Servicios::agregarAlHash(string nombreHash, string clavePasada, unsigned int idNueva)
{
	string pathHash = Parametros().getParametro(CARPETA_DATOS);
	pathHash += nombreHash;
	Hash *hash  = new Hash(pathHash);

	Registro* registro= hash->buscar(clavePasada);
	unsigned int offset;

	//si ya existe la lista//
	if (registro){
		//cout << "lista ya existente" << endl;
		offset = registro->getAtributosEnteros()->front();
		ListasIds().agregarIdDeLibro(&offset,idNueva,false);
	}
	//no existe la lista de ids
	else{
		//cout << "lista nueva" << endl;
		ListasIds().agregarIdDeLibro(&offset, idNueva,true);
		registro = new Registro();
		registro->setString(clavePasada);
		registro->agregarAtribEntero(offset);
		hash->insertar(registro);
	}

	delete registro;
	delete hash;
}

void Servicios::agregarAlArbol(string nombreArbol, string clavePasada, unsigned int idNueva)
{
	string pathArbol = Parametros().getParametro(CARPETA_DATOS);
	pathArbol += nombreArbol;
	ArbolBMasAlfabetico *arbol  = new ArbolBMasAlfabetico(pathArbol, TAMANIO_BLOQUE_BMAS);

	resultadoOperacion resultado(OK);

	Registro* registro= arbol->buscarRegistro(clavePasada, &resultado);
	unsigned int offset;

	if (registro && resultado.getDescripcion() == "ENCONTRADO")
	{
		//lista ya existe
		offset = registro->getReferenciai(1);
		ListasIds().agregarIdDeLibro(&offset,idNueva,false);
	}
	else{
		//debo crear la lista nueva
		ListasIds().agregarIdDeLibro(&offset, idNueva,true);
		arbol->insertar(clavePasada,offset);
	}
	delete arbol;

}

void Servicios::sacarDelHash(string nombreHash, string clave, unsigned int idLibro)
{

	string pathHash = Parametros().getParametro(CARPETA_DATOS);
	pathHash += nombreHash;
	Hash *hash  = new Hash(pathHash);

	Registro* registro= hash->buscar(clave);
	unsigned int offset;

	if (registro){
		offset = registro->getAtributosEnteros()->front();
		bool listaVacia = ListasIds().sacarIdDelLibro(offset,idLibro);
		if (listaVacia == LISTA_VACIA){
			hash->borrar(clave);
		}
	}
	delete hash;
}


void Servicios::sacarDelArbol(string nombreArbol, string clave, unsigned int idLibro)
{

	string pathArbol = Parametros().getParametro(CARPETA_DATOS);
	pathArbol += nombreArbol;
	ArbolBMasAlfabetico *arbol  = new ArbolBMasAlfabetico(pathArbol, TAMANIO_BLOQUE_BMAS);
	resultadoOperacion resultado(OK) ;

	Registro* registro= arbol->buscarRegistro(clave, &resultado);
	unsigned int offset;


	if (registro && resultado.getDescripcion() == "ENCONTRADO")
	{
		offset = registro->getReferenciai(1);

		if (ListasIds().sacarIdDelLibro(offset,idLibro) == LISTA_VACIA){
			resultadoOperacion resultado = arbol->eliminar(clave);
		}
	}

	delete arbol;

}


int Servicios::recuperarLibro(unsigned int idLibro, Libro **libro)
{
	unsigned int offset = 0;

	string pathArbolPrimario = Parametros().getParametro(CARPETA_DATOS);
	pathArbolPrimario += NOMBRE_BMAS_PRIMARIO;

	ArbolBMasNumerico *arbolP = new ArbolBMasNumerico(pathArbolPrimario, TAMANIO_BLOQUE_BMAS_NUMERICO);

	resultadoOperacion resultado(OK) ;

	Registro* registro = arbolP->buscarRegistroNumerico(idLibro, &resultado);

	if (registro && resultado.getDescripcion() == "ENCONTRADO"){
		offset = registro->getReferencias()->front();
	}
	else{
		return ERROR_LIBRO_INEXISTENTE;
	}


	string rutaArcLibros = Parametros().getParametro(ARCHIVO_LIBROS);

	if (rutaArcLibros == "") return ERROR_RUTA_ARCHIVO_LIBROS;

	ArchivoLibros *archivo = new ArchivoLibros(rutaArcLibros);

	*libro = (archivo->recuperarLibro(offset));

	delete archivo;

	delete arbolP;

	return 0;
}

int Servicios::agregarVarios(string cuantos)
{
	int corte = atoi(cuantos.c_str());
	string rutaLibros = Parametros().getParametro(CARPETA_LIBROS);
	list<string> *archivos = Util().getArchivos(rutaLibros);
	int cont = 0;

	for (list<string>::iterator it = archivos->begin(); it != archivos->end(); it++)
	{
		cont++;
		string pathAbs = rutaLibros + *it;
		cout << "Agregando Texto: " << *it << "..." <<endl;
		Servicios().tomarTexto(pathAbs);

		if (cont >= corte) break;
	}

	return 0;

}

int Servicios::consultarEditorial(string editorialBuscada){
	string pathArbol = Parametros().getParametro(CARPETA_DATOS);
	pathArbol += NOMBRE_BMAS_EDITORIALES;
	ArbolBMasAlfabetico *arbol  = new ArbolBMasAlfabetico(pathArbol, TAMANIO_BLOQUE_BMAS);

	resultadoOperacion resultBusqueda(OK);
	Registro* regEncontrado= arbol->buscarRegistro(editorialBuscada,&resultBusqueda);
	if (resultBusqueda.getCodigo()==NO_ENCONTRADO) {
		cout<<"No se encontraron libros de la editorial buscada"<<endl;
	}
	else {
		unsigned int offset= regEncontrado->getReferenciai(1);
		list<unsigned int> listaIdsConsulta;
		ListasIds().obtenerListaIds(offset,&listaIdsConsulta);
		list<Libro> listadoLibros;
		list<unsigned int>::iterator itIds = listaIdsConsulta.begin();
		Libro* unLibro = NULL;

		while (	itIds != listaIdsConsulta.end()){
			recuperarLibro(*itIds, &unLibro);
			listadoLibros.push_back(*unLibro);
			itIds++;
		}

		cout << "Listado del libros encontrados: " << endl;
		unsigned int i = 1;

		for (list<Libro>::iterator it = listadoLibros.begin(); it != listadoLibros.end(); it++)	{
			cout<<"**************************  "<< i << "  **************************" << endl;
			cout << (*it).toString() << endl;
			i++;
		}
		cout << "Fin listado de libros: " << endl;

	}

	delete arbol;
	return 0;
}

int Servicios::consultarAutor(string autorBuscado){
	string pathArbol = Parametros().getParametro(CARPETA_DATOS);
	pathArbol += NOMBRE_BMAS_AUTORES;
	ArbolBMasAlfabetico *arbol  = new ArbolBMasAlfabetico(pathArbol, TAMANIO_BLOQUE_BMAS);

	resultadoOperacion resultBusqueda(OK);
	Registro* regEncontrado= arbol->buscarRegistro(autorBuscado,&resultBusqueda);
	if (resultBusqueda.getCodigo()==NO_ENCONTRADO) {
		cout<<"No se encontraron libros del autor buscado"<<endl;
	}
	else {
		unsigned int offset= regEncontrado->getReferenciai(1);
		list<unsigned int> listaIdsConsulta;
		ListasIds().obtenerListaIds(offset,&listaIdsConsulta);
		list<Libro> listadoLibros;
		list<unsigned int>::iterator itIds = listaIdsConsulta.begin();
		Libro* unLibro = NULL;

		while (	itIds != listaIdsConsulta.end()){
			recuperarLibro(*itIds, &unLibro);
			listadoLibros.push_back(*unLibro);
			itIds++;
		}

		cout << "Listado del libros encontrados: " << endl;
		unsigned int i = 1;

		for (list<Libro>::iterator it = listadoLibros.begin(); it != listadoLibros.end(); it++)	{
			cout<<"**************************  "<< i << "  **************************" << endl;
			cout << (*it).toString() << endl;
			i++;
		}
		cout << "Fin listado de libros: " << endl;

	}

	delete arbol;
	return 0;
}

int Servicios::consultarTitulo(string tituloBuscado){
	string pathHash= Parametros().getParametro(CARPETA_DATOS);
	pathHash+= NOMBRE_HASH_TITULOS;
	Hash hashTitulos(pathHash);
	Registro* registroTitulo= hashTitulos.buscar(tituloBuscado);

	if (registroTitulo!= NULL){
		unsigned int offset= registroTitulo->getAtributosEnteros()->front();
		list<unsigned int> listaIdsConsulta;
		ListasIds().obtenerListaIds(offset,&listaIdsConsulta);
		list<Libro> listadoLibros;
		list<unsigned int>::iterator itIds = listaIdsConsulta.begin();
		Libro* unLibro = NULL;

		while (	itIds != listaIdsConsulta.end()){
			recuperarLibro(*itIds, &unLibro);
			listadoLibros.push_back(*unLibro);
			itIds++;
		}

		cout << "Listado del libros encontrados: " << endl;
		unsigned int i = 1;

		for (list<Libro>::iterator it = listadoLibros.begin(); it != listadoLibros.end(); it++)	{
			cout<<"**************************  "<< i << "  **************************" << endl;
			cout << (*it).toString() << endl;
			i++;
		}
		cout << "Fin listado de libros: " << endl;

	}
	else{
		cout<<"No se encontraron libros con el titulo buscado"<<endl;
	}

	return 0;

}

int Servicios::consultarPalabras(string palabrasBuscadas){
	list<string>* listaTerminos = Parser().parsearFrase(palabrasBuscadas);
	SinIndice *listas = SinIndice().getInstancia();

	if (listas->getPendientesPalabras()->size() != 0){
		//mira que no tenes todas las palabras procesadas las busqueda puede arrojar datos que vos no busques
		cout << " Warning: Ud no tiene procesadas las palabras de todos los libros ingresados"
				", puede que la busqueda no de los resultados buscados. Procese las palabras"
				" con la opcion -p para asegurarse que la busqueda se realice correctamente. "
				"Esta seguro que desea continuar? (Y/N)" << endl;

		string respuesta = "0";
		cin >> respuesta;
		while (respuesta != "Y" && respuesta != "N" && respuesta != "n" && respuesta != "y"){
			cout << "Esta seguro que desea continuar? (Y/N)" << endl;
			cin >> respuesta;
		}
		if (respuesta == "N" || respuesta == "n") return 0;
	}

	archivoNormasInf archivoNormas;
	if (!archivoNormas.estaActualizado()){
		//mira que no tenes las normas actualizadas la busqueda puede ser mala
		cout << " Warning: Ud no tiene el archivo de normas actualizado"
		", puede que la busqueda no de los resultados buscados. Puede actualizarlo"
		" con la opcion -u para asegurarse que la busqueda se realice correctamente. "
		" Tenga en cuenta que es un proceso que puede tardar varios minutos. "
		"Esta seguro que desea continuar? (Y/N)" << endl;

		string respuesta = "0";
		cin >> respuesta;
		while (respuesta != "Y" && respuesta != "N" && respuesta != "n" && respuesta != "y"){
			cout << "Esta seguro que desea continuar? (Y/N)" << endl;
			cin >> respuesta;
		}
		if (respuesta == "N" || respuesta == "n") return 0;

	}


	list<string>::iterator it = listaTerminos->begin();
	//cout << "palabras en la busqueda: " << endl;
	//while (it != listaTerminos->end()){
	//	cout << *it << endl;
	//	it++;
	//}
	int error = 0;
	error = ProcesadorConsultas().procesar(*listaTerminos);

	delete listaTerminos;
	return error;
}

int Servicios::actualizarNormasInf(){

	archivoNormasInf archivoNormas;

	if (archivoNormas.estaActualizado()){
		cout<<"Las Normas Infinito ya se encuentran actualizadas."<<endl;
		return 0;
	}
	//cout << " voy a actualizar " << endl;
	cout << "Actualizando..." << endl;
	string pathArbolPrim = Parametros().getParametro(CARPETA_DATOS);
	pathArbolPrim+= "bmas_primario";
	ArbolBMasNumerico* arbolPrimario = new ArbolBMasNumerico(pathArbolPrim,
			TAMANIO_BLOQUE_BMAS_NUMERICO);

	resultadoOperacion result(OK);
	Registro* regActual = arbolPrimario->buscarRegistroNumerico(0, &result);
	unsigned int N = 0;

	while (regActual != NULL) {
		regActual = arbolPrimario->siguiente();
		N++;
	}
	//cout<<"N:"<<N<<endl;
	unsigned int i=0;

	while (i < N) {

		cout << "Actualizo norma del doc: " << i << endl;

		string pathArbolTerminos= Parametros().getParametro(CARPETA_DATOS);

		stringstream conversor;
		conversor<<i;
		string nroDoc;
		conversor>>nroDoc;
		pathArbolTerminos+= PREFIJO_ARBOL_TERMINOS+nroDoc;

		ArbolBMasAlfabetico* arbolDeTerminos = new ArbolBMasAlfabetico(
					pathArbolTerminos, 6144);

		Registro* regArbolTerm= arbolDeTerminos->buscarRegistro("0", &result);

		double acumuladoNormaInf= 0;

		while (regArbolTerm != NULL) {

			unsigned int offset= regArbolTerm->getReferenciai(1);

			list<unsigned int> listaPosiciones;
			ListasIds().obtenerListaIds(offset, &listaPosiciones);
			unsigned int aij = listaPosiciones.size();

			string rutahash= Parametros().getParametro(CARPETA_DATOS);
			rutahash+="hash_palabras";
			Hash hash(rutahash);
			Registro* regTermino = hash.buscar(regArbolTerm->getString());

			double ni= 1;

			if (regTermino!= NULL){

				list<unsigned int> listaIds;
				offset= regTermino->getAtributosEnteros()->front();
				ListasIds().obtenerListaIds(offset, &listaIds);
				ni= listaIds.size();

			}

			double pesoGlobal= log10((double)N/(double)ni);
			double peso= aij*pesoGlobal;
			acumuladoNormaInf+= pow(peso,2);

			delete regTermino;
			regArbolTerm= arbolDeTerminos->siguiente();
		}
		double normaInfinito= sqrt(acumuladoNormaInf);

		archivoNormas.actualizarNorma(normaInfinito, i);

		//cout<<"norma inf. actualizada:"<<normaInfinito<<endl;
		i++;
		delete regArbolTerm;
	}
	archivoNormas.estadoActualizacion('A');

	return 0;
}

int Servicios::verEstructArchTerminos(string path){
	string pathExport;
	if (path == "")
	{
		pathExport = Parametros().getParametro(CARPETA_SALIDA);
		pathExport += "Arch_Terminos";
	}
	else
	{
		pathExport = path+"_Terminos";
	}
	string pathHash = Parametros().getParametro(CARPETA_DATOS);
	pathHash += NOMBRE_HASH_PALABRAS;

	Hash hash_palabras(pathHash);

	hash_palabras.mostrar2(pathExport);

	return 0;
}

int Servicios::verEstructArchNormasInf(string path){
	string pathExport;
	if (path == "")
	{
		pathExport = Parametros().getParametro(CARPETA_SALIDA);
		pathExport += "Arch_NormaInfinito";
	}
	else
	{
		pathExport = path+"_NormaInfinito";
	}
	string pathPrimario= Parametros().getParametro(CARPETA_DATOS);
	pathPrimario+= NOMBRE_BMAS_PRIMARIO;
	ArbolBMasNumerico arbolPrimario(pathPrimario, TAMANIO_BLOQUE_BMAS_NUMERICO);
	resultadoOperacion result(OK);
	list<unsigned int> idLibros;
	Registro* regEncontrado= arbolPrimario.buscarRegistroNumerico(0,&result);
	while (regEncontrado!=NULL){
		idLibros.push_back(regEncontrado->getAtributosEnteros()->front());
		regEncontrado= arbolPrimario.siguiente();
	}
	archivoNormasInf normasInf;
	normasInf.imprimirNormasInf(pathExport,idLibros);
	return 0;
}

int Servicios::verEstructArchOcurrPos(string path, unsigned int nroArbol){
	string pathExport;
	if (path == "")
	{
		pathExport = Parametros().getParametro(CARPETA_SALIDA);
		pathExport += "DOC_"+Util().UIntToString(nroArbol)+"_OcurrenciaPosicional";
	}
	else
	{
		pathExport = path+"_DOC_"+Util().UIntToString(nroArbol)+"_OcurrenciaPosicional";
	}
	string pathTerminos= Parametros().getParametro(CARPETA_DATOS);
	pathTerminos+= PREFIJO_ARBOL_TERMINOS+Util().UIntToString(nroArbol);
	if(Util().existeArchivo(pathTerminos)==false){
		return ERROR_LIBRO_INEXISTENTE;
	}
	fstream archivoSalida(pathExport.c_str(),ios::out);
	archivoSalida<<"A continuacion se imprimen los terminos presentes en el libro, ";
			archivoSalida<< "seguidos de las posiciones en las q aparecen en el mismo"<<endl<<endl;
	ArbolBMasAlfabetico arbolterminos(pathTerminos,TAMANIO_BLOQUE_BMAS_PALABRAS);
	resultadoOperacion result(OK);
	Registro* regEncontrado= arbolterminos.buscarRegistro("0",&result);
	unsigned int offset;
	while(regEncontrado!=NULL){
		offset= regEncontrado->getReferenciai(1);
		list<unsigned int> listaPos;
		ListasIds().obtenerListaIds(offset,&listaPos);
		archivoSalida<<regEncontrado->getString()<<": ";
		list<unsigned int>::iterator it= listaPos.begin();
		//archivoSalida<<"tamanio "<<listaPos.size()<<" ";
		while(it!=listaPos.end()){
			archivoSalida<<*it<<", ";
			it++;
		}
		archivoSalida<<endl;
		regEncontrado= arbolterminos.siguiente();
	}
	return 0;
}

int Servicios::verEstructListasInvert(string path){
	string pathExport;
	if (path == "")
	{
		pathExport = Parametros().getParametro(CARPETA_SALIDA);
		pathExport += "Arch_Listas_ListasInvertidas";
	}
	else
	{
		pathExport = path+"_ListasInvertidas";
	}
	fstream archivoSalida(pathExport.c_str(),ios::out);
	archivoSalida<<"A continuacion se imprimen los terminos presentes en todos los doc";
	archivoSalida<<" seguidos de los libros en los cuales aparecen"<<endl<<endl;
	string pathHash= Parametros().getParametro(CARPETA_DATOS);
	pathHash+= NOMBRE_HASH_PALABRAS;
	Hash hashTerminos(pathHash);
	unsigned int offset;
	map<string,unsigned int>* listaTerminos=hashTerminos.recuperacionComprensiva();
	map<string, unsigned int>::iterator itTerminos= listaTerminos->begin();
	while(itTerminos!=listaTerminos->end()){
		offset= itTerminos->second;
		list<unsigned int> listaIds;
		ListasIds().obtenerListaIds(offset,&listaIds);
		archivoSalida<<itTerminos->first<<": ";
		list<unsigned int>::iterator it= listaIds.begin();
		while(it!=listaIds.end()){
			archivoSalida<<*it<<", ";
			it++;
		}
		archivoSalida<<endl;
		itTerminos++;
	}
	delete listaTerminos;
	return 0;
}
