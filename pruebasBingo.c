#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>


int calculaAleatorios(int min, int max) {//Funcion que calcula aleatorios
  return rand() % (max - min + 1) + min;
}
int main(){

  char caract[10][10];

  int i,j;
  for(i=0;i<10;i++){
    for(j=0;j<10;j++) {

      caract[i][j] = calculaAleatorios(65,90);
    }
  }


  for(i=0;i<10;i++){
    for(j=0;j<10;j++) {
      printf("%s\t",caract[i][j]);
    }
    printf("\n");
  }

}
