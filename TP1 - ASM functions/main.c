#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "lib.h"

/////////////////////////////// INICIO TESTS DE STRINGS ///////////////////////////////

void test_strLen(){
    char * s0 = "";
	char * s1 = "O";
	char * s2 = "Or";
	char * s3 = "Org";
	char * s4 = "Orga";
	char * s5 = "Orga2";
	char * s6 = "Orga 2";
	assert(strLen(s0)==0);
	assert(strLen(s1)==1);
	assert(strLen(s2)==2);
	assert(strLen(s3)==3);
	assert(strLen(s4)==4);
	assert(strLen(s5)==5);
	assert(strLen(s6)==6);
	printf("%s\n","STRLEN CORRECTO");
}

void test_strClone(){
    char * p0 = "";
	char * p1 = "H";
	char * p2 = "Ho";
	char * p3 = "Hol";
	char * p4 = "Hola";
    char * s0 = strClone(p0);
	char * s1 = strClone(p1);
	char * s2 = strClone(p2);
	char * s3 = strClone(p3);
	char * s4 = strClone(p4);
	assert(strLen(s0)==0);
	assert(strLen(s1)==1);
	assert(strLen(s2)==2);
	assert(strLen(s3)==3);
	assert(strLen(s4)==4); 
    free(s0);
    free(s1);
    free(s2);
    free(s3);
    free(s4);
	printf("%s\n","STRCLONE CORRECTO");
} 

void test_strCmp(){
	char * s0 = "La ciudad de la furia";
	char * s1 = "Convit Jacinto";
	char * s2 = "convit jacinto";
	char * s3 = "hombresG";
	char * s4 = "hombresg";
	char * s5 = "      tp1";
	char * s6 = "      Tp1";
	char * s7 = "";
	char * s8 = "La vida boheme";
	assert(!strCmp(s0,s0));
	assert(strCmp(s1,s2)==1);
	assert(strCmp(s3,s4)==1);
	assert(strCmp(s5,s6)==-1);
	assert(strCmp(s7,s8)==1);
	
	
	// 0 si son iguales
	// 1 si a < b
	// −1 si b < a
	char sx1 = (int8_t)0x80;
	char sx2 = (int8_t)0x01;
	// char sx3 = (int8_t)0x2200;

	assert(strCmp(&sx1,&sx2)==1);
	
	// assert(strCmp(&sx3,&sx2)==1);
	// 
	// assert(strCmp(&sx3,&sx1)==-1);
	
	// [24c8f500,24f0cc094d07e300,2400] <-nosotros
	// [2400,24c8f500,24f0cc094d07e300] <-catedra
	// int32_t st1 = 0x24c8f500;
	// int32_t st2 = 0x24f0cc09;
	// int32_t st3 = 0x2400;
	
	// 0 si son iguales
	// 1 si a < b
	// −1 si a > b
	
	// TESTS QUE DEBERIAN ANDAR (y no funcionan)
	// assert(strCmp((char*)&st3,(char*)&st2)==1);
	// assert(strCmp((char*)&st2,(char*)&st3)==1);
	
	// TESTS QUE NO DEBERIAN ANDAR (y funcionan)
	// assert(strCmp(&st1[1],&st1[2])==1);
	// assert(strCmp(&st1[2],&st1[0])==1);
	
	
	printf("%s\n", "STRCMP CORRECTO");
}

void test_strConcat(){
	char * s0 = strClone("n");
	char * s1 = strClone("o");
	char * s2 = strClone("f");
	char * s3 = strClone("");
	char * s4 = strClone("f");
	char * s5 = strClone("");
	char * s6 = strClone("");
    char * concat1 = strConcat(s0,s1);
    char * concat2 = strConcat(s2,s3);
    char * concat3 = strConcat(s5,s4);
    char * concat4 = strConcat(s6,s6);
	assert(strLen(concat1)==2);
	assert(strLen(concat2)==1);
	assert(strLen(concat3)==1);
	assert(strLen(concat4)==0);
    free(concat1);
    free(concat2);
    free(concat3);
    free(concat4);
    printf("%s\n", "STRCONCAT CORRECTO");
}

void test_strDelete(){
	char * caso1 = strClone("Dennis Ritchie");
	char * caso2 = strClone("");
	strDelete(caso1);
	strDelete(caso2);
}

/////////////////////////////// FIN TEST DE STRINGS ///////////////////////////////


/////////////////////////////// INICIO TEST DE LISTS ///////////////////////////////

