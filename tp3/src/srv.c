#include "srv.h"


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
    
    n_ranks = n_ranks/2;
    mi_rank = mi_rank/2;
    int withClients =  n_ranks;
   	int delayedResponses[n_ranks];
	MPI_Request requests[n_ranks];

  	int id;
   	for (id = 0; id < n_ranks; id++) delayedResponses[id] = FALSE;
    
    while( ! listo_para_salir ) {
        
        MPI_Recv(NULL, 0, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        
        if (tag >= TAG_PIDO_RECURSO) {
        	int sequence_number = tag - TAG_PIDO_RECURSO;
			if(sequence_number > highest_sequence_number_seen) highest_sequence_number_seen = sequence_number;
       	    debug("Me pidieron el recurso");
        	if(!hay_pedido_local){
        		debug("Lo otorgo por que no lo necesito");
        		MPI_Send(NULL, 0, MPI_INT, origen, TAG_OTORGO_RECURSO, COMM_WORLD);
        	}
        	else {
        		if (sequence_number < our_sequence_number || (sequence_number == our_sequence_number && origen < mi_rank) ){
	        		debug("Lo otorgo por que tiene mayor prioridad");
        			MPI_Send(NULL, 0, MPI_INT, origen, TAG_OTORGO_RECURSO, COMM_WORLD);
        		}
        		else {
	        		debug("Tiene menor prioridad, no lo otorgo y guardo el pedido");
        			assert(delayedResponses[origen/2] == FALSE);
        			delayedResponses[origen/2] = TRUE;
        		}
        	}
        }
        
        if (tag == TAG_OTORGO_RECURSO) {
        	assert(hay_pedido_local == TRUE);
        	responses ++;
        	if (responses == n_ranks - 1){// n-1 y saco uno mas por mi rango
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
            if (n_ranks == 1){
            	debug("Dándole permiso");
        		MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }
            else {
                for (id = 0; id < n_ranks; id++){
	        		debug("Pido acceso a todos");
            		if (id != mi_rank){
            			MPI_Isend(NULL, 0, MPI_INT, id*2, TAG_PIDO_RECURSO + our_sequence_number, COMM_WORLD, & requests[id] );
            		}
            	}
            }
        }
        
        else if (tag == TAG_LIBERO) {
            assert(origen == mi_cliente);
            debug("Mi cliente libera su acceso exclusivo");
            assert(hay_pedido_local == TRUE);
            hay_pedido_local = FALSE;
            for (id = 0; id < n_ranks; id++){
				if ( delayedResponses[id] == TRUE ){
	        		debug("Respondo pedidos guardados");
	            	MPI_Send(NULL, 0, MPI_INT, id*2, TAG_OTORGO_RECURSO, COMM_WORLD);
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
            	if (id != mi_rank) {
            		MPI_Send(NULL, 0, MPI_INT, id*2, TAG_SIN_CLIENTE, COMM_WORLD);
            	}
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
       		debug("Otorgo por que ya no necesito el recurso, no tengo cliente");
       		MPI_Send(NULL, 0, MPI_INT, origen, TAG_OTORGO_RECURSO, COMM_WORLD);
        }        
        else if (tag == TAG_SIN_CLIENTE) {
        	withClients--;
        }
    }
    
}

