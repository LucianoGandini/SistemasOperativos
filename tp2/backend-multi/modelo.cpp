#include <modelo.h>
#ifdef DEBUG
#include <cstdio>
#endif
#include <constantes.h>
#include <globales.h> 
#include <stdlib.h>

Modelo::Modelo(int njugadores, int tamtablero, int tamtotalbarcos){
	max_jugadores = njugadores;
	tamanio_tablero = tamtablero;
	tamanio_total_barcos = tamtotalbarcos;
	
	this->jugadores = new Jugador*[max_jugadores];
	this->eventos = new std::queue<evento_t *>[max_jugadores];
	this->lock_eventos = new RWLock[max_jugadores];
	this->tiros = new tiro_t*[max_jugadores];
	this->lock_jugadores_y_tiros = new RWLock[max_jugadores];
	for (int i = 0; i < max_jugadores; i++) {
		this->jugadores[i] = NULL;
		this->tiros[i] = NULL;
	}
	this->cantidad_jugadores = 0;
	this->jugando = false;
}
Modelo::~Modelo() {
	for (int i = 0; i < max_jugadores; i++) {
		if (this->jugadores[i] != NULL) {
			delete this->jugadores[i];
			delete this->tiros[i];
		}
	}
	delete[] this->jugadores;
	delete[] this->tiros;
	delete[] this->lock_eventos;
	delete[] this->lock_jugadores_y_tiros;
}

int Modelo::agregarJugador(std::string nombre) {
	if (DEBUGEAR) printf("Modelo::agregarJugador -> lock_jugando LOCK ANTES \n");
	lock_jugando.rlock();
	if (this->jugando){
		lock_jugando.runlock();
		return -ERROR_JUEGO_EN_PROGRESO;
	}
	lock_jugando.runlock();
	if (DEBUGEAR) printf("Modelo::agregarJugador -> lock_jugando UNLOCK  \n");
	lock_jugadores_y_tiros[0].rlock();
	int nuevoid = 0;
	for (nuevoid = 0; nuevoid + 1 < max_jugadores && this->jugadores[nuevoid] != NULL; nuevoid++){//TODO chequearlo
		lock_jugadores_y_tiros[nuevoid].runlock();
		lock_jugadores_y_tiros[nuevoid + 1].rlock();
	}
	
	if (this->jugadores[nuevoid] != NULL){
		lock_jugadores_y_tiros[nuevoid].runlock();
		return -ERROR_MAX_JUGADORES;
	}
	lock_jugadores_y_tiros[nuevoid].runlock();
	lock_jugadores_y_tiros[nuevoid].wlock();

	this->tiros[nuevoid] = new tiro_t();
	this->tiros[nuevoid]->t_id = JUGADOR_SINDEFINIR;
	this->tiros[nuevoid]->stamp.tv_sec = 0;
	this->tiros[nuevoid]->stamp.tv_usec = 0;
	this->tiros[nuevoid]->eta = 0;
	this->tiros[nuevoid]->estado = TIRO_LIBRE;
	
	this->jugadores[nuevoid] = new Jugador(nombre);

	lock_cantidad_jugadores.wlock();
	this->cantidad_jugadores++;
	lock_cantidad_jugadores.wunlock();
	lock_jugadores_y_tiros[nuevoid].wunlock();
	return nuevoid;
}