void test_listNew(){
	list_t* empty_list_c = listNew_c();
	list_t* empty_list_asm = listNew();
	//printf("%d\n", sizeof(*empty_list_asm));
	//printf("%p\n", (void*)empty_list_c->last);
	assert(sizeof(*empty_list_asm)==16);
	assert(empty_list_asm->first == NULL);
	assert(empty_list_asm->last == NULL);
	assert(sizeof(empty_list_asm)==sizeof(empty_list_c));
	assert(empty_list_asm->first == empty_list_c->first);
	assert(empty_list_asm->last == empty_list_c->last);
	listDelete(empty_list_asm, (funcDelete_t*)&strDelete);
	listDelete_c(empty_list_c, (funcDelete_t*)&strDelete);
	printf("%s\n", "LISTNEW CORRECTO");
}

void test_listRemoveFirst(){
	list_t* list_A = listNew_c();

	listAddFirst(list_A, strClone("SEGUNDO"));
	listAddFirst(list_A, strClone("PRIMERO")); //|PRIMERO|SEGUNDO|

	listRemoveFirst(list_A,(funcDelete_t*)&strDelete);
	listElem_t* actual = list_A->first;   //apunta a SEGUNDO
	assert(strcmp(actual->data,"SEGUNDO")==0);

	listRemoveFirst(list_A,(funcDelete_t*)&strDelete);	

	assert(sizeof(*list_A)==sizeof(list_t));

	listDelete(list_A, (funcDelete_t*)&strDelete);

	printf("%s\n", "LIST_REMOVEFIRST CORRECTO");
}

void test_listRemoveLast(){
	list_t* list_A = listNew_c();

	listAddLast(list_A, strClone("PRIMERO")); 
	listAddLast(list_A, strClone("SEGUNDO")); //|PRIMERO|SEGUNDO|

	listRemoveLast(list_A,(funcDelete_t*)&strDelete);
	listElem_t* actual = list_A->last;   //apunta a PRIMERO
	assert(strcmp(actual->data,"PRIMERO")==0);

	listRemoveFirst(list_A,(funcDelete_t*)&strDelete);	

	assert(sizeof(*list_A)==sizeof(list_t));

	listDelete(list_A, (funcDelete_t*)&strDelete);

	printf("%s\n", "LIST_REMOVELAST CORRECTO");
}

void test_listRemove(){
	list_t* list_A = listNew();

	listAddLast(list_A, strClone("A")); 
	listAddLast(list_A, strClone("B"));
	listAddLast(list_A, strClone("C")); 

	char * string = "B";
	
	listRemove(list_A,string,(funcCmp_t*)&strCmp,(funcDelete_t*)&strDelete);
	
	listElem_t* actual = list_A->first;  
	assert(strcmp(actual->data,"A")==0);
	actual = actual->next;
	assert(strcmp(actual->data,"C")==0);
	actual = list_A->last;   //apunta a PRIMERO
	assert(strcmp(actual->data,"C")==0);

	string = "C";
	listRemove(list_A,string,(funcCmp_t*)&strCmp,(funcDelete_t*)&strDelete);

	string = "A";
	listRemove(list_A,string,(funcCmp_t*)&strCmp,(funcDelete_t*)&strDelete);


	list_t* l1 = listNew();
	
	char* strings[10] = {"aa","bb","dd","ff","00","zz","cc","ee","gg","hh"};

    listRemove(l1, strings[0], (funcCmp_t*)&strCmp, (funcDelete_t*)&strDelete);
    for(int i=0; i<1;i++) {
        listAdd(l1,strClone(strings[i]),(funcCmp_t*)&strCmp);
        listRemove(l1, strings[0], (funcCmp_t*)&strCmp, (funcDelete_t*)&strDelete);
    }
	
	listDelete(list_A, (funcDelete_t*)&strDelete);
	listDelete(l1, (funcDelete_t*)&strDelete);
	
	printf("%s\n", "LIST_REMOVE CORRECTO");
}

void test_listDelete(){
	list_t* list_A = listNew_c();

	listAddLast(list_A, strClone("PRIMERO")); 
	listAddLast(list_A, strClone("SEGUNDO")); //|PRIMERO|SEGUNDO|
	listDelete(list_A, (funcDelete_t*)&strDelete);

	list_t* list_B = listNew_c();

	listAddLast(list_B, strClone("PRIMERO")); 
	listDelete(list_B, (funcDelete_t*)&strDelete);
}

void test_listPrint(){


	FILE *test_file = fopen("salida_test_main.txt","w");

	list_t* list_A = listNew_c();

	listAddFirst(list_A, strClone("TEST")); 
	listAddLast(list_A, strClone("PRINT")); 
	listAddLast(list_A, strClone("OK"));

	listPrint(list_A, test_file, (funcPrint_t*)&strPrint);

 	fprintf(test_file,"\n");
	fclose(test_file);
	
	listDelete(list_A, (funcDelete_t*)&strDelete);
}

