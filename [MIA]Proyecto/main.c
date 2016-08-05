#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char** str_split(char* a_str, const char a_delim);
void automata(char Comando[300]);
char SinComilla (char frase[100]);
void CrearDisco(int tam, char unidad[1], char path[100], char name[100]);
void EliminarDisco(char path[100]);
void  CrearParticion(int tam, char unidad[1], char path[100], char name[100], char type[1], char fit[2]);
//*******************************************************************************************************************************
//--------------------------------------------------------STRUCTS Y VARIBLES------------------------------------------------------
//*******************************************************************************************************************************

typedef struct ComandoMKdisk{
    int size;
    char unit[1];
    char path[100];
    char name[100];
}ComandoMKdisk;

typedef struct Fdisk{
    int size;
    char unit[1];
    char path[100];
    char name[100];
    char type[1];
    char fit[2];
    char del[4];
    int add;
}Fdisk;

char Comando[1000];
int salida=0;

char** Token;
char * com;
//permisos para mkdisk
int DiscoAceptado = 0;
int DiscoAceptadoname=0;
int DiscoAceptadopath=0;
//permisos para rmdisk
int DelDisco = 0;
//permisos para fdisk
int FperAdd=0;
int FperPath=0;
int FperName=0;
int FperDel=0;
int FperFit=0;
int FperSize=0;

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
            if(strcasecmp((*(Token + i)), "exit") ==0 )
                salida=1;
//________________________________________________________________________________________________________________________SALTO DE LINEA
            if(strcasecmp((*(Token + i)), "\\") ==0 ){}

//________________________________________________________________________________________________________________________COMENTARIO
            if(strcasecmp((*(Token + i)), "#") ==0 ){}

//________________________________________________________________________________________________________________________MK-DISK
            if(strcasecmp((*(Token + i)), "mkdisk") ==0 ){
                ComandoMKdisk * MKDISK = malloc(sizeof(ComandoMKdisk));
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
                        DiscoAceptadopath=1;
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
                        DiscoAceptadoname=1;
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
                ComandoMKdisk * RMDISK = malloc(sizeof(ComandoMKdisk));
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
                Fdisk * FDISK = malloc(sizeof(Fdisk));
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
                            FperSize=1;//uno es verdadero
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
                        printf("*************unit -> %s \n", &FDISK->unit);
                        strcpy(FDISK->path, Resultante);
                        printf("*************unit -> %s \n", &FDISK->unit);
                        FperPath=1;
                        printf("*************unit -> %s \n", &FDISK->unit);
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
                        FperFit=1;

                    }else if (strncasecmp((*(Token + jadd)), "+delete", 7) == 0){//****DELETE
                        char *spl;
                        spl = strtok((*(Token + jadd)), "::");
                        spl = strtok(NULL,"::");
                        //printf("%s",spl);
                        strcpy(FDISK->del, spl);
                        FperDel=1;

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
                        FperName=1;
                    }else if (strncasecmp((*(Token + jadd)), "+add", 4) == 0){//****ADD
                        int tam=*(Token+jadd);
                        int tama=Getint(tam);
                            FDISK->size = tama;
                            FperAdd=1;//uno es verdadero
                    }

                }
                printf("size -> %i \n", FDISK->size);
                printf("unit -> %s \n", &FDISK->unit);
                printf("path -> %s \n", FDISK->path);
                printf("name -> %s \n", FDISK->name);
                printf("type -> %s \n", FDISK->type);
                printf("fit -> %s \n", FDISK->fit);
                printf("delete -> %s \n", FDISK->del);
                printf("add -> %s \n", FDISK->add);

                if((FperAdd==1 && FperDel==0)){//~~~~~~~~~~ADD
                    printf("Entro al add \n");
                }else if((FperAdd==0 && FperDel==1)){//~~~~~~~~~~DELETE
                    printf("Entro al delete \n");
                }else if(FperPath==1 && FperName==1 && FperSize==1){//~~~~~~~~~~CREAR PARTICION
                    printf("Entro a crear particion. \n");
                    CrearParticion(FDISK->size, FDISK->unit,FDISK->path,FDISK->name,FDISK->type,FDISK->fit);
                }else if((FperAdd==1 && FperDel==1) || (FperAdd==0 && FperDel==1) ||(FperAdd==1 && FperDel==0)){//~~~~~~~~~~ERROR
                    printf(">>ERROR: No se puede eliminar o agregar en un mismo comando. \n");
                }
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
    printf("%s ", name);
    printf("*****************\n");
    char path2[50];
    strcpy(path2,path);

    if (DiscoAceptado==1 && DiscoAceptadoname==1 && DiscoAceptadopath==1)/*uno es aceptado*/{
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
            printf(">>Archivo creado en la dirección:  %s", pathCompleta);
        }else{
            char sys[10] = "mkdir -p ";
            strcat(sys,path);
            system(sys);
            fichero = fopen (pathCompleta, "r+b");
            if (fichero!=NULL){
                fwrite(SizeArch,sizeof(SizeArch),1,fichero);
                fclose ( fichero );
            }
            printf(">>Archivo creado en la dirección:  %s", pathCompleta);
        }

        DiscoAceptado=0;
        DiscoAceptadoname=0;
        DiscoAceptadopath=0;
    }
    else
        printf(">> ERROR: Hace falta algun campo obligatorio.\n");

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

//____________________________________________________________________________________________________________________CREAR PARTICION
void  CrearParticion(int tam, char unidad[1], char path[100], char name[100], char type[1], char fit[2]){
    printf(">>Dato para crear partición. \n");
    printf("size -> %i \n", tam);
    printf("unit -> %s \n", unidad);
    printf("path -> %s \n", path);
    printf("name -> %s \n", name);
    printf("type -> %s \n", type);
    printf("fit -> %s \n", fit);
    printf("*****************************\n");

    if (strncasecmp(unidad, "m",1)==0){
         tam=tam*1024*1024;
        printf(" %d Megabytes\n",tam);
    }else if (strncasecmp(unidad, "b",1)==0){
        printf(" %d Bytes\n",tam);
    }else if (strncasecmp(unidad, "k",1)==0){
        tam=tam*1024;
        printf(" %d kiloBytes\n",tam);
    }else
         printf(">> ERROR:Unidad erronea %s\n",unidad);
}

//*******************************************************************************************************************************
//-----------------------------------------------------------MAIN----------------------------------------------------------------
//*******************************************************************************************************************************

int main()
{
    while (salida==0){
        printf("\n ***Ingresar Comando***\n");
        fgets(Comando,100,stdin);

        char caracter[1]="\n";
        char Resultante[100];
        int i = 0;
        int j = 0;
        while (Comando[i] != '\0')
        {
            if (caracter[0] != Comando[i])
            {
                Resultante[j] = Comando[i];
                j++;
            }
            i++;
        }
        Resultante[j] = '\0';

        strcpy(Comando, Resultante);

            automata(Comando);
}
    return 0;
}

