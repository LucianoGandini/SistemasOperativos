#include "srv.h"
/*
 *  Ejemplo de servidor que tiene el "sí fácil" para con su
 *  cliente y no se lleva bien con los demás servidores.
 *
 */

void servidor(int mi_cliente)
{
    MPI_Status status; int origen, tag;
    int hay_pedido_local = FALSE;
    int listo_para_salir = FALSE;
    //char buffer[100];
    int our_sequence_number;
    int highest_sequence_number_seen = 0;
    int responses;
    int n_ranks, mi_rank;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &mi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
    
    int withClients =  n_ranks;
   	int delayedResponses[n_ranks];

  	int id;
   	for (id = 0; id < n_ranks; id++) delayedResponses[id] = FALSE;
    
    while( ! listo_para_salir ) {
        
        MPI_Recv(NULL, 0, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        
        if (tag >= TAG_PIDO_RECURSO) {
        	int sequence_number = tag - TAG_PIDO_RECURSO;
			if(sequence_number > highest_sequence_number_seen) highest_sequence_number_seen = sequence_number;
        	if(!hay_pedido_local){
        		MPI_Send(NULL, 0, MPI_INT, origen, TAG_OTORGO_RECURSO, COMM_WORLD);
        	}
        	else {
        		if (sequence_number < our_sequence_number || (sequence_number == our_sequence_number && origen < mi_rank) ){
        			MPI_Send(NULL, 0, MPI_INT, origen, TAG_OTORGO_RECURSO, COMM_WORLD);
        		}
        		else {
        			assert(delayedResponses[origen] == FALSE);
        			delayedResponses[origen] = TRUE;
        		}
        	}
        }
        
        if (tag == TAG_OTORGO_RECURSO) {
        	assert(hay_pedido_local == TRUE);
        	responses ++;
        	if (responses == n_ranks - 2){// n-1 y saco uno mas por mi rango
        	    debug("Dándole permiso");
        		MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
        	}
        }
        
        if (tag == TAG_PEDIDO) {
            assert(origen == mi_cliente);
            debug("Mi cliente solicita acceso exclusivo");
            assert(hay_pedido_local == FALSE);
            hay_pedido_local = TRUE;
            our_sequence_number = highest_sequence_number_seen + 1;
            responses = 0;
            for (id = 0; id < n_ranks; id++){
            	if (id != mi_rank) MPI_Send(NULL, 0, MPI_INT, id, TAG_PIDO_RECURSO + mi_rank, COMM_WORLD);
            }
        }
        
        else if (tag == TAG_LIBERO) {
            assert(origen == mi_cliente);
            debug("Mi cliente libera su acceso exclusivo");
            assert(hay_pedido_local == TRUE);
            hay_pedido_local = FALSE;
            for (id = 0; id < n_ranks; id++){
				if ( delayedResponses[id] == TRUE ){
	            	MPI_Send(NULL, 0, MPI_INT, id, TAG_OTORGO_RECURSO, COMM_WORLD);
	            	delayedResponses[id] = FALSE;
	            }
            }
        }
        
        else if (tag == TAG_TERMINE) {
            assert(origen == mi_cliente);
            debug("Mi cliente avisa que terminó");
            listo_para_salir = TRUE;
            withClients--;
            for (id = 0; id < n_ranks; id++){
            	if (id != mi_rank) MPI_Send(NULL, 0, MPI_INT, id, TAG_SIN_CLIENTE, COMM_WORLD);
            }
        }
        
        else if (tag == TAG_SIN_CLIENTE) {
        	withClients--;
        }
    }
    
    while(withClients > 0) {
    	MPI_Recv(NULL, 0, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        
		if (tag >= TAG_PIDO_RECURSO) {
       		MPI_Send(NULL, 0, MPI_INT, origen, TAG_OTORGO_RECURSO, COMM_WORLD);
        }        
        else if (tag == TAG_SIN_CLIENTE) {
        	withClients--;
        }
    }
    
}