error Modelo::ubicar(int t_id, int * xs, int *  ys, int tamanio) {
	if (DEBUGEAR) printf("Modelo::ubicar -> lock_jugando LOCK ANTES \n");
	if (DEBUGEAR) lock_jugando.Estado();
	lock_jugando.rlock();
	if (DEBUGEAR) printf("Modelo::ubicar -> lock_jugando LOCK ADENTRO \n");
	if (this->jugando){
		lock_jugando.runlock();
		if (DEBUGEAR) printf("Modelo::ubicar -> lock_jugando UNLOCK \n");
		return -ERROR_JUEGO_EN_PROGRESO;
	}
	lock_jugando.runlock();
	if (DEBUGEAR) printf("Modelo::ubicar -> lock_jugando UNLOCK \n");
	
	if (DEBUGEAR) printf("Modelo::ubicar -> lock_jugadores_y_tiros LOCK ANTES \n");
	lock_jugadores_y_tiros[t_id].wlock();
	if (DEBUGEAR) printf("Modelo::ubicar -> lock_jugadores_y_tiros LOCK ADENTRO \n");
	if (this->jugadores[t_id] == NULL){
		return -ERROR_JUGADOR_INEXISTENTE;
	}
	error temp = this->jugadores[t_id]->ubicar(xs, ys, tamanio);
	lock_jugadores_y_tiros[t_id].wunlock();
	if (DEBUGEAR) printf("Modelo::ubicar -> lock_jugadores_y_tiros UNLOCK \n");
	
	return temp;
}

error Modelo::borrar_barcos(int t_id) {
	lock_jugando.rlock();
	if (this->jugando){
		lock_jugando.runlock();
		return -ERROR_JUEGO_EN_PROGRESO;
	}
	lock_jugando.runlock();

	lock_jugadores_y_tiros[t_id].wlock();
	if (this->jugadores[t_id] == NULL){
		lock_jugadores_y_tiros[t_id].wunlock();
		return -ERROR_JUGADOR_INEXISTENTE;
	}
	error temp = this->jugadores[t_id]->quitar_barcos();
	lock_jugadores_y_tiros[t_id].wunlock();
	return temp;
}

error Modelo::empezar() {
	lock_jugando.wlock();
	if (this->jugando){
		lock_jugando.wunlock();
		return -ERROR_JUEGO_EN_PROGRESO;
	}
	lock_cantidad_jugadores.rlock();
	bool completos = this->cantidad_jugadores == max_jugadores;
	lock_cantidad_jugadores.runlock();


	// 	for (nuevoid = 0; nuevoid + 1 < max_jugadores && this->jugadores[nuevoid] != NULL; nuevoid++){//TODO chequearlo
	// 	lock_jugadores_y_tiros[nuevoid].runlock();
	// 	lock_jugadores_y_tiros[nuevoid + 1].rlock();
	// }
	int ultimo;
	for (ultimo = 0; ultimo < max_jugadores && completos; ultimo++) {
		if (DEBUGEAR) printf("Modelo::empezar -> lock_jugadores_y_tiros LOCK ANTES 1 \n");
		lock_jugadores_y_tiros[ultimo].wlock();
		if (DEBUGEAR) printf("Modelo::empezar -> lock_jugadores_y_tiros LOCK DESPUES 1 \n");
		if (this->jugadores[ultimo] != NULL) {
			completos = completos && this->jugadores[ultimo]->listo();
		}
	}

	if (! completos){
		for (int i = 0; i < ultimo; i++){
			if (DEBUGEAR) printf("Modelo::empezar -> lock_jugadores_y_tiros UNLOCK ANTES 2 \n");
			lock_jugadores_y_tiros[i].wunlock();
			if (DEBUGEAR) printf("Modelo::empezar -> lock_jugadores_y_tiros UNLOCK DESPUES 2 \n");
		}
		lock_jugando.wunlock();
		return -ERROR_JUGADOR_NO_LISTO;
	}

	evento_t * nuevoevento;
	for (int i = 0; i < max_jugadores && completos; i++) {
		if (this->jugadores[i] != NULL) {
			nuevoevento = (evento_t*)malloc(sizeof(evento_t));
			nuevoevento->t_id = i;
			nuevoevento->s_id = 0;
			nuevoevento->x = 0;
			nuevoevento->y = 0;
			nuevoevento->status = EVENTO_START;
			
			if (DEBUGEAR) printf("Modelo::empezar -> lock_eventos LOCK ANTES \n");
			lock_eventos[i].wlock();
			this->eventos[i].push(nuevoevento);
			lock_eventos[i].wunlock();
			if (DEBUGEAR) printf("Modelo::empezar -> lock_eventos LOCK DESPUES \n");
		}
		lock_jugadores_y_tiros[i].wunlock();
	}
	
	this->jugando = true;
	lock_jugando.wunlock();
	return ERROR_NO_ERROR;
	
}
error Modelo::reiniciar() {
	lock_jugando.wlock();
	for (int i = 0; i < max_jugadores; i++) {
		lock_jugadores_y_tiros[i].wlock();
		if (this->jugadores[i] != NULL) {
			this->jugadores[i]->reiniciar();
			this->tiros[i]->t_id = JUGADOR_SINDEFINIR;
			this->tiros[i]->stamp.tv_sec = 0;
			this->tiros[i]->stamp.tv_usec = 0;
			this->tiros[i]->eta = 0;
			this->tiros[i]->estado = TIRO_LIBRE;
		}
		lock_jugadores_y_tiros[i].wunlock();
	}
	this->jugando = false;
	lock_jugando.wunlock();
	return ERROR_NO_ERROR;
	
}

