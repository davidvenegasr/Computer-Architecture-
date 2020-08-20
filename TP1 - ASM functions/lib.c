#include "lib.h"

/** STRING **/

void hexPrint(char* a, FILE *pFile) {
    int i = 0;
    while (a[i] != 0) {
        fprintf(pFile, "%02hhx", a[i]);
        i++;
    }
    fprintf(pFile, "00");
}

/** Lista **/


//void listAddFirst(list_t* l, void* data)(21 Inst.)
//void listAddLast(list_t* l, void* data)(21 Inst.)
//void listAdd(list_t* l, void* data, funcCmp_t* fc)(54 Inst.)
//list_t* listClone(list_t* l, funcDup_t* fn)(31 Inst.)
//void listDelete(list_t* l, funcDelete_t* fd)(26 Inst.)
//void listPrint(list_t* l, FILE *pFile, funcPrint_t* fp)(40 Inst.)

void listRemove(list_t* l, void* data, funcCmp_t* fc, funcDelete_t* fd)
{
	if(l->first != NULL)
	{
		listElem_t* actual = l->first;   //apunta al PRIMERO
		while(actual!=NULL)
		{ 
			if(fc(data,actual->data)==0){
				if(l->first == actual){
					actual = actual->next;
					listRemoveFirst(l,fd);
				} else if (l->last == actual){
					actual = actual->next;
					listRemoveLast(l,fd);
				} else{
					listElem_t* previo = actual-> prev;
					listElem_t* proximo = actual-> next;
					previo->next = actual->next;
					proximo->prev = previo;
					listElem_t* paraborrar = actual;
					actual = actual->next;
					if(fd != 0)
						fd(paraborrar->data);
					free(paraborrar);
				}
			} else {
			actual = actual->next;
			}
		}
	}
}


void listRemoveFirst(list_t* l, funcDelete_t* fd){
	if(l->first != NULL){
	listElem_t* elem = l->first;
	listElem_t* next = elem->next;
	l->first = next;
	if(next==NULL){
		l->last = next;
	} else{
		next->prev = NULL;
	}
	if(fd != NULL){
		fd(elem->data);
	}
	free(elem);
	}
}

void listRemoveLast(list_t* l, funcDelete_t* fd){
	if(l->first != NULL){
	listElem_t* elem = l->last;
	listElem_t* prev = elem->prev;
	l->last = prev;
	if(prev==NULL){
		l->first = prev;
	} else {
		prev->next=NULL;
	}
	if(fd != NULL){
		fd(elem->data);
	}
	free(elem);
	}
}

list_t* listNew_c(){
	list_t* lista = (list_t*) malloc(sizeof(list_t));
	lista->first = NULL;
	lista->last = NULL;
	return lista;
}

void listAddFirst_c(list_t* lista, void* data){
	listElem_t* elem = (listElem_t*) malloc(sizeof(listElem_t));

	if(lista->first == NULL){ //Es vacio, agregamos un elemento
		lista->last = elem;
	} else {
		(lista->first)->prev = elem;
	}
	//listElem_t* first = lista->first;
	elem->data = data;
	elem->next = lista->first;
	elem->prev = NULL;
	lista->first = elem;
}

void listAddLast_c(list_t* lista, void* data){
	listElem_t* elem = (listElem_t*) malloc(sizeof(listElem_t));
	if(lista->first == NULL){ //Es vacio, agregamos un elemento
		lista->first = elem;
	} else {
		(lista->last)->next = elem;
	}
	elem->data = data;
	elem->next = NULL;
	elem->prev = lista->last;
	lista->last = elem;
}

void listAdd_c(list_t* l, void* data, funcCmp_t* fc){

	if(l->first != NULL){
		if(fc != NULL){
			listElem_t* actual = l->first;   //apunta al PRIMERO
			while(actual!=NULL){
				if(actual->next==NULL){
					listAddLast(l, data);
				} else if(fc(data,actual->data)==-1) {
					actual = actual->next;
				} else if(fc(data,actual->data)==0 || fc(data,actual->data)==1){
					listElem_t* previo = actual-> prev;
					listElem_t* proximo = actual-> next;
					//Creo el nodo a insertar
					listElem_t* nuevo = (listElem_t*) malloc(sizeof(listElem_t));
					// Relleno el nodo 
					nuevo->data = data;
					nuevo->next = proximo;
					nuevo->prev = previo;
					//Reviso si va a ser primero o ultimo
					if(l->first == proximo){ // Enlazo con el sucesor
						l->first = nuevo;
						previo->prev = nuevo;
					//} else if (proximo == NULL){  //Enlazo con el predecesor
					//	l->last = nuevo;
					//	previo->next = nuevo;
					} else { 
					// Enlazo con el sucesor y predecesor
						previo->next = nuevo;
						previo->prev = nuevo;
					}
					break;
				}
			}
		}
	} else {
		listAddFirst(l,data);
	}
}

// list_t* listClone_c(list_t* ListaVieja, funcDup_t* fn){
// 	list_t* listaNueva = listNew_c();
// 	listElem_t* prox = ListaVieja->first; 
// 	if(prox != NULL)
// 	{
// 		listAddLast_c(listaNueva,prox->data);	
// 		while(prox->next!=NULL){
// 			prox = prox->next;
// 			listAddLast_c(listaNueva,fn(prox->data));
// 		}
// 	}
// 	return listaNueva;
// }
list_t* listClone_c(list_t* ListaVieja, funcDup_t* fn){
	list_t* listaNueva = listNew_c();
	listElem_t* actual = ListaVieja->first;
	while(actual != NULL){
		void* new_data = fn(actual->data);
		listAddLast_c(listaNueva, new_data);
		actual = actual->next;
	}
	return listaNueva;
}


void listDelete_c(list_t* l, funcDelete_t* fd){
	if(l->first != NULL && fd != NULL)
	{
	listElem_t* prox = l->first; 

	prox = prox->next;
	listRemoveFirst(l,fd);	
	while(prox!=NULL)
	{
		prox = prox->next;
		listRemoveFirst(l,fd);
	}
	}
	free(l);
}