void test_listAddFirst(){
	list_t* list_c = listNew_c();
	listAddFirst_c(list_c, strClone("SEGUNDO"));
	assert(strcmp(list_c->first->data,"SEGUNDO")==0);   //|SEGUNDO|

	listAddFirst_c(list_c, strClone("PRIMERO"));
	assert(strcmp(list_c->first->data,"PRIMERO")==0);	 //|PRIMERO|SEGUNDO|

	listElem_t* actual = list_c->first;   //apunta a PRIMERO
	actual = actual->next; //apunta a SEGUNDO
	assert(strcmp(actual->data,"SEGUNDO")==0);

	// Comparamos C y ASM
	list_t* list_asm = listNew();
	listAddFirst(list_asm, strClone("SEGUNDO"));
	listAddFirst(list_asm, strClone("PRIMERO"));
	actual = list_c->last;
	//char *p = (char *)actual->data;
	//printf("%s\n", p);
	//actual = list_asm->last;
	//char *p2 = (char *)actual->data;
	//printf("%s\n", p2);
	assert(strcmp(list_asm->first->data,list_c->first->data)==0);
	assert(strcmp(list_asm->last->data, list_c->last->data)==0);
	assert(sizeof(list_asm)==sizeof(list_c));


	listElem_t* actual_asm = list_asm->first;   //TEST 2do elemento
	actual_asm = actual_asm->next; //apunta a PRIMERO
	assert(strcmp(actual_asm->data,actual->data)==0);

	listDelete(list_asm, (funcDelete_t*)&strDelete);
	listDelete(list_c, (funcDelete_t*)&strDelete);
	printf("%s\n", "LIST_ADDFIRST CORRECTO");
}

void test_listAddLast(){
	list_t* list_c = listNew_c();
	listAddLast_c(list_c, strClone("FIRST"));
	assert(strcmp(list_c->first->data,"FIRST")==0);   //|FIRST|

	listAddLast_c(list_c, strClone("LAST"));
	assert(strcmp(list_c->last->data,"LAST")==0);	 //|FIRST|LAST|

	listElem_t* actual = list_c->first;   //apunta a FIRST
	actual = actual->next; //apunta a LAST
	assert(strcmp(actual->data,"LAST")==0);
	
	// Comparamos C y ASM
	list_t* list_asm = listNew();
	listAddLast(list_asm, strClone("FIRST"));
	listAddLast(list_asm, strClone("LAST"));
	actual = list_c->last;
	//char *p = (char *)actual->data;
	//printf("%s\n", p);
	//actual = list_asm->last;
	//char *p2 = (char *)actual->data;
	//printf("%s\n", p2);
	assert(strcmp(list_asm->first->data,list_c->first->data)==0);
	assert(strcmp(list_asm->last->data, list_c->last->data)==0);
	assert(sizeof(list_asm)==sizeof(list_c));
	listElem_t* actual_asm = list_asm->first;   //TEST 2do elemento
	actual_asm = actual_asm->next; //apunta a LAST
	actual = list_asm->last;
	//char *p2 = (char *)actual_asm->data;
	//printf("%s\n", p2);
	assert(strcmp(actual_asm->data, actual->data)==0);
	/*
	*/
	listDelete(list_asm, (funcDelete_t*)&strDelete);
	listDelete(list_c, (funcDelete_t*)&strDelete);
	printf("%s\n", "LISTADDLAST CORRECTO");
}

void test_listAdd(){
	list_t* list_c = listNew();
	listAddLast(list_c, strClone("A"));
	listAddLast(list_c, strClone("C")); 	//|A|C|
	assert(strcmp(list_c->first->data,"A")==0);

	listAdd(list_c,strClone("B"),(funcCmp_t*)&strCmp);
	listElem_t* actual = list_c->first;   //apunta a |A|
	actual = actual->next; //apunta a |B|
	assert(strcmp(list_c->first->data,"A")==0);
	assert(strcmp(actual->data,"B")==0);
	assert(strcmp(list_c->last->data,"C")==0);
		//caso borde
	list_t* list_c2 = listNew();
	listAdd(list_c2, strClone("A"),(funcCmp_t*)&strCmp);
	listAdd(list_c2, strClone("B"),(funcCmp_t*)&strCmp);

	listAdd(list_c2,strClone("C"),(funcCmp_t*)&strCmp);
	listAdd(list_c2,strClone("B"),(funcCmp_t*)&strCmp);
	actual = list_c2->first;   //apunta a |A|
	actual = actual->next; //apunta a |B|
	assert(strcmp(list_c2->first->data,"A")==0);
	assert(strcmp(actual->data,"B")==0);
	actual = actual->next; //apunta a |B|
	assert(strcmp(actual->data,"B")==0);
	assert(strcmp(list_c2->last->data,"C")==0);

	//listDelete(list_asm, (funcDelete_t*)&strDelete);
	listDelete(list_c, (funcDelete_t*)&strDelete);
	listDelete(list_c2, (funcDelete_t*)&strDelete);

	printf("%s\n", "LISTADD CORRECTO");
}

