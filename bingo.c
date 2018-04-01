#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

FILE *logFile;

int menu; //Variable global del menu

void jugarOrdenador(int row,int column,int blankSpace);
void tableroAutomatico(int **bingoCard1,int **bingoCard2,int row,int column,int blankSpace);
void poneNumsCarton(int **bingoCard1,int **bingoCard2,int row,int column,int blankSpace);
int compruebaErrorMatriz(int **bingoCard1,int row,int column,int random);
void poneBlancosCarton(int **bingoCard1, int **bingoCard2, int row, int column, int blankSpace);

////////////////////////////////////////////////FUNIONES GENERALES/////////////////////////////////////////
void writeLogMessage(int option,char *msg,int **bingoCard1,int row,int column){
  char initialMsg[15];
  time_t now = time(0);
  struct tm *tlocal = localtime(&now);//calculamos la hora actual
  char stnow[24];
  strftime(stnow, 24, "%d/%m/%y %H:%M:%S", tlocal);
  char fuenteLog[15];
  logFile = fopen("logBingo.txt","a");
  if(logFile==NULL){//Mandamos un mensaje de error si el archivo no se puede abrir
    perror("NO se puede abrir el archivo");
  }

  switch(option){
    case 0://Instrucines mandadas por teclado
    sprintf(initialMsg,"Teclado");//Imprimimos con la etiqueta teclado
    fprintf(logFile,"[%s][%s]: %s\n",stnow,initialMsg,msg);
    break;
    case 1:
    sprintf(initialMsg,"Carton");//Imprimimos con la etiqueta carton
    fprintf(logFile,"[%s][%s]: %s\n",stnow,initialMsg,msg);

    char linea[80];
    sprintf(linea, "%d %d\n", row, column);
    //  fputs(linea, logFile);
    for (int i = 0; i < row; i++) {
      linea[0] = '\0';
      for (int j = 0; j < column; j++) {
        char buffer[10];
        sprintf(buffer, "%d ", bingoCard1[i][j]);
        strcat(linea, buffer);
      }
      int len = strlen(linea);
      linea[len - 1] = '\n';
      fputs(linea, logFile);
    }
    break;


  }
  fclose(logFile);
}
////////////////////////////////////////////
int calculaAleatorios(int min, int max) {//Funcion que calcula aleatorios
  return rand() % (max - min + 1) + min;
}
//////////////////////////////////////////

int mainMenu(){//Funcion que me imprime un menu por pantalla y e devuelve la opcion elegida

  char msgMenu[100];
  printf("Selecione una opcion\n");
  printf("1-Jugar maquina vs maquina\n");
  printf("2-Jugar usuario vs usuario\n");
  printf("3-Salir\n");
  scanf("%d",&menu );
  while(menu>3 || menu<1){//Control de errores
    printf("ERROR,LA OPCION SELECIONADA NO EXISTE\n");
    printf("Selecione una opcion\n");
    printf("1-Jugar maquina vs maquina\n");
    printf("2-Jugar usuario vs usuario\n");
    printf("3-Salir\n");
    scanf("%d",&menu );
  }
  if(menu==1){
    sprintf(msgMenu,"El juego se ejecutara en modo maquina vs maquina");
  }else if(menu==2){
    sprintf(msgMenu,"El juego se ejecutara en modo jugador vs jugador");
  }else{
    sprintf(msgMenu,"Saliendo del juego, que tenga una buena tarde");
  }
  writeLogMessage(0,msgMenu,NULL,0,0);
  return menu;
}
////////////////////////////////////////////////

void juegoBingo(int menu){
  int row=0,column=0;
  int blankSpace;
  char msgVariables[100];
  //INtroducimos el tamaño que van a tener nuestros cartones
  printf("Introduzca el tamaño del carton, el maximo de numeros es 99\n");
  printf("FILAS:\t"); scanf("%d",&row);
  printf("COLUMNAS:\t"); scanf("%d",&column);
  while((row*column)>99){//Control de errores
    printf("ERROR, HAY MAS HUECOS QUE NUMEROS\n");
    printf("Introduzca el tamaño del carton, el maximo de numeros es 99\n");
    printf("FILAS:\t"); scanf("%d",&row);
    printf("COLUMNAS:\t"); scanf("%d",&column);
  }
  //Introducimos el numero de huecos blancos que hay en cada fila
  printf("Ahora selecione el numero de hueco que quiere que haya en cada fila\n");
  scanf("%d",&blankSpace);
  while(blankSpace>=column || blankSpace<=0){//COntrol de errores
    printf("ERROR, LOS ESPACIOS EN BLANCO DEBEN ESTAR ENTRE 1 Y %d\n",column-1);
    printf("Ahora selecione el numero de hueco que quiere que haya en cada fila\n");
    scanf("%d",&blankSpace);
  }
  sprintf(msgVariables,"Los cartones seran de %dx%d y habra %d huecos blancos por fila",row,column,blankSpace);
  writeLogMessage(0,msgVariables,NULL,0,0);

  if(menu==1){//Opcion de menu 1
    jugarOrdenador(row,column,blankSpace);
  }else{//Opcion de menu 2(ya que la 3 no entra en esta funcion porque se sale antes de la aplicacion)

  }
}

