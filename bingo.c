/*POR HACER: DE MENOS A MAS PRIORIDAD
1-TODA LA RAMA DE JUGADOR VS jugador



*/

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

void jugarOrdenador(int row,int column,int blankSpace);
void tableroAutomatico(int **bingoCard1,int **bingoCard2,int row,int column,int blankSpace);
void poneNumsCarton(int **bingoCard1,int **bingoCard2,int row,int column,int blankSpace);
int compruebaErrorMatriz(int **bingoCard1,int row,int column,int random);
void poneBlancosCarton(int **bingoCard1, int **bingoCard2, int row, int column, int blankSpace);
int comprobarFila(int **bingoCard1, int **bingoCard2, int row, int column);
void compruebaCarton(int **bingoCard1, int **bingoCard2, int row, int column,int numb);
int compruebaBola(int array[99], int numb);
int compruebaGanador(int **bingoCard1, int **bingoCard2, int row, int column);



////////////////////////////////////////////////FUNIONES GENERALES/////////////////////////////////////////
void writeLogMessage(int option,char *msg,int **bingoCard1,int row,int column){
  char initialMsg[15];
  int i,j;
  time_t now = time(0);
  struct tm *tlocal = localtime(&now);//calculamos la hora actual
  char stnow[24];
  strftime(stnow, 24, "%d/%m/%y %H:%M:%S", tlocal);

  logFile = fopen("logBingo.txt","a");
  if(logFile == NULL){//Mandamos un mensaje de error si el archivo no se puede abrir
    perror("No se puede abrir el archivo");
  }

  switch(option){

    case 0://Instrucines mandadas por teclado
    sprintf(initialMsg,"Teclado");//Imprimimos con la etiqueta teclado
    fprintf(logFile,"[%s][%s]: %s\n",stnow,initialMsg,msg);
    break;

    case 1:
    sprintf(initialMsg,"Carton");//Imprimimos con la etiqueta carton
    fprintf(logFile,"[%s][%s]: %s\n",stnow,initialMsg,msg);
    //Lineas que imprimen la matriz en el log
    char line[80];
    sprintf(line, "%d %d\n", row, column);
    for(i=0;i<row;i++){
      line[0] = '\0';
      for(j=0;j<column;j++){
        char buffer[10];
        sprintf(buffer,"%d ",bingoCard1[i][j]);
        strcat(line,buffer);
      }
      int len = strlen(line);
      line[len - 1] = '\n';
      fputs(line, logFile);
    }
    break;

    case 2:
    sprintf(initialMsg,"Juego");
    fprintf(logFile,"[%s][%s]: %s\n",stnow,initialMsg,msg);
    break;

    case 3:
    sprintf(initialMsg,"Ganador");
    fprintf(logFile,"[%s][%s]: %s\n",stnow,initialMsg,msg);
    break;
  }
  fclose(logFile); //Cerramos el fichero despues de cada escritura para que se imprima en orden
}
////////////////////////////////////////////
int calculaAleatorios(int min, int max) {//Funcion que calcula aleatorios
  return rand() % (max - min + 1) + min;
}
//////////////////////////////////////////

int mainMenu(){//Funcion que me imprime un menu por pantalla y e devuelve la opcion elegida
  int menu;
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
  if(menu == 1){//Imprimimos la seleccion del menu en el log
    sprintf(msgMenu,"El juego se ejecutara en modo maquina vs maquina");
  }else if(menu == 2){
    sprintf(msgMenu,"El juego se ejecutara en modo jugador vs jugador");
  }else{
    sprintf(msgMenu,"Saliendo del juego, que tenga una buena tarde");
  }
  writeLogMessage(0,msgMenu,NULL,0,0);
  return menu;
}
////////////////////////////////////////////////

void juegoBingo(int menu){
  int row = 0,column = 0;
  int blankSpace;
  char msgVariables[100];
  //INtroducimos el tamaño que van a tener nuestros cartones
  printf("Introduzca el tamaño del carton, el maximo de numeros es 99\n");
  printf("FILAS:\t"); scanf("%d",&row);
  printf("COLUMNAS:\t"); scanf("%d",&column);
  while((row*column) > 99){//Control de errores
    printf("ERROR, HAY MAS HUECOS QUE NUMEROS\n");
    printf("Introduzca el tamaño del carton, el maximo de numeros es 99\n");
    printf("FILAS:\t"); scanf("%d",&row);
    printf("COLUMNAS:\t"); scanf("%d",&column);
  }
  //Introducimos el numero de huecos blancos que hay en cada fila
  printf("Ahora selecione el numero de hueco que quiere que haya en cada fila\n");
  scanf("%d",&blankSpace);
  while(blankSpace >= column || blankSpace <= 0){//COntrol de errores
    printf("ERROR, LOS ESPACIOS EN BLANCO DEBEN ESTAR ENTRE 1 Y %d\n",column-1);
    printf("Ahora selecione el numero de hueco que quiere que haya en cada fila\n");
    scanf("%d",&blankSpace);
  }
  printf("El tamaño de los cartones sera de %dx%d y habra %d huecos blancos por fila\n",row,column,blankSpace);
  //Imprimimos las caracteristicas de los cartones en el log
  sprintf(msgVariables,"Los cartones seran de %dx%d y habra %d huecos blancos por fila",row,column,blankSpace);
  writeLogMessage(0,msgVariables,NULL,0,0);

  if(menu==1){//Opcion de menu 1
    jugarOrdenador(row,column,blankSpace);
  }else{//Opcion de menu 2 (ya que la 3 no entra en esta funcion porque se sale antes de la aplicacion)
    //Funcion para iniciar el juego jugador vs jugador
  }
}

