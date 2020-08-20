# TP1 ORGA2

### Funciones Implementadas
- STRINGS
- [x] char* strClone(char* a)
- [x] uint32_t strLen(char* a)
- [x] int32_t strCmp(char* a, char* b)
- [x] char* strConcat(char* a, char* b)
- [x] void strDelete(char* a)
- [x] void strPrint(char* a, FILE *pFile)
- LISTAS
- [x] list_t* listNew()
- [x] void listAddFirst(list_t* l, void* data)
- [x] void listAddLast(list_t* l, void* data)
- [x] void listAdd(list_t* l, void* data, funcCmp_t* fc)
- [x] list_t* listClone(list_t* l, funcDup_t* fn)
- [x] void listDelete(list_t* l, funcDelete_t* fd)
- [x] void listPrint(list_t* l, FILE *pFile, funcPrint_t* fp)
- [x] void listRemoveFirst(list_t* l, funcDelete_t* fd)
- [x] void listRemoveLast(list_t* l, funcDelete_t* fd) 
- [x] void listRemove(list_t* l, void* data, funcCmp_t* fc, funcDelete_t* fd) *Falta testing*
- SORTER 
- [x] sorter_t* sorterNew(uint16_t slots, funcSorter_t* fs, funcCmp_t* fc)
- [x] void sorterAdd(sorter_t* sorter, void* data)
- [x] void sorterRemove(sorter_t* sorter, void* data, funcDelete_t* fd)
- [x] list_t* sorterGetSlot(sorter_t* sorter, uint16_t slot, funcDup_t* fn)
- [x] char* sorterGetConcatSlot(sorter_t* sorter, uint16_t slot)
- [x] void sorterCleanSlot(sorter_t* sorter, uint16_t slot, funcDelete_t* fd)
- [x] void sorterDelete(sorter_t* sorter, funcDelete_t* fd)
- [x] void sorterPrint(sorter_t* sorter, FILE *pFile, funcPrint_t* fp)
- [x] uint16_t fs_sizeModFive(char* s)
- [x] uint16_t fs_firstChar(char* s)
- [x] uint16_t fs_bitSplit(char* s)



## Compilación y Testeo

Para compilar el código y poder correr las pruebas cortas deberá ejecutar make main y luego
./runMain.sh. En cambio, para compilar el código y correr las pruebas intensivas deberá ejecutar
./runTester.sh.

### Realizado por:

~David Alejandro Venegas 783/18 venegasr.david@gmail.com

~Luis Ricardo Bustamante 43/18 luisbustamante097@gmail.com 