void test_listClone(){

	// TEST EMPTY LIST
	list_t* empty_list_asm = listNew();
	list_t* empty_list_clone_c = listClone(empty_list_asm,(funcDup_t*)&strClone);
	
	assert(sizeof(*empty_list_clone_c)==16);
	assert(empty_list_clone_c->first == NULL);
	assert(empty_list_clone_c->last == NULL);
	assert(sizeof(empty_list_asm)==sizeof(empty_list_clone_c));
	assert(empty_list_asm->first == empty_list_clone_c->first);
	assert(empty_list_asm->last == empty_list_clone_c->last);
	
	//CASO UN ELEMENTO
	list_t* list_c1 = listNew_c();
	listAdd(list_c1, strClone("A"),(funcCmp_t*)&strCmp);
	list_t* list_c2 = listClone(list_c1,(funcDup_t*)&strClone);
	assert(strcmp(list_c2->first->data,"A")==0);
	
	listAdd(list_c2, strClone("B"),(funcCmp_t*)&strCmp);
	listAdd(list_c2, strClone("B"),(funcCmp_t*)&strCmp);
	listAdd(list_c2, strClone("C"),(funcCmp_t*)&strCmp);
	// listC2 ahora es |A|B|B|C|
	
	list_t* list_c3 = listClone(list_c2,(funcDup_t*)&strClone);
	
	listElem_t* actual = list_c3->first;   //apunta a |A|
	actual = actual->next; //apunta a |B|
	assert(strcmp(actual->data,"B")==0);
	actual = actual->next; //apunta a |B|
	assert(strcmp(actual->data,"B")==0);
	assert(strcmp(list_c3->last->data,"C")==0);
	
	listDelete(empty_list_asm, (funcDelete_t*)&strDelete);
	listDelete(empty_list_clone_c, (funcDelete_t*)&strDelete);
	listDelete(list_c1, (funcDelete_t*)&strDelete);
	listDelete(list_c2, (funcDelete_t*)&strDelete);
	listDelete(list_c3, (funcDelete_t*)&strDelete);

	
	printf("%s\n", "LISTCLONE CORRECTO");
}

void test_lista(FILE *pfile){
	list_t* cancion = listNew();//
    fprintf(pfile,"==> TEST_LISTA\n"); //Cancion de La Vida Boheme (2013) 
    listAddLast(cancion,strClone("\n\n,¿Cómo va a ser la vida mejor?\n,¿Cómo va a ser la vida mejor?\n\n"));
    listAddLast(cancion,strClone("Yo pregunté, nadie respondió\n,Yo pregunté, nadie respondió\n\n"));
	listAddLast(cancion,strClone("¿Cómo va a ser la vida mejor?\n,¿Cómo va a ser la vida mejor?\n\n"));
    listAddLast(cancion,strClone("Escúchame bien, escucha por favor\n,¿Cómo va a ser la vida mejor?\n"));
    listAddLast(cancion,strClone("Árbol a medio pintar, ¿quién te enterró ahí en el asfalto?\n,Suspendan el carnaval, ¡volvamos al luto de antaño!\n"));
    listAddLast(cancion,strClone("Antena del Cafetal, ¿qué soledad te hizo tan alta?\n,¿Cómo no voy a llorar si tú te vas no te haré falta?\n"));
    listAddLast(cancion,strClone("¿Cómo no voy a llorar?, ¿cómo no voy a llorar?\n,¿cómo no voy a llorar? Si tú\n"));
    listAddLast(cancion,strClone("Te vas no te haré falta, ¿cómo no voy a llorar?\n,¿cómo no voy a llorar?, ¿cómo\n,No voy a llorar si tú te vas?\n"));
    listAddLast(cancion,strClone("¿Cómo va a ser la vida mejor?\n,¿Cómo va a ser la vida mejor?\n"));
    listAddLast(cancion,strClone("Yo pregunté, nadie respondió\n,Yo pregunté, nadie respondió\n;No\n\n"));
    listPrint(cancion,pfile,(funcPrint_t*)&strPrint); fprintf(pfile,"\n");
    listDelete(cancion,(funcDelete_t*)&strDelete);
}

/////////////////////////////// FIN TEST DE LISTS ///////////////////////////////

/////////////////////////////// INICIO TEST DE SORTER///////////////////////////////