int compruebaErrorMatriz(int **bingoCard,int row,int column, int random){//COmprobamos que no hay ningun valor reperido dentro de lamatriz
  int outValue;
  int i,j;

  for(i=0;i<row;i++){
    for(j=0;j<column;j++){
      if(random==bingoCard[i][j]){
        return -2;//Valor para cuando se repite un valor en la matriz
      }
    }
  }
  return 0;//Para cuando elvalor no se repite
}

int ordenaVector(int **bingoCard, int row, int column){//Ordenamos la matriz  a un vector de tamaño row*column


  int tam= row*column;
  int *arrayAux;
  arrayAux=(int *)malloc(tam*sizeof(int));
  int aux;//ausialiar del metodo de la burbuja
  int m=0,n=0;//auxialiares para pasar de matriz a vector y viceversa

  int i,j,k,l;
  for(i=0;i<column;i++){//recorremos la matriz por columnas pora ordenarlas por columnas
    for(j=0;j<row;j++){

      arrayAux[m++] = bingoCard[j][i];//Metemos la matriz en un vector

    }
  }


  for(k=0;k<tam;k++){//Algoritmo de la burbuja para ordenar el vector
    for(l=0;l<tam-1;l++){
      if(arrayAux[l] > arrayAux[l+1]){
        aux = arrayAux[l];
        arrayAux[l] = arrayAux[l+1];
        arrayAux[l+1] = aux;
      }
    }
  }


  for(i=0;i<column;i++){//Hacemos lo mismo que en las lineas anteriores recorremos la matriz por COLUMNAS
    for(j=0;j<row;j++){
      bingoCard[j][i] = arrayAux[n++];
    }
  }
  /*DEBUG
  for(i=0;i<row;i++){
  for(j=0;j<column;j++){
  printf("%d\t",bingoCard[i][j]);
}
printf("\n");
}
*/
//  writeLogMessage(1,"Carton para la partida aleatorio ordenado",bingoCard,row,column);//Mandamos a imprimir la matriz
return **bingoCard;
}

/*int compruebaErrorBlancos(int **cardSpaces, int row, int column, int blankSpace){   //NO FUNCIONA, ARREGLAR

  int i,j;
  for(i=0;i<row;i++){
    for(j=0;j<blankSpace;j++) {
      if(cardSpaces[1][j] == cardSpaces[1][j-1]){
        return -2;
      }
    }
  }
  return 0;
}
*/

/////////////////////////////////////////////////////RAMA MAQUINA VS MAQUINA//////////////////////////////////////////////////////

void jugarOrdenador(int row, int column, int blankSpace){//Reservamos espacio para la matriz que sera nuestro carton de juego
  int **bingoCard1;
  int  **bingoCard2;
  bingoCard1=(int **)malloc(row*sizeof(int *));
  int i;
  for(i=0;i<row;i++){
    bingoCard1[i]=(int *)malloc(column*sizeof(int *));
  }

  bingoCard2=(int **)malloc(row*sizeof(int *));
  int j;
  for(j=0;j<row;j++){
    bingoCard2[j]=(int *)malloc(column*sizeof(int *));
  }
  tableroAutomatico(bingoCard1,bingoCard2,row,column,blankSpace);
}
/////////////////////////////

void tableroAutomatico(int **bingoCard1,int **bingoCard2,int row,int column,int blankSpace){
  int i,j;

  for(i=0;i<row;i++){
    for(j=0;j<column;j++){
      bingoCard1[i][j]=0;
      bingoCard2[i][j]=1;
      /*DEBUG
      printf("%d\t",bingoCard1[i][j]);

      printf("%d\n",bingoCard2[i][j]);
      */
    }
    //printf("\n");
  }

  writeLogMessage(1,"Carton para la partida j1 a 0",bingoCard1,row,column);//Mandamos a imprimir la matriz
  writeLogMessage(1,"Carton para la partida j2 a 1",bingoCard2,row,column);//Mandamos a imprimir la matriz
  poneNumsCarton(bingoCard1,bingoCard2,row,column,blankSpace);
}
////////////////////////////

