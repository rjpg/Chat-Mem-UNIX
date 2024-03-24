#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>  
#include <string.h>   
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>     
#include <signal.h> // control-c ....


#include <sys/ipc.h> //partilha de memoria 
#include <sys/shm.h>

#include <readline/readline.h>  //ler linhas 
#include <readline/history.h>

#define byte unsigned char
#define KEY 5678

#define MAXARGS 100       //numero max de argumentos (utilizadores)
#define MAXCHAR 300       //tamanho max da msg

#define IN 1           //entrar no server
#define OUT 2          // sair 
#define CHAT 0         // mensagem
#define LIST 3
 
typedef struct msg 
{
  char msg[MAXCHAR];   //mensagem
  byte type;            //tipo de msg
  int id;              //chave do cliente 
  int id_to;           // mais tarde .... 
}*lp_msg, s_msg;

typedef struct charmsg{
  byte msg[MAXCHAR];
}*lp_charmsg,s_charmsg;

// Semaferos

struct sembuf{
  short semNum; // num sem afectado por semOp
  short semOP;  //operção sobre o sem
  short semFlag; // verde ou vermelho ...
};
  
#define UP(sid,n){ \
  struct sembuf up={n,1,0}; \
  semop ( sid, &up,1); \
}

#define DOWN(sid,n){ \
  struct sembuf down={n,-1,0}; \
  semop ( sid, &down,1); \
  }

#define INIT(sid,n) UP(sid,n)