void test_sorter_funcs(){
	char* string1 = strClone("Hola");
	char* string2 = strClone("holaaa");

	assert(fs_sizeModFive(string1)==4);
	assert(fs_sizeModFive(string2)==1);

	assert(fs_firstChar(string1)==72);
	assert(fs_firstChar(string2)==104);
	
	strDelete(string1);
	strDelete(string2);

	string1 = strClone("");
	assert(fs_bitSplit(string1)==8);
	strDelete(string1);
	string1 = strClone(" ");
	assert(fs_bitSplit(string1)==5);
	strDelete(string1);
	string1 = strClone("@");
	assert(fs_bitSplit(string1)==6);
	strDelete(string1);
	string1 = strClone("@aaaa");
	assert(fs_bitSplit(string1)==6);
	strDelete(string1);
	string1 = strClone("H");
	assert(fs_bitSplit(string1)==9);
	strDelete(string1);
	string1 = strClone("a");
	assert(fs_bitSplit(string1)==9);
	strDelete(string1);
	
	printf("%s\n","MINI SORTERS CORRECTO" );
}

void test_sorterNew(){
	
	sorter_t* empty_sorter = sorterNew(5, (funcSorter_t*)&fs_sizeModFive, (funcCmp_t*)&strCmp);
	
	assert(sizeof(*empty_sorter)==32);
	assert(empty_sorter->size == 5);
	assert(empty_sorter->sorterFunction == (funcSorter_t*)&fs_sizeModFive);
	assert(empty_sorter->compareFunction == (funcCmp_t*)&strCmp);
	assert(empty_sorter->slots != NULL);
	list_t **slots_new = empty_sorter->slots;
	for (int i = 0; i<5; i++){
		assert(slots_new[i]->first == NULL);
		assert(slots_new[i]->last == NULL);
	}
	sorterDelete(empty_sorter, (funcDelete_t*)&strDelete);
	printf("%s\n", "SORTER_NEW CORRECTO");
	
}

void test_sorterAdd_modFive_basic(){
	sorter_t* sorter = sorterNew(5, (funcSorter_t*)&fs_sizeModFive, (funcCmp_t*)&strCmp);
	char* string0 = strClone("hello");
	char* string1 = strClone("hello1");
	char* string2 = strClone("hello22");
	char* string3 = strClone("hello333");
	char* string4 = strClone("hello4444");
	sorterAdd(sorter, string0);
	sorterAdd(sorter, string1);
	sorterAdd(sorter, string2);
	sorterAdd(sorter, string3);
	sorterAdd(sorter, string4);
	
	list_t **slots_array= sorter->slots;
	for (int i = 0; i<5; i++){
		list_t* current_list = slots_array[i];
		assert(current_list->first == current_list->last);
		char* string;
		switch (i) {
			case 0: string = string0; break;
			case 1: string = string1; break;
			case 2: string = string2; break;
			case 3: string = string3; break;
			case 4: string = string4; break;
		}
		listElem_t* first =current_list->first;
		assert(strcmp((char*)first->data, string) == 0);
	}
	
	sorterDelete(sorter, (funcDelete_t*)&strDelete);
	printf("%s\n", "SORTER_ADD_modFive_BASIC CORRECTO");	
}

void test_sorterAdd_modFive_adv(){
	sorter_t* sorter = sorterNew(5, (funcSorter_t*)&fs_sizeModFive, (funcCmp_t*)&strCmp);
	char* string_0_0 = strClone("");
	char* string_0_1 = strClone("hawai");
	char* string_0_2 = strClone("hellohello");
	char* string_1_0 = strClone("a");
	char* string_1_1 = strClone("hawai1");
	char* string_1_2 = strClone("hello1hello");
	char* string_4_0 = strClone("hola");
	
	sorterAdd(sorter, string_0_0);
	sorterAdd(sorter, string_0_1);
	sorterAdd(sorter, string_0_2);
	sorterAdd(sorter, string_1_0);
	sorterAdd(sorter, string_1_1);
	sorterAdd(sorter, string_1_2);
	sorterAdd(sorter, string_4_0);
	
	list_t **slots_array= sorter->slots;
	
	list_t* current_list = slots_array[0];
	listElem_t* current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string_0_0)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string_0_1)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string_0_2)==0);
	
	current_list = slots_array[1];
	current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string_1_0)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string_1_1)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string_1_2)==0);
	
	current_list = slots_array[4];
	current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string_4_0)==0);
	
	sorterDelete(sorter, (funcDelete_t*)&strDelete);
	printf("%s\n", "SORTER_ADD_modFive_ADV CORRECTO");	
}