int compruebaErrorMatriz(int **bingoCard,int row,int column, int random){//COmprobamos que no hay ningun valor reperido dentro de lamatriz
  int outValue;

  int i,j;
  for(i=0;i<row;i++){
    for(j=0;j<column;j++){
      if(random == bingoCard[i][j]){
        return -2;//Valor para cuando se repite un valor en la matriz
      }
    }
  }
  return 0;//Para cuando elvalor no se repite
}

int ordenaVector(int **bingoCard, int row, int column){//Ordenamos la matriz  a un vector de tamaño row*column


  int tam = row*column;;
  int *arrayAux;
  arrayAux = (int *)malloc(tam*sizeof(int));
  int aux;//ausialiar del metodo de la burbuja
  int m = 0,n = 0;//auxialiares para pasar de matriz a vector y viceversa

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
return **bingoCard;
}

void imprimeCarton(int **bingoCard1,int **bingoCard2,int row, int column){

  int i,j;

  printf("Carton Jugador 1\n");//Imprimimos carton 1
  for(i=0;i<row;i++){
    for(j=0;j<column;j++){

      if(bingoCard1[i][j] == -1){
        printf("X\t");
      }else if(bingoCard1[i][j] == -2){
        printf("$\t");

    }else{
      printf("%d\t",bingoCard1[i][j]);
    }
  }
  printf("\n");
}
printf("\n");

printf("Carton Jugador 2\n"); //Imprimimos carton 2
for(i=0;i<row;i++){
  for(j=0;j<column;j++){
    if(bingoCard2[i][j] == -1){
      printf("X\t");
    }else if(bingoCard2[i][j] == -2){
      printf("$\t");
  }else{
    printf("%d\t",bingoCard2[i][j]);
  }
  }
  printf("\n");
}


writeLogMessage(1,"Carton jugador 1",bingoCard1,row,column);//Mandamos a imprimir la matriz del j1
writeLogMessage(1,"Carton jugador 2",bingoCard2,row,column);//Mandamos a imprimir la matriz del j2

}

