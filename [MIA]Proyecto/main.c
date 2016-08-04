#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char** str_split(char* a_str, const char a_delim);
void automata(char Comando[300]);
char SinComilla (char frase[100]);
void CrearDisco(int tam, char unidad[1], char path[100], char name[100]);
void EliminarDisco(char path[100]);

//*******************************************************************************************************************************
//--------------------------------------------------------STRUCTS Y VARIBLES------------------------------------------------------
//*******************************************************************************************************************************

typedef struct ComandoStruc{
    int size;
    char unit[1];
    char path[100];
    char name[100];
    char type[1];
    char fit[2];
    char del[4];
    int add;
}ComandoStruc;

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
int DelDisco = 0;

FILE *fichero;


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
//________________________________________________________________________________________________________________________SALTO DE LINEA
            if(strcasecmp((*(Token + i)), "\\") ==0 ){}

//________________________________________________________________________________________________________________________COMENTARIO
            if(strcasecmp((*(Token + i)), "#") ==0 ){}

//________________________________________________________________________________________________________________________MK-DISK
            if(strcasecmp((*(Token + i)), "mkdisk") ==0 ){
                ComandoStruc * MKDISK = malloc(sizeof(ComandoStruc));
                char *eme = "m";
                strcpy(MKDISK->unit,eme);

                int jadd;
                for (jadd = (i+1); *(Token + jadd); jadd++){
                    //printf("token=[%s]\n", *(Token + i));
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
                        strcpy(MKDISK->unit,com);
                    }
                    else if (strncasecmp((*(Token + jadd)), "-path", 5) == 0){//****PATH
                        char *spl;
                        spl = strtok((*(Token + jadd)), "::");
                        spl = strtok(NULL,"::");
                        //printf("%s",spl);
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
                        strcpy(MKDISK->name, Resultante);
                    }

                    // printf("token = [%s] \n", *(Token + r));
                }
                /*      printf("size -> %d \n", MKDISK->size);
                printf("unit -> %s \n", &MKDISK->unit);
                printf("path -> %s \n", MKDISK->path);
                printf("name -> %s \n", MKDISK->name);*/

                CrearDisco(MKDISK->size, MKDISK->unit,MKDISK->path,MKDISK->name);


            }
//________________________________________________________________________________________________________________________RM-DISK
            if(strcasecmp((*(Token + i)), "rmdisk") ==0 ){
                ComandoStruc * RMDISK = malloc(sizeof(ComandoStruc));
                int jadd;
                for (jadd = (i+1); *(Token + jadd); jadd++){
                    if (strncasecmp((*(Token + jadd)), "-path", 5) == 0){//****PATH
                        DelDisco=1;
                        char *spl;
                        spl = strtok((*(Token + jadd)), "::");
                        spl = strtok(NULL,"::");
                        //printf("%s",spl);
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
                        strcpy(RMDISK->path, Resultante);
                    }
                    EliminarDisco(RMDISK->path);
                }
            }
//________________________________________________________________________________________________________________________F-DISK
            if(strcasecmp((*(Token + i)), "fdisk") ==0 ){
                ComandoStruc * FDISK = malloc(sizeof(ComandoStruc));
                char *ca = "K";
                strcpy(FDISK->unit,ca);
                char *pe ="P";
                strcpy(FDISK->type,pe);
                char *wf="WF";
                strcpy(FDISK->fit,wf);

                int jadd;
                for (jadd = (i+1); *(Token + jadd); jadd++){
                    if (strncasecmp((*(Token + jadd)), "-size", 5) == 0){//****SIZE
                        int tam=*(Token+jadd);
                        int tama=Getint(tam);
                        if (tama<=0)
                            printf("Tamaño de disco incorrecto");
                        else{
                            FDISK->size = tama;
                            DiscoAceptado=1;//uno es verdadero
                        }
                    }else if (strncasecmp((*(Token + jadd)), "+unit", 5) == 0){//****UNIT
                        char * com;
                        com = strtok((*(Token + jadd)),"::");
                        com = strtok(NULL, "::");
                        strcpy(FDISK->unit,com);
                    }else if (strncasecmp((*(Token + jadd)), "-path", 5) == 0){//****PATH
                        char *spl;
                        spl = strtok((*(Token + jadd)), "::");
                        spl = strtok(NULL,"::");
                        //printf("%s",spl);
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
                        strcpy(FDISK->path, Resultante);
                    }else if (strncasecmp((*(Token + jadd)), "+type", 5) == 0){//****TYPE
                        char * com;
                        com = strtok((*(Token + jadd)),"::");
                        com = strtok(NULL, "::");
                        strcpy(FDISK->type,com);

                    }else if (strncasecmp((*(Token + jadd)), "+fit", 4) == 0){//****FIT
                        char *spl;
                        spl = strtok((*(Token + jadd)), "::");
                        spl = strtok(NULL,"::");
                        //printf("%s",spl);
                        strcpy(FDISK->fit, spl);

                    }else if (strncasecmp((*(Token + jadd)), "+delete", 7) == 0){//****DELETE
                        char *spl;
                        spl = strtok((*(Token + jadd)), "::");
                        spl = strtok(NULL,"::");
                        //printf("%s",spl);
                        strcpy(FDISK->del, spl);

                    }else if (strncasecmp((*(Token + jadd)), "-name", 5) == 0){//****NAME
                        char *spl;
                        spl = strtok((*(Token + jadd)), "::");
                        spl = strtok(NULL,"::");
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
                        strcpy(FDISK->name, Resultante);
                    }else if (strncasecmp((*(Token + jadd)), "+add", 4) == 0){//****ADD
                        int tam=*(Token+jadd);
                        int tama=Getint(tam);
                            FDISK->size = tama;
                            //uno es verdadero
                    }
                    printf("size -> %i \n", FDISK->size);
                    printf("unit -> %s \n", &FDISK->unit);
                    printf("path -> %s \n", FDISK->path);
                    printf("name -> %s \n", FDISK->name);
                }
        }}
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