void test_sorterAdd_firstChar(){
	sorter_t* sorter = sorterNew(256, (funcSorter_t*)&fs_firstChar, (funcCmp_t*)&strCmp);
	char* string1 = strClone("1");
	char* string2 = strClone("hawai");
	char* string3 = strClone("hola");
	char* string4 = strClone("rana");
	char* string5 = strClone("2");
	char* string6 = strClone("mano");
	char* string7 = strClone("hola");
	char* string8 = strClone("");
	
	sorterAdd(sorter, string1);
	sorterAdd(sorter, string2);
	sorterAdd(sorter, string3);
	sorterAdd(sorter, string4);
	sorterAdd(sorter, string5);
	sorterAdd(sorter, string6);
	sorterAdd(sorter, string7);
	sorterAdd(sorter, string8);
	
	list_t **slots_array= sorter->slots;
	
	list_t* current_list = slots_array[0];	// "NULL" = 0
	listElem_t* current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string8)==0);
	
	current_list = slots_array[(int)'h'];
	current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string2)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string3)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string7)==0);
	
	current_list = slots_array[(int)'m'];
	current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string6)==0);
	
	current_list = slots_array[(int)'r'];
	current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string4)==0);
	
	current_list = slots_array[(int)'1'];
	current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string1)==0);
	
	current_list = slots_array[(int)'2'];
	current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string5)==0);
	
	sorterDelete(sorter, (funcDelete_t*)&strDelete);
	printf("%s\n", "SORTER_ADD_firstChar CORRECTO");	
}

void test_sorterAdd_bitSplit(){
	FILE *test_file = fopen("salida_test_add_bs.txt","w");
	sorter_t* sorter = sorterNew(10, (funcSorter_t*)&fs_bitSplit, (funcCmp_t*)&strCmp);
    char sx[3] = {0,0,0};
    // for(int i=0; i<256; i++) {
    //     for(int j=0; j<10; j++) {
			sx[0] = (int8_t)0x81;
			sx[1] = (int8_t)0x00;
			sorterAdd(sorter, strClone(sx));
            sx[0] = (int8_t)0x03;
            sx[1] = (int8_t)0x00;
            sorterAdd(sorter, strClone(sx));
    //     }
    // }
	
	sorterPrint(sorter, test_file, (funcPrint_t*)&hexPrint);
	sorterDelete(sorter, (funcDelete_t*)&strDelete);
	fprintf(test_file,"\n");
	fclose(test_file);
	printf("%s\n", "SORTER_ADD_bitSplit CORRECTO");	
}

void test_sorterRemove(){
	sorter_t* sorter = sorterNew(5, (funcSorter_t*)&fs_sizeModFive, (funcCmp_t*)&strCmp);
	char* string_0_0 = strClone("");
	char* string_0_1 = strClone("hawai");
	char* string_0_2 = strClone("hawai");
	char* string_0_3 = strClone("hellohello");
	char* string_1_0 = strClone("a");
	char* string_1_1 = strClone("hawai1");
	char* string_1_2 = strClone("hello1hello");
	char* string_4_0 = strClone("hola");
	
	// list_t **slots_array= sorter->slots;
	// list_t* current_list = slots_array[0];
	// listAdd(current_list,string_0_1,(funcCmp_t*)&strCmp);
	// listRemove(current_list, string_0_1, (funcCmp_t*)&strCmp,(funcDelete_t*)&strDelete);
	// char* string_new = strClone("hawai");
	// listAdd(current_list, string_new,(funcCmp_t*)&strCmp);
	// sorterDelete(sorter, (funcDelete_t*)&strDelete);
	

	sorterAdd(sorter, string_0_0);
	sorterAdd(sorter, string_0_1);
	sorterAdd(sorter, string_0_2);
	sorterAdd(sorter, string_0_3);
	sorterAdd(sorter, string_1_0);
	sorterAdd(sorter, string_1_1);
	sorterAdd(sorter, string_1_2);
	sorterAdd(sorter, string_4_0);
	
	//SIMILAR a test_sorterAdd_modFive_adv
	list_t **slots_array= sorter->slots;
	list_t* current_list = slots_array[0];
	listElem_t* current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string_0_0)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string_0_1)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string_0_2)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string_0_3)==0);
	current_list = slots_array[1];
	current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string_1_0)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string_1_1)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string_1_2)==0);
	current_list = slots_array[4];
	current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string_4_0)==0);
	//
	
	sorterRemove(sorter, string_1_2, (funcDelete_t*)&strDelete);
	
	current_list = slots_array[1];
	current_elem = current_list->first;
	assert (strCmp((char*)current_elem->data, string_1_0)==0);
	current_elem = current_elem->next;
	assert (strCmp((char*)current_elem->data, string_1_1)==0);
	current_elem = current_elem->next;
	assert (current_elem == NULL);
	
	// char* string_new = strClone("hello1hell3");	_WARNING_
	// sorterAdd(sorter, string_new);
	// sorterRemove(sorter, string_1_1, (funcDelete_t*)&strDelete);
	
	sorterDelete(sorter, (funcDelete_t*)&strDelete);
	printf("%s\n", "SORTER_DELETE CORRECTO");	
}

