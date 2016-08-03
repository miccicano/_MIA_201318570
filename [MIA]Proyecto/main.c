#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char** str_split(char* a_str, const char a_delim);
void automata(char Comando[300]);
char SinComilla (char frase[100]);

//*******************************************************************************************************************************
//---------------------------------------------------STRUCTS Y VARIBLES------------------------------------------------------
//*******************************************************************************************************************************
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

int DiscoAceptado = 0;//cero es falso

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
//________________________________________________________________________________________________________________________EXIT
            if(strcasecmp((*(Token + i)), "exit\n") ==0 )
                salida=1;
//________________________________________________________________________________________________________________________MKDISK
            if(strcasecmp((*(Token + i)), "mkdisk") ==0 ){
                ComandoMKdisk * MKDISK = malloc(sizeof(ComandoMKdisk));
                MKDISK->unit = 'm';
                int jadd;
                for (jadd = (i+1); *(Token + jadd); jadd++){
                    if (strncasecmp((*(Token + jadd)), "-size", 4) == 0){//****SIZE
                       // int tam =Getint(*(Token+jadd));
                        int tam=*(Token+jadd);
                        int tama=Getint(tam);
                        if (tama<=0)
                            printf("Tamaño de disco incorrecto");
                        else{
                            MKDISK->size = tama;
                            DiscoAceptado=1;//uno es verdadero
                        }
                    }

                    else if (strncasecmp((*(Token + jadd)), "+unit",4) == 0){//****UNIT
                        char * com;
                        com = strtok((*(Token + jadd)),"::");
                        com = strtok(NULL, "::");
                        MKDISK->unit = *com;
                    }
                    else if (strncasecmp((*(Token + jadd)), "-path", 5) == 0){//****PATH
                        char *spl;
                        spl = strtok((*(Token + jadd)), "::");
                        spl = strtok(NULL,"::");
                        printf("%s",spl);
                        char caracter[1]="\"";
                        char Resultante[100];
                        int i = 0;
                        int j = 0;
                        while (spl[i] != '\0')
                        {
                          if (caracter[0] != spl[i])
                          {
                            Resultante[j] = spl[i];
                            j++;
                          }
                          i++;
                        }
                        Resultante[j] = '\0';
                        strcpy(MKDISK->path, Resultante);
                    }
                    else if (strncasecmp((*(Token + jadd)), "-name", 4) == 0){//****NAME
                        char *spl;
                        spl = strtok((*(Token + jadd)), "::");
                        spl = strtok(NULL,"::");
                        char caracter[1]="\'";
                        char Resultante[100];
                        int i = 0;
                        int j = 0;
                        while (spl[i] != '\0')
                        {
                          if (caracter[0] != spl[i])
                          {
                            Resultante[j] = spl[i];
                            j++;
                          }
                          i++;
                        }
                        Resultante[j] = '\0';
                        strcpy(MKDISK->name, Resultante);
                    }

                }
                printf("size -> %d \n", MKDISK->size);
                printf("unit -> %s \n", &MKDISK->unit);
                printf("path -> %s \n", MKDISK->path);
                printf("name -> %s \n", MKDISK->name);

                CrearDisco(MKDISK->size, MKDISK->unit,MKDISK->path,MKDISK->name);


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


char SinComilla (char frase[100]){
    char caracter[1]="\'";
    char Resultante[100];
    int i = 0;
    int j = 0;
    while (frase[i] != '\0')
    {
      if (caracter[0] != frase[i])
      {
        Resultante[j] = frase[i];
        j++;
      }
      i++;
    }
    Resultante[j] = '\0';
    printf("%s",Resultante);
    return Resultante;
}

//*******************************************************************************************************************************
//---------------------------------------------------ACCIONES PARA EL DISCO------------------------------------------------------
//*******************************************************************************************************************************
//____________________________________________________________________________________________________________________CREAR DISCO
void CrearDisco(int tam, char unidad, char path[100], char name[100]){
    printf("%i", tam);
    printf("%c",unidad);
    printf("%s",path);
    printf("%s",name);


    if (DiscoAceptado==1)/*uno es aceptado*/{
        printf("entro a disco aceptado");
        if (strncasecmp(unidad, "m",1)==0){
            tam=tam*1024*1024; //COMANDO M 1024^2
            printf(" %d Megabytes\n",tam);

        }
        else if (strncasecmp(unidad, "K",1)==0)
        {
            tam=tam*1024; //COMANDO K 1024
            printf(" %d Megabytes\n",tam);
        }
        else
        {
            printf(">> ERROR:Unidad erronea %c\n",unidad );
        }
    }


}

//*******************************************************************************************************************************
//-----------------------------------------------------------MAIN----------------------------------------------------------------
//*******************************************************************************************************************************

int main(int argc, char *argv[])
{
    while (salida==0){
        printf("\n ***Ingresar Comando***\n");
        fgets(Comando,100,stdin);
            automata(Comando);
}
    return 0;
}