char GetChar (char * token){
    char * com;
    com = strtok(token,"::");
    com = strtok(NULL, "::");
    char valor[100];
    strcpy(valor, com);
    return *valor;
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
    return *Resultante;
}

//*******************************************************************************************************************************
//---------------------------------------------------ACCIONES PARA EL DISCO------------------------------------------------------
//*******************************************************************************************************************************
//____________________________________________________________________________________________________________________CREAR DISCO
void CrearDisco(int tam, char unidad[1], char path[100], char name[100]){
   printf("*******************\n");
    printf("%i \n", tam);
    printf("%s \n",unidad);
    printf("%s \n",path);
    printf("%s \n", name);
    printf("*****************\n");
    char path2[50];
    strcpy(path2,path);

    if (DiscoAceptado==1)/*uno es aceptado*/{
        if (strncasecmp(unidad, "m",1)==0){
            tam=tam*1024*1024;
            printf(" %d Mb\n",tam);
        }
        else if (strncasecmp(unidad, "K",1)==0){
            tam=tam*1024;
            printf(" %d Kb\n",tam);
        }
        else
            printf(">> ERROR:Unidad erronea %s\n",unidad );

        //crear el archivo binario
        char pathCompleta[100];
        strcpy(pathCompleta,strcat(path,name));

        char caracter[1]="\n";
        char Resultante[100];
        int i = 0;
        int j = 0;
        while (pathCompleta[i] != '\0')
        {
            if (caracter[0] != pathCompleta[i])
            {
                Resultante[j] = pathCompleta[i];
                j++;
            }
            i++;
        }
        Resultante[j] = '\0';

        strcpy(pathCompleta, Resultante);


        char SizeArch[tam];
        memset(SizeArch, 0, sizeof SizeArch);

        fichero = fopen (pathCompleta, "w+b" );
        if (fichero!=NULL) {
            fwrite(SizeArch,sizeof(SizeArch),1,fichero);
            fclose ( fichero );
        }else{
            char sys[10] = "mkdir -p ";
            strcat(sys,path);
            system(sys);
            fichero = fopen (pathCompleta, "r+b");
            if (fichero!=NULL){
                fwrite(SizeArch,sizeof(SizeArch),1,fichero);
                fclose ( fichero );
            }
            printf("no pos no existe, %s", sys);
        }
    }
    else
        printf(">> ERROR:Tamaño erroneo\n");

}


//____________________________________________________________________________________________________________________ELIMINAR DISCO
void  EliminarDisco(char path[100]){
    printf("%s",path);
    if(DelDisco==1){
        char caracter[1]="\n";
        char Resultante[100];
        int i = 0;
        int j = 0;
        while (path[i] != '\0')
        {
            if (caracter[0] != path[i])
            {
                Resultante[j] = path[i];
                j++;
            }
            i++;
        }
        Resultante[j] = '\0';

        strcpy(path, Resultante);


        fichero=fopen(path,"r+b");
        if(fichero!=NULL){
            char sys[10] = "rm ";
            strcat(sys,path);
            system(sys);
        }else
            printf(">>ERROR: El disco ingresado no existe.");
    }else
        printf(">>ERROR: El disco ingresado no existe.");
}

//*******************************************************************************************************************************
//-----------------------------------------------------------MAIN----------------------------------------------------------------
//*******************************************************************************************************************************

int main()
{
    while (salida==0){
        printf("\n ***Ingresar Comando***\n");
        fgets(Comando,100,stdin);
            automata(Comando);
}
    return 0;
}