void poneNumsCarton(int **bingoCard1,int **bingoCard2,int row,int column,int blankSpace){

  int  random,random2;
  int i,j;
  int matrixMistake;
  for(i=0;i<row;i++){
    for(j=0;j<column;j++){

      random=calculaAleatorios(1,99);//Calculamos un aleatorio entre 1 y el numero maximo de numeros que es 99
      random2=calculaAleatorios(1,99);

      while(compruebaErrorMatriz(bingoCard1,row,column,random)==-2){//Si nos retorna -2 es que la hay un numero repetido en la funcion(carton1)
        random=calculaAleatorios(1,99);//Recalculamos el aleatorio, solosaldra del bucle cunado el numero no se repita
      }
      while(compruebaErrorMatriz(bingoCard2,row,column,random2)==-2){//Si nos retorna -2 es que la hay un numero repetido en la funcion(carton2)
        random2=calculaAleatorios(1,99);//Recalculamos el aleatorio, solosaldra del bucle cunado el numero no se repita
      }
      bingoCard1[i][j]=random;//Metemos el aleatorio dentro de la matriz j1
      bingoCard2[i][j]=random2;//Metemos el aleatorio dentro de la matriz j2
    }
    printf("\n");
  }
  /*DEBUG**
  for(i=0;i<row;i++){
  for(j=0;j<column;j++){
  printf("%d\t",bingoCard1[i][j]);
  printf("%d\t",bingoCard2[i][j]);
}
printf("\n");
}
*/

**bingoCard1=ordenaVector(bingoCard1,row,column);//llamadmaos a la funcion que nos ordenara la matriz
**bingoCard2=ordenaVector(bingoCard2,row,column);

writeLogMessage(1,"Carton para la partida del jugador 1 ordenado",bingoCard1,row,column);//Mandamos a imprimir la matriz del j1
writeLogMessage(1,"Carton para la partida del jugador 2 ordenado",bingoCard2,row,column);//Mandamos a imprimir la matriz del

poneBlancosCarton(bingoCard1,bingoCard2,row,column,blankSpace);
}
/////////////////////////////////////
void poneBlancosCarton(int **bingoCard1, int **bingoCard2, int row, int column, int blankSpace){

  int **aux;
  int returned;
  int i,j,k,l,m,n,o,p;

  aux=(int **)malloc(row*sizeof(int *));

  for(p=0;p<row;p++){
    aux[p]=(int *)malloc(column*sizeof(int *));
  }

  for(l=0;l<row;l++){
    for(k=0;k<blankSpace;k++){
      /*
      while(compruebaErrorBlancos(aux,row,column,blankSpace)==-2){//Si nos retorna -2 es que la hay un numero repetido en la funcion(carton1)
        aux[l][k]=calculaAleatorios(0,column);
      }
      */
      aux[l][k]=calculaAleatorios(0,column);

    }
  }

  /*DEBUG*/
  for(l=0;l<row;l++){
    for(k=0;k<blankSpace;k++){
      printf("%d\t",aux[l][k]);
    }
    printf("\n");
  }
  //*/


  for(i=0;i<row;i++){
    for(j=0;j<column;j++){

      for(k=0;k<blankSpace;k++){
        bingoCard1[i][aux[i][k]]=-1;


      }
    }

  }

  /*DEBUG
  for(i=0;i<row;i++){
  for(j=0;j<column;j++){
  printf("%d\t",bingoCard1[i][j]);
  printf("%d\t",bingoCard2[i][j]);
}
printf("\n");
}
*/

writeLogMessage(1,"Carton para la partida del jugador 1 ordenado y con huecos",bingoCard1,row,column);//Mandamos a imprimir la matriz del j1


}



/////////////////////////////////////////////////////RAMA JUGADOR VS JUGADOR//////////////////////////////////////////////////////

/////////////////////////////////////////////////////MAIN/////////////////////////////////////////////////////////
int main(){
  srand (time(NULL));//nos permite generar mas de un aleatorio por minuto
  remove("logBingo.txt");//Borramos el aterior archivo de log
  //Declaracion de variables
  int menu;
  int infinite=-1;//Variable del bucle infinito

  do{//bucle infinito
    menu=mainMenu();//Llamamos al menu
    if(menu==3){//Si se recibe un 3, cerramos el programa
      printf("GRACIAS POR JUGAR, QUE TENGA UN BUEN DIA\n");
      exit(-1);
    }
    juegoBingo(menu);//Llamamos a la funcion juegoBingo

  }while(infinite=-1);//bucle infinito
}