void jugar(int **bingoCard1, int **bingoCard2, int row, int column){

  int balls[99];//array donde guardamos las volas que van saliendo
  int aleat;
  char msg[100];
  int rowFlag = 0;//Inicializamos el flag de comprobar fila, una vez que un jugador cante fila, el valor cambaira y no se volvera a comprobar fila
  int backRow;
  int backBingo;
  int bingoFlag = 0;
  printf("El juego esta listo para empezar\n");
  writeLogMessage(2,"Juego listo para emepzar, que lo disfrute",NULL,0,0);
  imprimeCarton(bingoCard1,bingoCard2,row,column);//Imprimimos el carton

  int i;
  for(i=0;i<99;i++){ //Bucle que genera como maxumo 99 iteraciones o hasta que se declare algun ganador del juego
    aleat = calculaAleatorios(1,99);//Calculamos el aleatorio de las bolas;
    sleep(2);

    while(compruebaBola(balls,aleat)==-2){//Si nos retorna -2 es que la hay un numero repetido en el array de las bolas
      aleat=calculaAleatorios(1,99);//Recalculamos el aleatorio, solosaldra del bucle cunado el numero no se repita
    }
    balls[i] = aleat;
    printf("La bola que ha salido es la numero: %d\t\n",balls[i]); //Ddecimo que bola ha salido y en que iteracion
    sprintf(msg,"Ha salido el numero %d en la iteracion %d", balls[i],i);
    writeLogMessage(2,msg,0,0,0);

    printf("Los cartones quedan asi : \n");

    compruebaCarton(bingoCard1,bingoCard2,row,column,aleat);//Lamamos a la fucnion que comprueba si el numero esta en el carton o no, y luego lo imprime
    //Comprobamos fila
    if(rowFlag == 0){
      backRow = comprobarFila(bingoCard1,bingoCard2,row,column);
      if(backRow == 1){
        printf("El jugador 1 ha cantado fila en la iteracion %d\n",i);
        writeLogMessage(2,"El jugador 1 ha cantado fila",0,0,0);
        rowFlag = 1;//cambiampos el flag de comprobar fila para que no se vuelva a comprobar
      }else if(backRow == 2){
        printf("EL jugador 2 ha cantado fila en la iteracion %d\n",i);
        writeLogMessage(2,"El jugador 2 ha cantado fila",0,0,0);
        rowFlag = 1;//cambiampos el flag de comprobar fila para que no se vuelva a comprobar
      }else{
        printf("Nadie ha cantado fila\n");
      }
    }
    //Comprobamos bingo
    if(bingoFlag == 0){
      backBingo = compruebaGanador(bingoCard1,bingoCard2,row,column);
      if(backBingo == 1){
        printf("GANA EL JUGADOR 1, ENHORABUENA\n");
        writeLogMessage(3,"Gana el jugador 1",0,0,0);
        bingoFlag = 1;
      }else if( backBingo == 2){
        printf("GANA EL JUGADOR 2, ENHORABUENA\n");
        writeLogMessage(3,"Gana el jugador 2",0,0,0);
        bingoFlag = 1;
      }else{
        printf("Sin ganador por el momento\n");
      }
    }
    if(bingoFlag == 1){
      printf("JUEGO TERMINADO, SERA LLEVADO AL MENU PRINCIPAL\n");
      writeLogMessage(2,"JUEGO TERMINADO",0,0,0);
      i=200; //Terminamos el bucle manualemte y retornamos al menu
      int z;
      for(z=0;z<3;z++){
        printf("Saliendo\n");
        sleep(1);
      }
    }
  }  //Repetimos
}

////////////////////////////////////////////////7

int compruebaBola(int array[99], int numb){ //Hacemos que no salga la misma bola dos veces

  int i;
  for(i=0;i<99;i++){
    if(numb == array[i]){
      return -2;//Valor para cuando se repite un valor del vector
    }
  }
  return 0; //Valor para cuando no se repite un valor del vector
}

///////////////////////////////////////////////////

void compruebaCarton(int **bingoCard1, int **bingoCard2, int row, int column,int numb){ //COmprobar cuadno hay un numero en el carton

  int i,j;

  for(i=0;i<row;i++){
    for(j=0;j<column;j++){
      if(bingoCard1[i][j] == numb){//Carton 1
        bingoCard1[i][j] = -2;
      }
      if(bingoCard2[i][j] == numb){//Carton 2
        bingoCard2[i][j] = -2;
      }
    }
  }
  imprimeCarton(bingoCard1,bingoCard2,row,column);//Imprimimos el carton
}

/////////////////////////////////////////////////////////////////

int comprobarFila(int **bingoCard1, int **bingoCard2, int row, int column){ //Devulve 1 si hay fila en jugador 1, 2 si hay fila en jugador 2 y 0 si no hay fila en esta jugada

  int count1 = 0;
  int count2 = 0;
  int i,j;

  for(i=0;i<row;i++){
    count1 = 0;
    count2 = 0;
    for(j=0;j<column;j++){

      if(bingoCard1[i][j] < 0){ //Recorremos las filas, y aumentamos el contador hasta que toda una fila sea negativa, el contador se reinicia antes de cada iteracion
        count1++;
      }
      if(bingoCard2[i][j] < 0){
        count2++;
      }
      if(count1 == column){
        return 1;
      }else if(count2 == column){
        return 2;
      }
    }
  }

  return 0;
}

int compruebaGanador(int **bingoCard1, int **bingoCard2, int row, int column){

  int p1accountant = 0;
  int p2accountant = 0;
  int i,j;
  for(i=0;i<row;i++){
    for(j=0;j<column;j++){
      if(bingoCard1[i][j] < 0 ){
        p1accountant++;
        if(p1accountant == row*column){
          return 1; //Ha ganado el jugador 1
        }
      }
      if(bingoCard2[i][j] < 0){
        p2accountant++;
        if(p2accountant == row*column){
          return 2; //Ha ganado el jugador 2
        }
      }
    }
  }
  return 0;//No ha habido ganador
}

/////////////////////////////////////////////////////RAMA MAQUINA VS MAQUINA//////////////////////////////////////////////////////

