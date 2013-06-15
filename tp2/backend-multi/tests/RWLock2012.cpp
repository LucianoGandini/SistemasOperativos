#include "RWLock2012.h"

RWLock::RWLock() {
    //pthread_rwlock_init(&(this->rwlock),NULL);
    this->lectores_corriendo = 0;
    this->escritores_corriendo = 0;
}

void RWLock :: rlock() {
	//pthread_rwlock_rdlock(&(this->rwlock));
	pthread_mutex_lock(&mutex_lectores_corriendo);
	pthread_mutex_lock(&mutex_escritores_corriendo);
	pthread_mutex_lock(&mutex_cola_peticiones);
	if (!this->cola_peticiones.empty())
	{
		//printf("cola no vacia\n");
		assert((this->escritores_corriendo == 0) || (this->lectores_corriendo == 0));
		Peticion *p = new(Peticion);
		(p->mutex_peticion) = new (pthread_mutex_t);
		pthread_mutex_init(p->mutex_peticion, NULL);
		p->tipo_peticion = TIPO_LECTURA;
		this->cola_peticiones.push(p);

		pthread_mutex_lock(p->mutex_peticion);			// hago esto para poder bloquearme despues
		//printf("no me bloquee1111\n");

		pthread_mutex_unlock(&mutex_cola_peticiones);
		pthread_mutex_unlock(&mutex_escritores_corriendo);
		pthread_mutex_unlock(&mutex_lectores_corriendo);

		pthread_mutex_lock(p->mutex_peticion);
		//printf("ahora runeo11111\n");
		
	}
	else
	{
		if ( this->lectores_corriendo == 0 && this->escritores_corriendo == 0 )
		{
			assert(this->cola_peticiones.empty());
			//printf("cola vacia pero lanzo uno\n");
			// no hay nadie corriendo, entonces paso yo
			this->lectores_corriendo++;
			assert(this->lectores_corriendo == 1);
			pthread_mutex_unlock(&mutex_cola_peticiones);
			pthread_mutex_unlock(&mutex_escritores_corriendo);
			pthread_mutex_unlock(&mutex_lectores_corriendo);

		}
		else if (this->lectores_corriendo == 0 && this->escritores_corriendo != 0)
		{
			assert(this->escritores_corriendo == 1);
			// hay un escritor asi que mejor me encolo y espero
			Peticion *p = new(Peticion);
			(p->mutex_peticion) = new (pthread_mutex_t);
			pthread_mutex_init(p->mutex_peticion, NULL);
			p->tipo_peticion = TIPO_LECTURA;
			this->cola_peticiones.push(p);

			pthread_mutex_lock(p->mutex_peticion);		// hago esto para poder bloquearme despues
			//printf("no me bloquee22222\n");

			pthread_mutex_unlock(&mutex_cola_peticiones);
			pthread_mutex_unlock(&mutex_escritores_corriendo);
			pthread_mutex_unlock(&mutex_lectores_corriendo);

			pthread_mutex_lock(p->mutex_peticion);
			//printf("ahora runeo22222\n");

		}
		else if (this->lectores_corriendo != 0 && this->escritores_corriendo == 0)
		{
			//printf("me encolo\n");
			assert(this->lectores_corriendo > 0);
			// hay lectores leyendo. pero para que nadie entre en inanicion me encolo y espero
			Peticion *p = new(Peticion);
			(p->mutex_peticion) = new (pthread_mutex_t);
			pthread_mutex_init(p->mutex_peticion, NULL);
			p->tipo_peticion = TIPO_LECTURA;
			this->cola_peticiones.push(p);
			pthread_mutex_lock(p->mutex_peticion);	// hago esto para poder bloquearme despues
			//printf("no me bloquee33\n");
			pthread_mutex_unlock(&mutex_cola_peticiones);
			pthread_mutex_unlock(&mutex_escritores_corriendo);
			pthread_mutex_unlock(&mutex_lectores_corriendo);

			pthread_mutex_lock(p->mutex_peticion);
			//printf("ahora runeo33\n");

		}
		else
		{
			// si llego aca es por que hay lectores y escritores corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}

}
void RWLock :: wlock() {
	//pthread_rwlock_wrlock(&(this->rwlock));
    
	pthread_mutex_lock(&mutex_lectores_corriendo);
	pthread_mutex_lock(&mutex_escritores_corriendo);
	pthread_mutex_lock(&mutex_cola_peticiones);

	if (!this->cola_peticiones.empty())
	{
		Peticion *p = new(Peticion);
		(p->mutex_peticion) = new (pthread_mutex_t);
		pthread_mutex_init(p->mutex_peticion, NULL);
		p->tipo_peticion = TIPO_ESCRITURA;
		this->cola_peticiones.push(p);

		pthread_mutex_lock(p->mutex_peticion); // hago esto para poder bloquearme despues
		
		pthread_mutex_unlock(&mutex_cola_peticiones);
		pthread_mutex_unlock(&mutex_escritores_corriendo);
		pthread_mutex_unlock(&mutex_lectores_corriendo);

		pthread_mutex_lock(p->mutex_peticion);
	}
	else
	{
		if ( lectores_corriendo == 0 && escritores_corriendo == 0 )
		{
			// no hay nadie corriendo, entonces paso yo
			this->escritores_corriendo++;
			assert(this->escritores_corriendo == 1);
			pthread_mutex_unlock(&mutex_cola_peticiones);
			pthread_mutex_unlock(&mutex_escritores_corriendo);
			pthread_mutex_unlock(&mutex_lectores_corriendo);

		}
		else if (lectores_corriendo == 0 && escritores_corriendo != 0)
		{
			
			// hay un escritor asi que mejor me encolo y espero
			Peticion *p = new(Peticion);
			(p->mutex_peticion) = new (pthread_mutex_t);
			pthread_mutex_init(p->mutex_peticion, NULL);
			p->tipo_peticion = TIPO_ESCRITURA;
			this->cola_peticiones.push(p);
			
			pthread_mutex_lock(p->mutex_peticion); // hago esto para poder bloquearme despues
			
			pthread_mutex_unlock(&mutex_cola_peticiones);
			pthread_mutex_unlock(&mutex_escritores_corriendo);
			pthread_mutex_unlock(&mutex_lectores_corriendo);

			pthread_mutex_lock(p->mutex_peticion);
			
		}
		else if (lectores_corriendo != 0 && escritores_corriendo == 0)
		{
			// hay lectores leyendo. entonces me encolo y espero
			Peticion *p = new(Peticion);
			(p->mutex_peticion) = new (pthread_mutex_t);
			pthread_mutex_init(p->mutex_peticion, NULL);
			p->tipo_peticion = TIPO_ESCRITURA;
			this->cola_peticiones.push(p);
			
			pthread_mutex_lock(p->mutex_peticion); // hago esto para poder bloquearme despues
			
			pthread_mutex_unlock(&mutex_cola_peticiones);
			pthread_mutex_unlock(&mutex_escritores_corriendo);
			pthread_mutex_unlock(&mutex_lectores_corriendo);

			pthread_mutex_lock(p->mutex_peticion);
			
		}
		else
		{
			// si llego aca es por que hay lectores y escritores corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}
	
}
void RWLock :: runlock() {
	
    //pthread_rwlock_unlock(&(this->rwlock));
	pthread_mutex_lock(&mutex_lectores_corriendo);
	pthread_mutex_lock(&mutex_escritores_corriendo);
	pthread_mutex_lock(&mutex_cola_peticiones);
	
	assert(this->lectores_corriendo > 0);
	
	if ( this->lectores_corriendo != 0 )	// comprobacion por si llegan muchos runlock 
		this->lectores_corriendo--;

	//printf("lectores_corriendo_runlock = %d \n", this->lectores_corriendo);
    //printf("this->lectores_corriendo = %d \n", this->lectores_corriendo);
	
	if (!this->cola_peticiones.empty())
	{

		if ( this->lectores_corriendo == 0 && this->escritores_corriendo == 0 )
		{
			// no hay nadie corriendo, entonces hago pasar a las siguientes peticiones de la cola
			int peticion_a_popear = this->cola_peticiones.front()->tipo_peticion;
			bool isColaVacia = this->cola_peticiones.empty();
			if (peticion_a_popear == TIPO_LECTURA)
			{
				while ((peticion_a_popear == TIPO_LECTURA) && !isColaVacia)
				{
					//printf("dentro del guile \n");
					if (pthread_mutex_unlock(this->cola_peticiones.front()->mutex_peticion))
						printf("algo salio mal \n");;
					this->lectores_corriendo++;
					this->cola_peticiones.pop();
					isColaVacia = this->cola_peticiones.empty();
					if (!isColaVacia)
					{
						peticion_a_popear = this->cola_peticiones.front()->tipo_peticion;
					}
					//printf("fin del guile \n");
				}
				assert(!(peticion_a_popear == TIPO_LECTURA) || isColaVacia);
			}
			else
			{
				//printf("rama false del if \n");
				assert(this->cola_peticiones.front()->mutex_peticion != NULL);
				if (pthread_mutex_unlock(this->cola_peticiones.front()->mutex_peticion))
					printf("algo salio mal\n");
				this->escritores_corriendo++;
				this->cola_peticiones.pop();
			}
			//printf("despues del if \n");
			assert(((this->escritores_corriendo == 0) || (this->lectores_corriendo == 0)) && (this->escritores_corriendo <= 1));
			
			pthread_mutex_unlock(&mutex_cola_peticiones);
			pthread_mutex_unlock(&mutex_escritores_corriendo);
			pthread_mutex_unlock(&mutex_lectores_corriendo);

		}
		else if (this->lectores_corriendo == 0 && this->escritores_corriendo != 0)
		{
			// si llego aca quiere decir que habia un escritor y un lector corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
		else if (this->lectores_corriendo != 0 && this->escritores_corriendo == 0)
		{
			// si llego aca quiere decir que habia un varios lectores corriendo. entonces hago pasar a los demas que estan al principio de la cola
			int peticion_a_popear = this->cola_peticiones.front()->tipo_peticion;
			bool isColaVacia = this->cola_peticiones.empty();
			while (peticion_a_popear == TIPO_LECTURA && !isColaVacia)
			{
				pthread_mutex_unlock(this->cola_peticiones.front()->mutex_peticion);
				this->lectores_corriendo++;
				this->cola_peticiones.pop();
				isColaVacia = this->cola_peticiones.empty();
				if (!isColaVacia)
				{
					peticion_a_popear = this->cola_peticiones.front()->tipo_peticion;
				}
			}
			
			assert((this->escritores_corriendo == 0) && (this->lectores_corriendo > 0)) ;
			
			pthread_mutex_unlock(&mutex_cola_peticiones);
			pthread_mutex_unlock(&mutex_escritores_corriendo);
			pthread_mutex_unlock(&mutex_lectores_corriendo);
			
		}
		else
		{
			// si llego aca es por que habia un lector y un escritor corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}
	else
	{
		// si la cola esta vacia entonces no hay nadie esperando. entonces me voy
		assert(this->escritores_corriendo == 0);
		//assert(this->lectores_corriendo == 0);
		pthread_mutex_unlock(&mutex_cola_peticiones);
		pthread_mutex_unlock(&mutex_escritores_corriendo);
		pthread_mutex_unlock(&mutex_lectores_corriendo);
	}
}
void RWLock :: wunlock() {
	//pthread_rwlock_unlock(&(this->rwlock));
	pthread_mutex_lock(&mutex_lectores_corriendo);
	pthread_mutex_lock(&mutex_escritores_corriendo);
	pthread_mutex_lock(&mutex_cola_peticiones);
	
	if ( this->escritores_corriendo != 0 )	// comprobacion por si llegan muchos wunlock 
		this->escritores_corriendo--;
	
	assert(this->escritores_corriendo == 0);
	
	if (!this->cola_peticiones.empty())
	{
		if ( lectores_corriendo == 0 && escritores_corriendo == 0 )
		{
			// no hay nadie corriendo, entonces hago pasar a las siguientes peticiones de la cola
			int peticion_a_popear = this->cola_peticiones.front()->tipo_peticion;
			bool isColaVacia = this->cola_peticiones.empty();
			if (peticion_a_popear == TIPO_LECTURA)
			{
				while (peticion_a_popear == TIPO_LECTURA && !isColaVacia)
				{
					pthread_mutex_unlock(this->cola_peticiones.front()->mutex_peticion);
					this->lectores_corriendo++;
					this->cola_peticiones.pop();
					isColaVacia = this->cola_peticiones.empty();
					if (!isColaVacia)
					{
						peticion_a_popear = this->cola_peticiones.front()->tipo_peticion;
					}
				}
			}
			else
			{
				assert(this->cola_peticiones.front()->mutex_peticion != NULL);
				if (pthread_mutex_unlock(this->cola_peticiones.front()->mutex_peticion))
					printf("algo salio mal\n");
				this->escritores_corriendo++;
				this->cola_peticiones.pop();
			}
			
			assert(((this->escritores_corriendo == 0) || (this->lectores_corriendo == 0)) && (this->escritores_corriendo <= 1));
			
			pthread_mutex_unlock(&mutex_cola_peticiones);
			pthread_mutex_unlock(&mutex_escritores_corriendo);
			pthread_mutex_unlock(&mutex_lectores_corriendo);
			
		}
		else if (lectores_corriendo == 0 && escritores_corriendo != 0)
		{
			// si llego aca es por que hay 2 o mas escritores corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
		else if (lectores_corriendo != 0 && escritores_corriendo == 0)
		{
			// si llego aca es por que hay 2 o mas lectores corriendo mientras estaba corriendo mi escritor. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
		else
		{
			// si llego aca es por que hay 2 o mas escritores corriendo. es imposible
			printf("no puede ser. se rompio todo\n");
			exit(-1);
		}
	}
	else
	{
		// si la cola esta vacia entonces no hay nadie esperando. entonces me voy
		assert(this->escritores_corriendo == 0);
		assert(this->lectores_corriendo == 0);
		pthread_mutex_unlock(&mutex_cola_peticiones);
		pthread_mutex_unlock(&mutex_escritores_corriendo);
		pthread_mutex_unlock(&mutex_lectores_corriendo);
	}
}