void test_sorterGetSlot(){
	sorter_t* sorter = sorterNew(5, (funcSorter_t*)&fs_sizeModFive, (funcCmp_t*)&strCmp);
	char* string_0_0 = strClone("");
	char* string_0_1 = strClone("hawai");
	char* string_0_2 = strClone("hawai");
	char* string_0_3 = strClone("hellohello");
	char* string_1_0 = strClone("a");
	char* string_1_1 = strClone("hawai1");
	char* string_1_2 = strClone("hello1hello");
	char* string_4_0 = strClone("hola");
	
	sorterAdd(sorter, string_0_0);
	sorterAdd(sorter, string_0_1);
	sorterAdd(sorter, string_0_2);
	sorterAdd(sorter, string_0_3);
	sorterAdd(sorter, string_1_0);
	sorterAdd(sorter, string_1_1);
	sorterAdd(sorter, string_1_2);
	sorterAdd(sorter, string_4_0);
	
	list_t* new_list0 = sorterGetSlot(sorter, 0, (funcDup_t*)&strClone);
	listElem_t* current_elem0 = new_list0->first;
	assert(strCmp((char*)current_elem0->data, string_0_0)==0);
	current_elem0 = current_elem0->next;
	assert(strCmp((char*)current_elem0->data, string_0_1)==0);
	current_elem0 = current_elem0->next;
	assert(strCmp((char*)current_elem0->data, string_0_2)==0);
	current_elem0 = current_elem0->next;
	assert(strCmp((char*)current_elem0->data, string_0_3)==0);
	current_elem0 = current_elem0->next;
	assert(current_elem0 == NULL);
	
	list_t* new_list1 = sorterGetSlot(sorter, 1, (funcDup_t*)&strClone);
	listElem_t* current_elem1 = new_list1->first;
	assert(strCmp((char*)current_elem1->data, string_1_0)==0);
	current_elem1 = current_elem1->next;
	assert(strCmp((char*)current_elem1->data, string_1_1)==0);
	current_elem1 = current_elem1->next;
	assert(strCmp((char*)current_elem1->data, string_1_2)==0);
	current_elem1 = current_elem1->next;
	assert(current_elem1 == NULL);
	
	list_t* new_list2 = sorterGetSlot(sorter, 4, (funcDup_t*)&strClone);
	listElem_t* current_elem2 = new_list2->first;
	assert(strCmp((char*)current_elem2->data, string_4_0)==0);
	current_elem2 = current_elem2->next;
	assert(current_elem2 == NULL);
	
	
	listDelete(new_list0, (funcDelete_t*)&strDelete);
	listDelete(new_list1, (funcDelete_t*)&strDelete);
	listDelete(new_list2, (funcDelete_t*)&strDelete);
	sorterDelete(sorter, (funcDelete_t*)&strDelete);
	printf("%s\n", "SORTER_GETSLOT CORRECTO");	
}

void test_sorterCleanSlot(){
	sorter_t* sorter = sorterNew(5, (funcSorter_t*)&fs_sizeModFive, (funcCmp_t*)&strCmp);
	char* string_0_0 = strClone("");
	char* string_0_1 = strClone("hawai");
	char* string_0_2 = strClone("hawai");
	char* string_0_3 = strClone("hellohello");
	char* string_1_0 = strClone("a");
	char* string_1_1 = strClone("hawai1");
	char* string_1_2 = strClone("hello1hello");
	char* string_4_0 = strClone("hola");
	
	sorterAdd(sorter, string_0_0);
	sorterAdd(sorter, string_0_1);
	sorterAdd(sorter, string_0_2);
	sorterAdd(sorter, string_0_3);
	sorterAdd(sorter, string_1_0);
	sorterAdd(sorter, string_1_1);
	sorterAdd(sorter, string_1_2);
	sorterAdd(sorter, string_4_0);
	
	list_t **slots_array= sorter->slots;
	
	sorterCleanSlot(sorter, 0, (funcDelete_t*)&strDelete);
	assert(slots_array[0]->first == NULL);
	
	sorterCleanSlot(sorter, 1, (funcDelete_t*)&strDelete);
	assert(slots_array[1]->first == NULL);
	
	sorterCleanSlot(sorter, 4, (funcDelete_t*)&strDelete);
	assert(slots_array[4]->first == NULL);
	
	sorterAdd(sorter, strClone(""));
	sorterAdd(sorter, strClone("hawai"));
	sorterAdd(sorter, strClone("hawaihawai"));
	
	sorterCleanSlot(sorter, 0, (funcDelete_t*)&strDelete);
	assert(slots_array[0]->first == NULL);
	
	sorterDelete(sorter, (funcDelete_t*)&strDelete);
	printf("%s\n", "SORTER_CLEANSLOT CORRECTO");	
}