void jugarOrdenador(int row, int column, int blankSpace){//Reservamos espacio para la matriz que sera nuestro carton de juego
  int **bingoCard1;
  int  **bingoCard2;
  bingoCard1 = (int **)malloc(row*sizeof(int *));
  bingoCard2 = (int **)malloc(row*sizeof(int *));
  int i;
  for(i=0;i<row;i++){
    bingoCard1[i]=(int *)malloc(column*sizeof(int *));
    bingoCard2[i]=(int *)malloc(column*sizeof(int *));
  }
  tableroAutomatico(bingoCard1,bingoCard2,row,column,blankSpace);
}


/////////////////////////////////////////////////////////

void tableroAutomatico(int **bingoCard1,int **bingoCard2,int row,int column,int blankSpace){
  int i,j;

  for(i=0;i<row;i++){
    for(j=0;j<column;j++){
      bingoCard1[i][j]=0;
      bingoCard2[i][j]=1;
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
writeLogMessage(1,"Carton para la partida j1 a 0",bingoCard1,row,column);//Mandamos a imprimir la matriz
writeLogMessage(1,"Carton para la partida j2 a 1",bingoCard2,row,column);//Mandamos a imprimir la matriz
//*/
poneNumsCarton(bingoCard1,bingoCard2,row,column,blankSpace);
}
////////////////////////////

void poneNumsCarton(int **bingoCard1,int **bingoCard2,int row,int column,int blankSpace){

  int random,random2;
  int i,j;
  int matrixMistake;
  for(i=0;i<row;i++){
    for(j=0;j<column;j++){

      random = calculaAleatorios(1,99);//Calculamos un aleatorio entre 1 y el numero maximo de numeros que es 99
      random2 = calculaAleatorios(1,99);

      while(compruebaErrorMatriz(bingoCard1,row,column,random)==-2){//Si nos retorna -2 es que la hay un numero repetido en la funcion(carton1)
        random = calculaAleatorios(1,99);//Recalculamos el aleatorio, solosaldra del bucle cunado el numero no se repita
      }
      while(compruebaErrorMatriz(bingoCard2,row,column,random2)==-2){//Si nos retorna -2 es que la hay un numero repetido en la funcion(carton2)
        random2 = calculaAleatorios(1,99);//Recalculamos el aleatorio, solosaldra del bucle cunado el numero no se repita
      }
      bingoCard1[i][j] = random;//Metemos el aleatorio dentro de la matriz j1
      bingoCard2[i][j] = random2;//Metemos el aleatorio dentro de la matriz j2
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

**bingoCard1 = ordenaVector(bingoCard1,row,column);//llamamos a la funcion que nos ordenara la matriz
**bingoCard2 = ordenaVector(bingoCard2,row,column);
/*DEBUG
writeLogMessage(1,"Carton para la partida del jugador 1 ordenado",bingoCard1,row,column);//Mandamos a imprimir la matriz del j1
writeLogMessage(1,"Carton para la partida del jugador 2 ordenado",bingoCard2,row,column);//Mandamos a imprimir la matriz del j2
*/
poneBlancosCarton(bingoCard1,bingoCard2,row,column,blankSpace);
}
/////////////////////////////////////
void poneBlancosCarton(int **bingoCard1, int **bingoCard2, int row, int column, int blankSpace){//Ponemois los blancos en el carton

  int returned;
  int i,j,k,l,m,n,o,p;
  int aux1,aux2;
  int count1 = 0;
  int count2 = 0;

  for(l=0;l<row;l++){
    count1 = 0;
    count2 = 0;

    while(count1 != blankSpace){

      aux1=calculaAleatorios(0,column-1); //Recorremos las filas de la matriz, y no para hasta que aux = al numero de huecos puesto, asi garantizamos que no se repitan posiciones
      if(bingoCard1[l][aux1] != -1){
        count1++;
        bingoCard1[l][aux1] = -1;
      }
    }

    while(count2 != blankSpace){

      aux2=calculaAleatorios(0,column-1);
      if(bingoCard2[l][aux2] != -1){
        count2++;
        bingoCard2[l][aux2] = -1;
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

jugar(bingoCard1,bingoCard2,row,column);

}

/////////////////////////////////////////////////////RAMA JUGADOR VS JUGADOR//////////////////////////////////////////////////////

/////////////////////////////////////////////////////MAIN/////////////////////////////////////////////////////////
int main(){
  srand (time(NULL));//nos permite generar mas de un aleatorio por minuto
  remove("logBingo.txt");//Borramos el aterior archivo de log
  //Declaracion de variables
  int menu;
  int infinite = -1;//Variable del bucle infinito

  do{//bucle infinito
    menu = mainMenu();//Llamamos al menu
    if(menu == 3){//Si se recibe un 3, cerramos el programa
      printf("GRACIAS POR JUGAR, QUE TENGA UN BUEN DIA\n");

      exit(-1);
    }
    juegoBingo(menu);//Llamamos a la funcion juegoBingo

  }while(infinite = -1);//bucle infinito
}