error Modelo::quitarJugador(int s_id) {
	lock_jugando.rlock();
	if (this->jugando){
		lock_jugando.runlock();
		return -ERROR_JUEGO_EN_PROGRESO;
	}
	lock_jugando.runlock();

	lock_jugadores_y_tiros[s_id].wlock();
	if (this->jugadores[s_id] == NULL){
		lock_jugadores_y_tiros[s_id].wunlock();
		return -ERROR_JUGADOR_INEXISTENTE;
	}
	delete this->jugadores[s_id];
	delete this->tiros[s_id];
	
	this->jugadores[s_id] = NULL;
	this->tiros[s_id] = NULL;
	
	lock_jugadores_y_tiros[s_id].wunlock();
	return ERROR_NO_ERROR;
}

void Modelo::wlockTwoJugadoresYTiros(int s_id, int t_id){
	if(s_id < t_id){
		lock_jugadores_y_tiros[s_id].wlock();
		lock_jugadores_y_tiros[t_id].wlock();
	} else if (s_id == t_id){
		lock_jugadores_y_tiros[s_id].wlock();
	} else {
		lock_jugadores_y_tiros[t_id].wlock();
		lock_jugadores_y_tiros[s_id].wlock();
	}
}

void Modelo::wunlockTwoJugadoresYTiros(int s_id, int t_id){
	if(s_id < t_id){
		lock_jugadores_y_tiros[s_id].wunlock();
		lock_jugadores_y_tiros[t_id].wunlock();
	} else if (s_id == t_id){
		lock_jugadores_y_tiros[s_id].wunlock();
	} else {
		lock_jugadores_y_tiros[t_id].wunlock();
		lock_jugadores_y_tiros[s_id].wunlock();
	}
}

int Modelo::apuntar(int s_id, int t_id, int x, int y, int *eta) {//posible deadlock evadido con wunlockTwo y wlockTwo, ver como hacer para que puedan atacarse mutuamente
	//jugadores
	lock_jugando.rlock();
	if (!this->jugando){
		lock_jugando.runlock();
		return -ERROR_JUEGO_NO_COMENZADO;
	}
	lock_jugando.runlock();

	wlockTwoJugadoresYTiros(s_id,t_id);

	if (this->jugadores[s_id] == NULL){
		wunlockTwoJugadoresYTiros(s_id, t_id);
		return -ERROR_JUGADOR_INEXISTENTE;
	}

	if (this->jugadores[t_id] == NULL){
		wunlockTwoJugadoresYTiros(s_id, t_id);
		return -ERROR_JUGADOR_INEXISTENTE;
	}

	if (! this->jugadores[s_id]->esta_vivo()){
		wunlockTwoJugadoresYTiros(s_id, t_id);
		return -ERROR_JUGADOR_HUNDIDO;
	}

	int retorno = RESULTADO_APUNTADO_DENEGADO;
	if (this->es_posible_apuntar(this->tiros[s_id])) {
		retorno = this->jugadores[t_id]->apuntar(s_id, x, y);//modifica t_id
		if (retorno == RESULTADO_APUNTADO_ACEPTADO) {
			this->tiros[s_id]->t_id = t_id;
			gettimeofday(&this->tiros[s_id]->stamp, NULL);
			this->tiros[s_id]->eta = rand() % (MAX_ETA) + MIN_ETA;
			//dprintf("ETA %d ms", this->tiros[s_id]->eta);
			*eta = this->tiros[s_id]->eta;
			this->tiros[s_id]->estado = TIRO_APUNTADO;
			this->tiros[s_id]->x = x;
			this->tiros[s_id]->y = y;
			evento_t * nuevoevento = (evento_t*)malloc(sizeof(evento_t));
			nuevoevento->t_id = t_id;
			nuevoevento->s_id = s_id;
			nuevoevento->x = x;
			nuevoevento->y = y;
			nuevoevento->status = CASILLA_EVENTO_INCOMING;
			lock_eventos[t_id].wlock();
			this->eventos[t_id].push(nuevoevento);
			lock_eventos[t_id].wunlock();
		}
	}

	wunlockTwoJugadoresYTiros(s_id, t_id);
	return retorno;
	
}