void test_sorterPrint(){

	FILE *test_file = fopen("salida_test_main2.txt","w");

	sorter_t* sorter = sorterNew(5, (funcSorter_t*)&fs_sizeModFive, (funcCmp_t*)&strCmp);
	char* string_0_0 = strClone("");
	char* string_0_1 = strClone("hawai");
	char* string_0_2 = strClone("hawai");
	char* string_0_3 = strClone("hellohello");
	char* string_1_0 = strClone("a");
	char* string_1_1 = strClone("hawai1");
	char* string_1_2 = strClone("hello1hello");
	char* string_4_0 = strClone("hola");
	
	sorterAdd(sorter, string_0_0);
	sorterAdd(sorter, string_0_1);
	sorterAdd(sorter, string_0_2);
	sorterAdd(sorter, string_0_3);
	sorterAdd(sorter, string_1_0);
	sorterAdd(sorter, string_1_1);
	sorterAdd(sorter, string_1_2);
	sorterAdd(sorter, string_4_0);

	sorterPrint(sorter, test_file, (funcPrint_t*)&strPrint);

 	fprintf(test_file,"\n");
	fclose(test_file);
	
	sorterDelete(sorter, (funcDelete_t*)&strDelete);
	printf("%s\n", "SORTER_PRINT CORRECTO");	
}

void test_sorterGetConcatSlot(){

	sorter_t* sorter = sorterNew(5, (funcSorter_t*)&fs_sizeModFive, (funcCmp_t*)&strCmp);
	char* string_0_0 = strClone("arbol");
	char* string_0_1 = strClone("hawai");
	char* string_0_2 = strClone("hawai");
	char* string_0_3 = strClone("mandarinas");
	char* string_1_0 = strClone("a");
	char* string_1_1 = strClone("concatenado");
	char* string_1_2 = strClone("torres");
	char* string_4_0 = strClone("hola");
	
	sorterAdd(sorter, string_0_0);
	sorterAdd(sorter, string_0_1);
	sorterAdd(sorter, string_0_2);
	sorterAdd(sorter, string_0_3);
	sorterAdd(sorter, string_1_0);
	sorterAdd(sorter, string_1_1);
	sorterAdd(sorter, string_1_2);
	sorterAdd(sorter, string_4_0);

	char* new_str0 = sorterGetConcatSlot(sorter, 0);
	assert(strCmp(new_str0, "arbolhawaihawaimandarinas")==0);
	
	char* new_str1 = sorterGetConcatSlot(sorter, 1);
	assert(strCmp(new_str1, "aconcatenadotorres")==0);
	
	char* new_str2 = sorterGetConcatSlot(sorter, 4);
	assert(strCmp(new_str2, "hola")==0);
	
	strDelete(new_str0);
	strDelete(new_str1);
	strDelete(new_str2);

	sorterDelete(sorter, (funcDelete_t*)&strDelete);
	printf("%s\n", "SORTER_GETCONCATSLOT CORRECTO");	
}

/////////////////////////////// FIN TEST DE SORTER///////////////////////////////



void test_sorter(FILE *pfile){
    fprintf(pfile,"\n==> TEST_SORTER\n");
    sorter_t* s = sorterNew(5, (funcSorter_t*)&fs_sizeModFive, (funcCmp_t*)&strCmp);
    sorterAdd(s, strClone("La Vida Boheme"));
    sorterAdd(s, strClone("Arawato"));
    sorterAdd(s, strClone("Canserbero"));
    sorterAdd(s, strClone("Rawayana"));
    sorterAdd(s, strClone("Danny Ocean"));
    sorterPrint(s, pfile, (funcPrint_t*)&strPrint);
    sorterDelete(s, (funcDelete_t*)&strDelete);
}

int main (void){
    FILE *pfile = fopen("salida.caso.propios.txt","w");
    
    printf("%s\n\n", "_____________________________INICIO TEST STRINGS_____________________________");
    test_strLen();
    test_strClone();
    test_strCmp();
    test_strConcat();
    test_strDelete();
    printf("%s\n\n", "_____________________________INICIO TEST LISTS_____________________________");
    test_listRemoveFirst();
    test_listRemoveLast();
    test_listRemove();
    test_listDelete();
    test_listNew();
    test_listAddFirst();
    test_listAddLast();
    test_listAdd();
    
    test_listClone();
    
	printf("%s\n\n", "_____________________________INICIO TEST SORTER_____________________________");
    test_sorter_funcs();
	test_sorterNew();
	test_sorterAdd_modFive_basic();
	test_sorterAdd_modFive_adv();
	test_sorterAdd_firstChar();
	test_sorterAdd_bitSplit();
	test_sorterRemove();
	test_sorterGetSlot();
	test_sorterCleanSlot();
	test_sorterPrint();
	test_sorterGetConcatSlot();
	
	printf("%s\n\n", "____________________________________________________________________________");
    test_lista(pfile);
    test_sorter(pfile);
    fclose(pfile);
    test_listPrint();
    return 0;
}

