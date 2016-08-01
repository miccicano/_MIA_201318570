#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char** str_split(char* a_str, const char a_delim);
void automata(char Comando[300]);

typedef struct ComandoMKdisk{
    int size;
    char unit;
    char path[100];
    char name[100];
}ComandoMKdisk;

char Comando[1000];
char *token;
char ins0[10];
char ins1[10];
char ins2[10];
char ins3[10];
char Comando3[1000];
int salida=0;

char** Token;
char * com;

//con esto hago el split de comando
char** str_split(char* a_str, const char a_delim){
    char** resultado = 0;
    size_t contador = 0;
    char* tmp = a_str;
    char* ultimoPara = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;
    while (*tmp){
        if (a_delim == *tmp){
            contador++;
            ultimoPara = tmp;
        }
        tmp++;
    }
    contador += ultimoPara < (a_str + strlen(a_str) - 1);
    contador++;
    resultado = malloc(sizeof(char*) * contador);
    if (resultado){
        size_t idx  = 0;
        char* token = strtok(a_str, delim);
        while (token){
            assert(idx < contador);
            *(resultado + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == contador - 1);
        *(resultado + idx) = 0;
    }
    return resultado;
}

//*******************************************************************************************************************************
//-----------------------------------------------------------ANALIZADOR----------------------------------------------------------
//*******************************************************************************************************************************
void automata(char Comando[300]){
    Token = str_split(Comando, ' ');
    if(Token){
        int i;
        for (i = 0; *(Token + i); i++){
            //printf("token=[%s]\n", *(Token + i));
//________________________________________________________________________________________________________________________EXIT
            if(strcasecmp((*(Token + i)), "exit\n") ==0 )
                salida=1;
//________________________________________________________________________________________________________________________MKDISK
            if(strcasecmp((*(Token + i)), "mkdisk") ==0 ){
                ComandoMKdisk * MKDISK = malloc(sizeof(ComandoMKdisk));
                MKDISK->unit = 'm';
                int jadd;
                for (jadd = (i+1); *(Token + jadd); jadd++){
                     //printf("token=[%s]\n", *(Token + i));
                    if (strncasecmp((*(Token + jadd)), "-size", 4) == 0){
                        MKDISK->size = Getint(*(Token+jadd));
                    }

                    else if (strncasecmp((*(Token + jadd)), "+unit",4) == 0){
                        char * com;
                        com = strtok((*(Token + jadd)),"::");
                        com = strtok(NULL, "::");
                        MKDISK->unit = *com;
                    }
                    else if (strncasecmp((*(Token + jadd)), "-path", 4) == 0){
                        char *spl;
                        spl = strtok((*(Token + jadd)), "::");
                        spl = strtok(NULL,"::");
                        //MKDISK->path=*spl;
                    }
                    else if (strncasecmp((*(Token + jadd)), "-name", 4) == 0)
                    {
                        char *spl;
                        spl = strtok((*(Token + jadd)), "::");
                        spl = strtok(NULL,"::");
                        //MKDISK->name=*spl;
                    }

                    // printf("token = [%s] \n", *(Token + r));
                }
                printf("size -> %d \n", MKDISK->size);
                printf("unit -> %s \n", &MKDISK->unit);
                printf("path -> %s \n", MKDISK->path);
                printf("name -> %s \n", MKDISK->name);


            }
        }
    }
}

//*******************************************************************************************************************************
//-----------------------------------------------SPLIT PARA COMANDO PEQUEÑO------------------------------------------------------
//*******************************************************************************************************************************
int Getint (char * token){
    char * com;
    com = strtok(token,"::");
    com = strtok(NULL, "::");
    int numero = atoi(com);
    return numero;
}


//*******************************************************************************************************************************
//---------------------------------------------------ACCIONES PARA EL DISCO------------------------------------------------------
//*******************************************************************************************************************************
//____________________________________________________________________________________________________________________CREAR DISCO
void CrearDisco(){
 char* aux1;
        char* aux3=inst;
        aux1=strtok(aux3,":");
        if(aux1==NULL){ }else
        if(strcasecmp(aux1,"-size")==0){
            aux1=strtok(NULL,":");
            size=aux1;
        }else if(strcasecmp(aux1,"-unit")==0){
            aux1=strtok(NULL,":");
            unit=aux1;
        }else if(strcasecmp(aux1,"-path")==0){
             aux1=strtok(NULL,":");
             char aux8[200];
              strcpy(aux8,aux1);
              if(aux8[0]=='"'){
            int i=0;
              while(aux8[i+1]!='"'){
                  aux8[i]=aux8[i+1];
                  if(aux8[i]==' '){
                   aux8[i]='_';
                  }
               i++;
              }
              aux8[i]='\0';
              aux8[i+1]='\0';
              }
           strcpy(aux1,aux8);
       pa=aux1;
        }else if(strcasecmp(aux1,"-ruta")==0){
            aux1=strtok(NULL,":");
            char aux8[200];
             strcpy(aux8,aux1);
             if(aux8[0]=='"'){
           int i=0;
             while(aux8[i+1]!='"'){
                 aux8[i]=aux8[i+1];
                 if(aux8[i]==' '){
                  aux8[i]='_';
                 }
              i++;
             }
             aux8[i]='\0';
             aux8[i+1]='\0';
             }
          strcpy(aux1,aux8);
      ruta=aux1;
       }else if(strcasecmp(aux1,"-cont")==0){
            aux1=strtok(NULL,":");
            char aux8[200];
             strcpy(aux8,aux1);
             if(aux8[0]=='"'){
           int i=0;
             while(aux8[i+1]!='"'){
                 aux8[i]=aux8[i+1];
                 if(aux8[i]==' '){
                  aux8[i]='_';
                 }
              i++;
             }
             aux8[i]='\0';
             aux8[i+1]='\0';
             }
}

//*******************************************************************************************************************************
//-----------------------------------------------------------MAIN----------------------------------------------------------------
//*******************************************************************************************************************************

int main(int argc, char *argv[])
{

     int i=0;

    while (salida==0){
        int i=0;
    printf("\n Ingresar comando\n");
   fgets(com,500,stdin);
          for( i=0;i<1000;i++){
              if(com[i] == '\n'){
                  com[i] = '\0';
              }else if(com[i]=='\\'){
                  int ii=i+1;
                  while(com[ii]!='\n'){
                      com[ii]=' ';
                      ii++;
                  }
                   com[ii]=' ';
                fgets(com3,500,stdin);
                strcat(com,com3);
                printf(com);
              }else{
               com[i]= tolower(com[i]);
              }
          }

       char *PalabraCadena = NULL;
       PalabraCadena = strtok( com, " ");
if(PalabraCadena!=NULL){
       if(strcmp(PalabraCadena,"exit")==0 || strcmp(PalabraCadena,"Exit")==0 || strcmp(PalabraCadena,"EXIT")==0 ){
                 salida=1;
                 }
}

     if(PalabraCadena!=NULL){
printf(PalabraCadena);}

    }


    return 0;
}