int Modelo::dame_eta(int s_id) {
	lock_jugando.rlock();
	if (!this->jugando) {
		lock_jugando.runlock();
		return -ERROR_JUEGO_NO_COMENZADO;
	}
	lock_jugando.runlock();
	
	lock_jugadores_y_tiros[s_id].rlock(); 
	if (this->jugadores[s_id] == NULL){
		lock_jugadores_y_tiros[s_id].runlock();
		return -ERROR_JUGADOR_INEXISTENTE;
	}
	tiro_t * tiro = this->tiros[s_id];
	if (tiro->estado != TIRO_APUNTADO) return -ERROR_ESTADO_INCORRECTO;
	lock_jugadores_y_tiros[s_id].runlock();
	return tiro->eta;
}

int Modelo::tocar(int s_id, int t_id) {
	lock_jugando.rlock();
	if (!this->jugando){
		lock_jugando.runlock();
		return -ERROR_JUEGO_NO_COMENZADO;
	}
	lock_jugando.runlock();

	wlockTwoJugadoresYTiros(s_id,t_id);

	if (this->jugadores[s_id] == NULL){
		wunlockTwoJugadoresYTiros(s_id, t_id);
		return -ERROR_JUGADOR_INEXISTENTE;
	}

	if (this->jugadores[t_id] == NULL){
		wunlockTwoJugadoresYTiros(s_id, t_id);
		return -ERROR_JUGADOR_INEXISTENTE;
	}

	
	int retorno = -ERROR_ETA_NO_TRANSCURRIDO;
	if (this->es_posible_tocar(this->tiros[s_id])) {
		int x = this->tiros[s_id]->x;
		int y = this->tiros[s_id]->y;
		retorno = this->jugadores[t_id]->tocar(s_id, x, y);
		if (retorno == EMBARCACION_RESULTADO_TOCADO ||
			retorno == EMBARCACION_RESULTADO_HUNDIDO ||
			retorno == EMBARCACION_RESULTADO_HUNDIDO_M ||
			retorno == EMBARCACION_RESULTADO_AGUA ||
			retorno == EMBARCACION_RESULTADO_AGUA_H
			) {
			
			this->tiros[s_id]->estado = TIRO_LIBRE;
			//Evento para el tirado
			evento_t * nuevoevento = (evento_t*)malloc(sizeof(evento_t));
			nuevoevento->t_id = t_id;
			nuevoevento->s_id = s_id;
			nuevoevento->x = x;
			nuevoevento->y = y;
			nuevoevento->status = retorno;
			lock_eventos[t_id].wlock();
			this->eventos[t_id].push(nuevoevento);
			lock_eventos[t_id].wunlock();
			//Evento para el tirador
			nuevoevento = (evento_t*)malloc(sizeof(evento_t));
			nuevoevento->t_id = t_id;
			nuevoevento->s_id = s_id;
			nuevoevento->x = x;
			nuevoevento->y = y;
			nuevoevento->status = retorno;
			lock_eventos[s_id].wlock();
			this->eventos[s_id].push(nuevoevento);
			lock_eventos[s_id].wunlock();
		}
		if (retorno == EMBARCACION_RESULTADO_HUNDIDO) {
			this->jugadores[s_id]->agregar_puntaje(PUNTAJE_HUNDIDO);
		} else if (retorno == EMBARCACION_RESULTADO_HUNDIDO_M) {
			this->jugadores[s_id]->agregar_puntaje(PUNTAJE_HUNDIDO+PUNTAJE_MISMO_JUGADOR);
		} else if (retorno == EMBARCACION_RESULTADO_TOCADO) {
			this->jugadores[s_id]->agregar_puntaje(PUNTAJE_TOCADO);
		} else if (retorno == EMBARCACION_RESULTADO_AGUA_H) {
			this->jugadores[s_id]->agregar_puntaje(PUNTAJE_MAGALLANES);
		}
	}
	wunlockTwoJugadoresYTiros(s_id, t_id);
	return retorno;
}

#ifdef DEBUG
void Modelo::print() {
	printf("MODELO -- NJugadores %d, Jugando %d\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n", this->cantidad_jugadores, this->jugando);
	for (int i = 0; i < max_jugadores; i++) {
		this->lock_jugadores_y_tiros[i].rlock();
		if (this->jugadores[i] != NULL) {
			this->jugadores[i]->print();
			printf( "Tiro: id %d, stamp (%lu, %lu), eta %d, estado %d\n", this->tiros[i]->t_id, this->tiros[i]->stamp.tv_sec, (long unsigned int)this->tiros[i]->stamp.tv_usec, this->tiros[i]->eta, this->tiros[i]->estado);
		}
		printf("\n");
		this->lock_jugadores_y_tiros[i].runlock();
	}
	
	printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
}

#endif
bool  Modelo::es_posible_tocar(tiro_t * tiro) {
	bool pasoeleta = tiro->estado == TIRO_APUNTADO;
	if (pasoeleta) {
		struct timeval ahora;
		struct timeval * antes = &tiro->stamp;
		gettimeofday(&ahora, NULL);
		int msec = (ahora.tv_sec - antes->tv_sec) * 1000;
		msec += (ahora.tv_usec - antes->tv_usec) / 1000;
		
		if (msec < tiro->eta) {
			pasoeleta = false;
		}
	}
	
	return pasoeleta;
}
	
bool  Modelo::es_posible_apuntar(tiro_t * tiro) {
	return tiro->estado == TIRO_LIBRE;
}

int Modelo::hayEventos(int s_id) {
	lock_eventos[s_id].rlock();
	int size = this->eventos[s_id].size();
	lock_eventos[s_id].runlock(); 
	return size;
}

evento_t * Modelo::dameEvento(int s_id) {
	evento_t *retorno = NULL;
	lock_eventos[s_id].wlock();
	if (! this->eventos[s_id].empty() ) {
		retorno = this->eventos[s_id].front();
		this->eventos[s_id].pop();
	}
	lock_eventos[s_id].wunlock();
    return retorno;

}

evento_t * Modelo::actualizar_jugador(int s_id) {
    evento_t *retorno = NULL;
    
    lock_jugadores_y_tiros[s_id].rlock();
	tiro_t * t = this->tiros[s_id];
	lock_jugadores_y_tiros[s_id].runlock();
	
	int tocado = this->tocar(s_id, t->t_id);
	
	lock_eventos[s_id].wlock();
    if (! this->eventos[s_id].empty() ) {
        retorno = this->eventos[s_id].front();
		this->eventos[s_id].pop();
    } else {
		retorno = (evento_t*)malloc(sizeof(evento_t));
		retorno->s_id = s_id;
		retorno->t_id = -1;
		retorno->status = -tocado;
		retorno->x = 0;
		retorno->y = 0;
	}
	lock_eventos[s_id].wunlock();
    return retorno;
}
