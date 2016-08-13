#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>
char** str_split(char* a_str, const char a_delim);
void automata(char Comando[300]);
char SinComilla (char frase[100]);
void CrearDisco(int tam, char unidad[1], char path[100], char name[100]);
void EliminarDisco(char path[100]);
void  CrearParticion(int tam, char unidad[1], char path[100], char name[100], char type[1], char fit[2]);
void  EliminarParticion(char path[100], char name[100], char type[1]);
void ejecutarexec(char path[50]);
void inicializarmatriz();
void  Umount(char id[5]);
void  MostrarMontura();
void  MontarParticion(char path[100], char name[20]);

//*******************************************************************************************************************************
//--------------------------------------------------------STRUCTS Y VARIBLES------------------------------------------------------
//*******************************************************************************************************************************

typedef struct ComandoMKdisk{
    int size;
    char unit[2];
    char path[100];
    char name[20];
}ComandoMKdisk;

typedef struct ComandoMount{
    char path[100];
    char name[20];
}ComandoMount;

typedef struct Mount{
    char nombre[50];
    int pos;
}Monturas;

typedef struct ComandoFdisk{
    int size;
    char unit[2];
    char path[100];
    char name[16];
    char type[2];
    char fit[3];
    char del[4];
    int add;
}ComandoFdisk;

typedef struct FdiskParticion{
    char pStatus;
    char pType;
    char pFit;
    unsigned int pStart;
    unsigned int pSize;
    char pName[20];
    int pId;
}FdiskParticion;

typedef struct MasterBootRecord{
    unsigned int mbrSize;
    char mbrTimeCreation[25];
    int mbrSignature;
    FdiskParticion mbrPartition[4];
}MasterBootRecord;

typedef struct ExtendedBootRecord{
    char ebrStatus;
    char ebrFit;
    unsigned int ebrStart;
    unsigned int ebrSize;
    char ebrName[20];
    unsigned int ebrNext;
}ExtendedBootRecord;

typedef struct Montar{
    char pathDisco[150];
    char nameParticion[16];
    char id[4];
    bool disponible;
}Montar;

typedef struct Reporte{
    char path[50];
    char name[20];
    char id[5];
}Reporte;



char Comando[1000];
int salida=0;

char** Token;
char** TokenC;
char * com;
//permisos para mkdisk
int DiscoAceptado = 0;
int DiscoAceptadoname=0;
int DiscoAceptadopath=0;
//permisos para rmdisk
int DelDisco = 0;
//permisos para ComandoFdisk
int FperAdd=0;
int FperPath=0;
int FperName=0;
int FperDel=0;
int FperFit=0;
int FperSize=0;
//permisos para exec
int ex =0;
//permisos para mount
int mountname=0;
int mountpath=0;
//permisos para reporte
int reportpath=0;
int reportname=0;
int reportid=0;
FILE *file;

//matriz para montar
Montar montar[26][150];

void inicializarmatriz(){
    int i,j;
    for(i=0;i<26;i++){
        for(j=0;j<149;j++){
            montar[i][j].disponible=true;
        }
    }
}

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
//________________________________________________________________________________________________________________________EXEC
            if(strcasecmp((*(Token + i)), "exec") ==0 ){
                char path[50];
                int jadd;
                for (jadd = (i+1); *(Token + jadd); jadd++){
                if (strncasecmp((*(Token + jadd)), "-path", 5) == 0){//****PATH
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
                    strcpy(path, Resultante);
                    ex=1;
                }
                }
                if(ex==1){
                    ejecutarexec(path);
                    printf("<<SALIO DE EXEC>>>\n");
                }else
                    printf(">>ERROR: Debe ingresar una dirección valida.\n");
            }

//________________________________________________________________________________________________________________________MK-DISK
            if(strcasecmp((*(Token + i)), "mkdisk") ==0 ){
                ComandoMKdisk * MKDISK = malloc(sizeof(ComandoMKdisk));
                char *eme = "m";
                strcpy(MKDISK->unit,eme);

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
                }
                     /* printf("size -> %d \n", MKDISK->size);
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

//________________________________________________________________________________________________________________________MOUNT
            if(strcasecmp((*(Token + i)), "mount") ==0 ){
                ComandoMount * MOUNT = malloc(sizeof(ComandoMount));
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
                        strcpy(MOUNT->path, Resultante);
                        mountname=1;
                    }else if (strncasecmp((*(Token + jadd)), "-name", 4) == 0){//****NAME
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
                        strcpy(MOUNT->name, Resultante);
                        mountpath=1;
                    }
                }
                if(mountname==1 && mountpath==1){
                    MontarParticion(MOUNT->path,MOUNT->name);
                    mountpath=0;
                    mountname=0;
                }else if(mountname==0 && mountpath==0){
                    MostrarMontura();
                }else
                    printf(">>ERROR: Hace falta algun campo obligatorio");
            }

//________________________________________________________________________________________________________________________REPORTES
            if(strcasecmp((*(Token + i)), "rep") ==0 ){
                Reporte * report = malloc(sizeof(Reporte));
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
                        strcpy(report->path, Resultante);
                        reportname=1;
                    }else if (strncasecmp((*(Token + jadd)), "-name", 4) == 0){//****NAME
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
                        strcpy(report->name, Resultante);
                        reportpath=1;
                    }else if (strncasecmp((*(Token + jadd)), "-id", 3) == 0){//****PATH
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
                        strcpy(report->id, Resultante);
                        reportid=1;
                    }
                }
                if(reportname==1 && reportpath==1 && reportid==1){
                    CrearReporte(report->path,report->name,report->id);
                    reportpath=0;
                    reportid=0;
                    reportname=0;
                }else
                    printf(">>ERROR: Hace falta algun campo obligatorio");
            }
//________________________________________________________________________________________________________________________F-DISK
            if(strcasecmp((*(Token + i)), "fdisk") ==0 ){
                ComandoFdisk * FDISK = malloc(sizeof(ComandoFdisk));
                char *ca = "m";
                strcpy(FDISK->unit,ca);
                char *wf="WF";
                strcpy(FDISK->fit,wf);
                char *pe ="P";
                strcpy(FDISK->type,pe);
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
                        strcpy(FDISK->path, Resultante);
                        FperPath=1;
                    }else if (strncasecmp((*(Token + jadd)), "+type", 5) == 0){//****TYPE
                        char * com;
                        com = strtok((*(Token + jadd)),"::");
                        com = strtok(NULL, "::");
                        strcpy(FDISK->type,com);
                             printf("type -> %s \n", FDISK->type);
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
                       // int tam=*(Token+jadd);
                        int tama=Getint(*(Token+jadd));
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
                    FperDel=0;
                    EliminarParticion(FDISK->path,FDISK->name,FDISK->del);
                }else if(FperPath==1 && FperName==1 && FperSize==1){//~~~~~~~~~~CREAR PARTICION
                    CrearParticion(FDISK->size, FDISK->unit,FDISK->path,FDISK->name,FDISK->type,FDISK->fit);
                    printf("<<Salio de crear particion>> \n");
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
    printf("%s \n", name);
    printf("*****************\n\n\n");

    MasterBootRecord mbr;
    mbr.mbrSize=tam;
    timer_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char output[128];
    strftime(output,128,"%d/%m/%y %H:%M:%S",tlocal);
    printf(">>AVISO: Hora creación: %s\n",output);
    int id=path;
    int signature=rand() % 100;

    strcpy(mbr.mbrTimeCreation,output);
    int size=0;

    //verificar la unidad
    if (DiscoAceptado==1 && DiscoAceptadoname==1 && DiscoAceptadopath==1)/*uno es aceptado*/{
        if (strncasecmp(unidad, "m",1)==0){
            size=tam*(1024*1024);
            printf(">>AVISO: Tamaño de disco: %d Mb\n\n",size);
        }
        else if (strncasecmp(unidad, "K",1)==0){
            size=tam*1024;
            printf(">>AVISO: Tamaño de disco: %d Kb\n\n",size);
        }
        else
            printf(">> ERROR:Unidad erronea %s\n",unidad );


        //comprobar carpeta y la crea por si no existe
        file = fopen (path, "r+b");
        if (file==NULL){
            char sys[10] = "mkdir -p ";
            strcat(sys,path);
            system(sys);
            printf(">>Carpeta Creada en la dirección:  %s \n", path);
        }


        //crear el archivo binario
        char pathCompleta[100];
        strcpy(pathCompleta,strcat(path,name));
        file = fopen (pathCompleta, "w+b" );
        if (file!=NULL) {
            char SizeArch[3]="\0";
          // memset(SizeArch, 0, sizeof SizeArch);
           int i;
           for(i=0;i<size;i++){
               fseek(file,i,SEEK_SET);
               fwrite(SizeArch,1,sizeof(SizeArch),file);

           }

            //escribo en el mbr
            strcpy(mbr.mbrTimeCreation,output);
            mbr.mbrSize=size;
            mbr.mbrSignature=signature;
            //escribo las particiones

            for(i=0;i<4;i++){
            mbr.mbrPartition[i].pStart=0;
            mbr.mbrPartition[i].pStatus='\0';
            mbr.mbrPartition[i].pFit='\0';
            mbr.mbrPartition[i].pType='\0';
            mbr.mbrPartition[i].pSize=0;
            mbr.mbrPartition[i].pId=0;
            int x;
            for(x=0;x<15;x++){mbr.mbrPartition[i].pName[x]='\0';}
            }
            fseek(file,0,SEEK_SET);
            fwrite(&mbr,sizeof(MasterBootRecord),1,file);
            fclose ( file );
            printf(">>Archivo creado en la dirección:  %s", pathCompleta);
        }else{
            printf(">>ERROR: Fallo al crear disco en ruta:  %s", pathCompleta);
        }

        //verifica si escribio correcto el mbr
        file=fopen(pathCompleta,"r+");
        MasterBootRecord temporal;
        fread(&temporal,sizeof(MasterBootRecord),1,file);
        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n>>Signature %d \n>>tamaño %d \n>>Fecha %s",temporal.mbrSignature,temporal.mbrSize, temporal.mbrTimeCreation);
        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        fclose(file);


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
        while (path[i] != '\0'){
            if (caracter[0] != path[i])
            {
                Resultante[j] = path[i];
                j++;
            }
            i++;
        }
        Resultante[j] = '\0';
        strcpy(path, Resultante);


        file=fopen(path,"r+b");
        if(file!=NULL){
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
    printf("\n*****************************\n");
    printf(">>Dato para crear partición. \n");
    printf("size -> %i \n", tam);
    printf("unit -> %s \n", unidad);
    printf("path -> %s \n", path);
    printf("name -> %s \n", name);
    printf("type -> %s \n", type);
    printf("fit -> %s \n", fit);
    printf("*****************************\n\n");

    int equalname = 0;
    int primarias=0;
    int extendidas=0;
    int logicas = 0;
    int AuxPrimarias=0;
    int AuxExtendidas=0;
    int TamAceptado =1;

    //verifico la unidad del tamaño
    int size=0;
    if (strncasecmp(unidad, "m",1)==0){
        size=tam*1024*1024;
        printf(" %d Megabytes\n",size);
    }else if (strncasecmp(unidad, "b",1)==0){
        size=tam;
        printf(" %d Bytes\n",size);
    }else if (strncasecmp(unidad, "k",1)==0){
        size=tam*1024;
        printf(" %d kiloBytes\n",size);
    }else
        printf(">> ERROR:Unidad erronea %s\n",unidad);

    if(size<(2*1024*1024)){
        printf(">>ERROR: Tamaño minimo para crear partición es de 2MB.\n");
        TamAceptado=0;
    }


    file = fopen (path, "rb+" );
    if (file!=NULL) {
        //aca empiezan validaciones sobre el disco
    }else
        printf(">>ERROR: El disco seleccionado no existe \n");


    FILE* file2= fopen(path, "rb+");

    if(file2==NULL){
        printf(">>ERROR: Disco inexistente. %s\n",path);
    }else/*el disco existe*/{
        MasterBootRecord LecturaMBR;
        ExtendedBootRecord ebr;
        fread(&LecturaMBR, sizeof(MasterBootRecord), 1, file2);

        printf("\n\n*****************************\n");
        printf(">>AVISO::Datos del Disco\n");
        printf("Signature %i \n",LecturaMBR.mbrSignature);
        printf("Fecha Creación %s \n", LecturaMBR.mbrTimeCreation);
        printf("Tamaño %i\n",LecturaMBR.mbrSize);
        printf("\n\n*****************************\n\n");


        int z=0;
        for(z=0;z<4;z++){
            if(strcasecmp(LecturaMBR.mbrPartition[z].pName,name)==0){
                equalname=1;
            }
        }


        printf(">>AVISO::Datos de Particiones\n");
        //obtener las particiones primarias

                for(z=0;z<4;z++){
                    int nom=0, nombre=0;
                    while(name[nombre]!=NULL){
                        if(LecturaMBR.mbrPartition[z].pName[nom]==name[nom]){
                            nombre++;
                        }
                        nom++;
                    }
                    if(nombre==nom && LecturaMBR.mbrPartition[z].pStatus!='0'){
                        equalname=1;
                        printf("\nERROR: Coincidencia de nombres.  %i \n\n",equalname);
                    }

                    if(LecturaMBR.mbrPartition[z].pType=='p'||LecturaMBR.mbrPartition[z].pType=='P'){
                    }
                    if(LecturaMBR.mbrPartition[z].pType=='e'||LecturaMBR.mbrPartition[z].pType=='E'){
                        extendidas++;
                    }
                }//fin de Recorrido de PArticiones
       /* //obtener las particiones primarias
        for(z=0;z<4;z++){
            if(strcasecmp(LecturaMBR.mbrPartition[z].pName,name)==0 && LecturaMBR.mbrPartition[z].pStatus!='0'){
                equalname=1;
                printf("\nERROR: Coincidencia de nombres.  %i \n\n",equalname);
            }

            if(strcasecmp(LecturaMBR.mbrPartition[z].pType,'p')==0){
                primarias++;
            }
            if(strcasecmp(LecturaMBR.mbrPartition[z].pType,'e')==0){
                extendidas++;
            }
        }//fin de Recorrido de Particiones*/




        if(TamAceptado==1){//Si el tamaño de la particion es =>2MB
            int sizeRestante=0;
            int espaciolibre =0;
            int d;
            for (d=0;d<=3;d++) {

                sizeRestante+=LecturaMBR.mbrPartition[d].pSize;
            }
            espaciolibre = LecturaMBR.mbrSize-sizeRestante-sizeof(MasterBootRecord);
            printf("Tamaño disponible para usar %d\n",espaciolibre);
            int start;

            //conteo de particiones
            for (d=0;d<=3;d++) {
                char *auxType= (LecturaMBR.mbrPartition[d].pType);
                printf("%d\n",auxType);
                if (auxType=='p'){
                    primarias++;
                }else if (auxType=='e') {
                    extendidas++;
                } else if (auxType=='l') {
                    logicas=1;
                }
            }
            printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nParticiones Primarias %d\n Particiones Extendidas %d\n Particiones Logicas %d\n",primarias,extendidas,logicas);

            int asignado = 0;

//__________________________________________________________________________________________________________________________PARTICIÓN PRIMARIA
            if (strncasecmp(type, "p",1)==0){
                printf("<<ENTRO A PARTICION PRIMARIA>>\n\n");
                if (primarias==4){
                    printf(">>ERROR: Ya hay cuatro particiones primarias creadas, no puede crear más.");
                }else{
                    if(size<=espaciolibre){//verifica si el espacio disponible es suficiente
                        int p;
                        for ( p=0;p<4;p++) {
                            //busca las particiones
                            int sizze=LecturaMBR.mbrPartition[p].pSize;
                            if (sizze==0){

                                LecturaMBR.mbrPartition[p].pStatus='1';
                                LecturaMBR.mbrPartition[p].pType=*type;
                                LecturaMBR.mbrPartition[p].pFit=*fit;
                                LecturaMBR.mbrPartition[p].pSize=size;
                                strcpy(LecturaMBR.mbrPartition[p].pName,name);
                                int u;
                                for ( u=0;u<4;u++){
                                    if (LecturaMBR.mbrPartition[u].pSize>0){
                                        asignado=LecturaMBR.mbrPartition[u].pSize+asignado;
                                    }else {
                                        printf("Particion vacia\n");}
                                }
                                start=sizeof(MasterBootRecord)+asignado +1;
                                LecturaMBR.mbrPartition[p].pStart=start;
                                printf(">>AVISO: Partición creada en la rut: %s\n",path);
                                break;
                            }else
                                printf(">>ERROR: No hay espacio disponible para crear partición.\n");
                        }
                        fseek(file2,0,SEEK_SET);
                        fwrite(&LecturaMBR,sizeof(MasterBootRecord),1,file2);
                        fclose(file2);
                    }
                }

            }//termina particion primaria



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PARTICIÓN EXTENDIDA
            if (strncasecmp(type, "e",1)==0){
                printf("<<ENTRO A PARTICION EXTENDIDA>>\n\n");
                if (extendidas==1){
                    printf(">>ERROR: Ya existe una partición extendida, no puede crear más.");
                }else if (primarias==4){
                    printf(">>ERROR: Ya existen cuatro particiones creadas, no puede crear más.");

                }else {
                    if(size<=espaciolibre){//verifica si el espacio disponible es suficiente
                        printf("////////////////////////si hay espacio disponible %d\n", extendidas);
                        int p;
                        for ( p=0;p<4;p++) {
                            //busca las particiones
                            int sizze=LecturaMBR.mbrPartition[p].pSize;
                            if (sizze==0){

                                LecturaMBR.mbrPartition[p].pStatus='1';
                                LecturaMBR.mbrPartition[p].pType=*type;
                                LecturaMBR.mbrPartition[p].pFit=*fit;
                                LecturaMBR.mbrPartition[p].pSize=size;
                                strcpy(LecturaMBR.mbrPartition[p].pName,name);
                                int u;
                                for ( u=0;u<4;u++){
                                    if (LecturaMBR.mbrPartition[u].pSize>0){
                                        asignado=LecturaMBR.mbrPartition[u].pSize+asignado;
                                    }else {
                                        printf("Particion vacia \n");}
                                }
                                start=sizeof(MasterBootRecord)+asignado +1;
                                LecturaMBR.mbrPartition[p].pStart=start;

                                ebr.ebrFit='\0';
                                int x;
                                for(x=0;x<15;x++){ebr.ebrName[x]='\0';}
                                ebr.ebrNext=-1;
                                ebr.ebrSize=0;
                                ebr.ebrStart=start+sizeof(MasterBootRecord)+1;
                                ebr.ebrStart=1;


                                printf(">>AVISO: Partición creada en la rut: %s\n",path);
                                break;
                            }else
                                printf(">>ERROR: No hay espacio disponible para crear partición.\n");
                        }

                        fseek(file2,0,SEEK_SET);
                        fwrite(&LecturaMBR,sizeof(MasterBootRecord),1,file2);
                        fseek(file2,sizeof(MasterBootRecord)+1,SEEK_SET);
                        fwrite(&ebr,sizeof(ExtendedBootRecord),1,file2);
                        fclose(file2);
                    }
                }

            }//termina particion Extendida


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PARTICIÓN LOGICA
            if (strncasecmp(type, "l",1)==0){
                printf("<<ENTRO A PARTICION LOGICA>>\n\n");
                if (extendidas==0){
                    printf(">>ERROR: NO existe una partición extendida, no puede crear partición lógica.");
                }else {
                    if(size<=espaciolibre){//verifica si el espacio disponible es suficiente
                        printf("////////////////////////si hay espacio disponible %d\n", logicas);

                        FILE *file3;
                        file3=fopen(path,"rb+");
                        ExtendedBootRecord lecturaEBR;

                        printf("\n >>Datos del EBR. \n");
                        printf("Tamaño %d \n",lecturaEBR.ebrSize);
                        printf("Tamaño EBR %d\n",sizeof(MasterBootRecord));

                        lecturaEBR.ebrStart=1;
                        lecturaEBR.ebrFit=*type;
                        lecturaEBR.ebrStart=sizeof(MasterBootRecord)+1;
                        lecturaEBR.ebrSize=size;
                        lecturaEBR.ebrNext=sizeof(MasterBootRecord)+sizeof(ExtendedBootRecord)+lecturaEBR.ebrSize-1;
                        strcpy(lecturaEBR.ebrName,name);
                        fseek(file3,sizeof(MasterBootRecord)+sizeof(ExtendedBootRecord)+lecturaEBR.ebrSize+1,SEEK_SET);
                        fwrite(&lecturaEBR,sizeof(MasterBootRecord),1,file3);
                       fclose(file3);
                    }
                }

            }//termina particion Extendida
        }



    }

}

//____________________________________________________________________________________________________________________ELIMINAR PARTICION
void  EliminarParticion(char path[100], char name[100], char del[4]){
    printf("\n*****************************\n");
    printf(">>Dato para eliminar partición. \n");
    printf("path -> %s \n", path);
    printf("name -> %s \n", name);
    printf("eliminacion -> %s \n", del);
    printf("*****************************\n\n");

   /* char pathCompleta[100];
    strcpy(pathCompleta,strcat(path,name));*/

    int elimina=0;
    FILE *delfile=fopen(path,"rb+"); // ABRO PARA LECTURA Y ESCRITURA

    MasterBootRecord delmbr;

    fread(&delmbr,sizeof(MasterBootRecord),1,delfile);
    printf(">>AVISO: datos del MBR \n");
    printf("Signature %d \n",delmbr.mbrSignature);
    printf("Tamaño Total Del disco %d \n",delmbr.mbrSize);
    printf("Tamaño MBr %d\n\n",sizeof(MasterBootRecord));
int encontro=0;
char compararname[100];

    if (strncasecmp(del, "full",4)==0){
         printf("\n >>ATENCION: Presione 3 para eliminar particion.\n");
        scanf( "%d", &elimina );

        if (elimina==3){
            int p;
            for (p=0;p<4;p++) {
                strcpy(compararname, delmbr.mbrPartition[p].pName);
                printf("%s \n", compararname);
                if (strcasecmp(compararname,name)==0){
                    int d;
                    for (d=0;d<4;d++) {
                        delmbr.mbrPartition[p].pStatus='\0';
                        delmbr.mbrPartition[p].pType='\0';
                        delmbr.mbrPartition[p].pFit='\0';
                        delmbr.mbrPartition[p].pStart=0;
                        delmbr.mbrPartition[p].pSize=0;
                        int x;
                        for(x=0;x<15;x++){
                            delmbr.mbrPartition[p].pName[x]="\0";
                        }
                    }
                    encontro=1;

                }else{
                    encontro=0;
                }
            }
            if (encontro==0)
                printf(">>ERROR: Particion no encontrada\n");
            else if (encontro==1)
                printf(">>AVISO: Particion eliminada");

        }else{
            printf(">>AVISO: Particion no eliminada.\n");
        }


    }else if (strncasecmp(del, "fast",4)==0){
        printf("\n >>ATENCION: Presione 3 para eliminar particion. \n");
        scanf( "%d", &elimina );
        if (elimina==3){
           int p;
            for (p=0;p<4;p++) {
                strcpy(compararname, delmbr.mbrPartition[p].pName);
                printf("%s \n", compararname);
                if (strcasecmp(compararname,name)==0){
                    int d;
                    for ( d=0;d<=3;d++) {
                        delmbr.mbrPartition[p].pSize=0;
                    }
                    encontro=1;

                }else{
                    encontro = 0;
                }
            }

            if (encontro==0)
                printf(">>ERROR: Particion no encontrada\n");
            else if (encontro==1)
                printf(">>AVISO: Particion eliminada");

        }else
            printf(">>AVISO: Particion no eliminada.\n");


    }else
        printf(">>ERROR: No coincide parametro de eliminacion\n");
    //termina if de del

    fseek(delfile,0,SEEK_SET);
    fwrite(&delmbr,sizeof(MasterBootRecord),1,delfile);
    close(delfile);
}

//____________________________________________________________________________________________________________________MONTAR PARTICION
void  MontarParticion(char path[100], char name[20]){
    printf("\n*****************************\n");
    printf(">>Dato para crear Montura. \n");
    printf("path -> %s \n", path);
    printf("name -> %s \n", name);
    printf("*****************************\n\n");

    int equalname=0;
    FILE *mountfile;
    mountfile= fopen(path,"rb+");
    char idAux[5];
    char idaux[2];
    char abc[27];
    char num[2];
    int filau, columnau;
    strcpy(abc,"abcdefghijklmnñopqrstuvwxyz");
    strcpy(idAux,"vd");
    MasterBootRecord LecturaMBR;
    fread(&LecturaMBR,sizeof(MasterBootRecord),1,mountfile);
    if(mountfile!=NULL){
        int z=0;
        for(z=0;z<4;z++){
            if(strcasecmp(LecturaMBR.mbrPartition[z].pName,name)==0){
                equalname=1;
            }
        }//termina recorrido de mbr

        if (equalname==0){
            printf(">>ERROR: Particion no existe");
        }else {
            printf("<<ENTRO A MONTAR>>\n");
            int fila,columna;
            for (fila=1; fila<28;fila++){
                for(columna=1;columna<151;columna++){
                    if(strcasecmp(montar[fila][columna].pathDisco,path)==0){
                        printf("ya ha montado el disco\n");
                        filau=fila;
                        fila=27;
                        break;
                    }
                    if(strcasecmp(montar[fila][columna].nameParticion,name)==0){
                        printf("ya existe la particion");
                        fila=27;
                        break;
                    }else{
                        filau=fila;
                        columnau=columna;
                      fila=27;
                      break;
                   }
               }//for columna
           }//for fila
             strcpy(montar[filau][columnau].nameParticion,name);
             strcpy(montar[filau][columnau].pathDisco,path);
             idAux[2] = abc[filau-1];
             columnau=columnau+1;
             sprintf(num,"%d",columnau);
             strcat(idAux,num);
             printf("%s", idAux);
             strcpy(montar[filau][columnau].id,idAux);
             montar[filau][columnau].disponible=false;
           printf(">>AVISO: Partición montada con Exito. \n");

        }

    }else{//el archivo existe
        printf(">>ERROR: Disco No existe.\n");
    }




}

//____________________________________________________________________________________________________________________MOSTRAR MONTURA
void  MostrarMontura(){
    int fila, columna;
    printf(">>AVISO: Particiones montadas:\n");
    for (fila=0; fila<26;fila++){
        for(columna=0;columna<149;columna++){
            if(montar[fila][columna].disponible==false){

                printf(">>DISCO: %s \n", montar[fila][columna].pathDisco);
                printf(">>Partición: %s \n", montar[fila][columna].nameParticion);
                printf(">>ID: %s \n", montar[fila][columna].id);
                printf("___________________________________________________\n");
            }
        }
    }
}

//____________________________________________________________________________________________________________________QUITAR MONTURA
void  Umount(char id[5]){

}

//____________________________________________________________________________________________________________________CREAR REPORTE
void  CrearReporte(char path[50], char name[20], char id[5]){
    printf("\n*****************************\n");
    printf(">>Dato para crear Reporte. \n");
    printf("Path -> %s \n", path);
    printf("Name -> %s \n", name);
    printf("ID -> %s \n", id);
    printf("*****************************\n\n");

    char pathdisk[50];
    strcpy(pathdisk,"\0");
    int fila, columna;
    fila=obtenerValor(id[2]);
    columna=(int)id[3]-48;
    printf("%d ----\n",columna);


    if(montar[fila][columna].disponible==true){
        printf(">>ERROR: No existe partición montada en esa coordenada.\n");
    }else{
        strcpy(pathdisk,montar[fila][columna].pathDisco);
        printf("%s \n",pathdisk);
    }

    /*char sys[10] = "mkdir -p ";
    strcat(sys,path);
    system(sys);

    strcpy(sys,"rmdir ");
    strcat(sys,path);
    system(sys);*/
    printf(">>Carpeta Creada en la dirección:  %s \n", path);


    if(strncasecmp(name, "mbr", 3) ==0 ){
        char nameDisk[75];
        FILE *GrafoReporte = fopen("/home/mitchel/Escritorio/prueba/Reportes.dot","w+");

        fprintf(GrafoReporte,"digraph grafo{\n");
        fprintf(GrafoReporte,"node[shape = box];\n");
        char MH[150];
        strcpy(MH,pathdisk);
        char* salto = strtok(MH,"/");
        while(salto != NULL){
            strcpy(nameDisk,salto);
            salto = strtok(NULL,"/");
        }



        FILE *LecturaMBR = fopen(pathdisk,"rb+");
        MasterBootRecord lecturambr;
        fseek(LecturaMBR,0,SEEK_SET);
        fread(&lecturambr,sizeof(MasterBootRecord),1,LecturaMBR);
        fprintf(GrafoReporte,"\t  subgraph clusterPila1{ \n");
        fprintf(GrafoReporte,"\t  label = \"REPORTE  \"; \n");
        fprintf(GrafoReporte,"\t  node [shape = box] color = orange style=filled; \n");
        fprintf(GrafoReporte,"\t  subgraph clusterLibrera{ \n");
        fprintf(GrafoReporte,"label = \"%s\";\n",nameDisk);
        fprintf(GrafoReporte,"labelloc = \"t\";\n");


        fprintf(GrafoReporte,"\"Record\" [label = <<table border = \"1\" cellspacing = \"2\">\n");
        fprintf(GrafoReporte,"<tr><td colspan = \"2\"><b> MBR: %s </b></td></tr>\n",nameDisk);
        fprintf(GrafoReporte,"<tr><td><b>ATRIBUTO</b></td><td><b> VALOR </b></td></tr>\n");
        fprintf(GrafoReporte,"<tr><td><b>mbrSIZE</b></td><td> %d </td></tr>\n",lecturambr.mbrSize);
        fprintf(GrafoReporte,"<tr><td><b>mbrFECHA_CREACION</b></td><td> %s </td></tr>\n",lecturambr.mbrTimeCreation);
        fprintf(GrafoReporte,"<tr><td><b>mbrSIGNATURE</b></td><td> %d </td></tr>\n",lecturambr.mbrSignature);
        int i;
        for( i = 0; i < 4; i++){
            if(lecturambr.mbrPartition[i].pStatus==1){
                fprintf(GrafoReporte,"<tr><td><b> STATUS %d</b></td><td>%c</td></tr>\n",(i+1),lecturambr.mbrPartition[i].pStatus);
                fprintf(GrafoReporte,"<tr><td><b> TYPE %d</b></td><td><b>%c</b></td></tr>\n",(i+1),(toupper(lecturambr.mbrPartition[i].pType)));
                fprintf(GrafoReporte,"<tr><td><b> FIT %d</b></td><td><b>%c</b></td></tr>\n",(i+1),(toupper(lecturambr.mbrPartition[i].pFit)));
                fprintf(GrafoReporte,"<tr><td><b> START %d</b></td><td>%d</td></tr>\n",(i+1),lecturambr.mbrPartition[i].pStart);
                fprintf(GrafoReporte,"<tr><td><b> SIZE %d</b></td><td>%d</td></tr>\n",(i+1),lecturambr.mbrPartition[i].pSize);
                fprintf(GrafoReporte,"<tr><td><b> NAME %d</b></td><td>%s</td></tr>\n",(i+1),lecturambr.mbrPartition[i].pName);
            }
        }
        fprintf(GrafoReporte,"</table>>];\n");
        fprintf(GrafoReporte,"\t\t  label = \" <<MBR>> \"; \n");
        fprintf(GrafoReporte,"\t  style = filled; \n");
        fprintf(GrafoReporte,"\t  color = White; \n");
        fprintf(GrafoReporte,"\t  } \n");
        fprintf(GrafoReporte,"}");
        fprintf(GrafoReporte,"}");
        fflush(GrafoReporte);
        fclose(LecturaMBR);
        fclose(GrafoReporte);
        printf("\n\nJLFKSJDFLJSALFJLSAKFJLKSAJFLKSAJFLSAJFLKSAJFLKSJALFSALF\n\n");


        char sys[600];
        system("dot -Tpng /home/mitchel/Escritorio/prueba/Reportes.dot -o /home/mitchel/Escritorio/Reportes/Reportes.png");
        strcpy(sys,"");strcat(sys,"viewnior \"");
        strcat(sys,"/home/mitchel/Escritorio/Reportes/Reportes.png");
        strcat(sys,"\"&");
        system(sys);
        //  system(viewnior"/home/mitchel/Escritorio/Reportes/Reportes.png\"&");

    }else if(strncasecmp(name, "disk", 4) ==0 ){
        FILE * reader = fopen(pathdisk, "rb+");
           fseek(reader, 0, SEEK_SET);

           FILE * GrafoReporte = fopen("/home/mitchel/Escritorio/prueba/Reportes.dot", "wb+");
           fseek(GrafoReporte, 0, SEEK_SET);

           MasterBootRecord aux_mbr;

           fread(&aux_mbr, sizeof(MasterBootRecord), 1, reader);

           char flag_l = '0';

           fseek(reader, sizeof(MasterBootRecord) + 1, SEEK_SET);
           ExtendedBootRecord aux_ebr;

           while (fread(&aux_ebr, sizeof(ExtendedBootRecord),1,reader)){
               if (aux_ebr.ebrStart == '1'){
                   flag_l = '1';
                   break;
               }
           }


           fprintf(GrafoReporte,"\t  subgraph clusterPila1{ \n");
           fprintf(GrafoReporte,"\t  label = \"REPORTE  \"; \n");
           fprintf(GrafoReporte,"\t  node [shape = box] color = orange style=filled; \n");
           fprintf(GrafoReporte,"\t  subgraph clusterLibrera{ \n");
           fprintf(GrafoReporte,"label = \"%s\";\n",pathdisk);
           fprintf(GrafoReporte,"digraph{\nrankdir = TB;\nsubgraph clusterEncabezado{\nnode[shape = record, color = orange, height = 0.5];\nlabel=\"");
           fprintf(GrafoReporte, "%s\";\n", pathdisk);
           int contadorp=0;
int i;
           for (i = 0; i<4 ; i++){
               if (aux_mbr.mbrPartition[i].pStatus == '0')
               {
                   fprintf(GrafoReporte,"node%d[label = \" Libre \"];\n", contadorp);
                   contadorp++;
               }
               else if (aux_mbr.mbrPartition[i].pStatus == '1'){

                   if (aux_mbr.mbrPartition[i].pType == 'p' || aux_mbr.mbrPartition[i].pType == 'P'){
                       fprintf(GrafoReporte,"node%d[label = \"  %s \"];\n",contadorp, aux_mbr.mbrPartition[i].pName);
                       contadorp++;
                   }
                   else if (aux_mbr.mbrPartition[i].pType == 'e' || aux_mbr.mbrPartition[i].pType == 'E'){
                       if (flag_l == '1'){

                           fprintf(GrafoReporte, "nodext[label=\"{%s | {", aux_mbr.mbrPartition[i].pName);

                           fseek(reader, sizeof(MasterBootRecord) + 1, SEEK_SET);
                           while(fread(&aux_ebr,sizeof(ExtendedBootRecord),1,reader)) {
                               if (aux_ebr.ebrStatus == '1'){
                                   if (aux_ebr.ebrNext!=-1){
                                       fprintf(GrafoReporte,"%s | ", aux_ebr.ebrName);
                                   }
                                   else{
                                       fprintf(GrafoReporte,"%s}}\"];\n", aux_ebr.ebrName);
                                       break;
                                   }
                               }
                           }
                       }
                       else{
                           fprintf(GrafoReporte,"nodext[label=\"%s\"];\n", aux_mbr.mbrPartition[i].pName);
                       }
                   }
               }
           }
           fprintf(GrafoReporte,"</table>>];\n");
           fprintf(GrafoReporte,"\t\t  label = \" <<MBR>> \"; \n");
           fprintf(GrafoReporte,"\t  style = filled; \n");
           fprintf(GrafoReporte,"\t  color = orange; \n");
           fprintf(GrafoReporte,"\t  } \n");
fprintf(GrafoReporte,"\t  } \n");
           fclose(GrafoReporte);

char sys[100];
           system("dot -Tpng /home/mitchel/Escritorio/prueba/Reportes.dot -o /home/mitchel/Escritorio/Reportes/Reportes.png");
           strcpy(sys,"");strcat(sys,"viewnior \""); strcat(sys,"/home/mitchel/Escritorio/Reportes/Reportes.png");
           strcat(sys,"\"&"); system(sys);

           //INVOCANDO GRAPHVIZ
          // system("dot -Tjpg /home/mitchel/Documentos/Discos/ReportemBR.txt -o /home/jjenano/Documentos/Discos/diagrama.jpg");
           //ABRIENDO LA IMAGEN
         //  system("xdg-open /home/jjenano/Documentos/Discos/diagrama.jpg &");
    }




}

int obtenerValor(char id[1]){
    printf("%c \n", id);
    if (id=='a'){
        return 1;
    }else if (id=='b'){
        return 2;
    }else if (id=='b'){
        return 3;
    }else if (id=='c'){
        return 4;
    }else if (id=='d'){
        return 5;
    }else if (id=='e'){
        return 6;
    }else if (id=='f'){
        return 7;
    }else if (id=='g'){
        return 8;
    }else if (id=='h'){
        return 9;
    }else if (id=='i'){
        return 10;
    }else if (id=='j'){
        return 11;
    }else if (id=='k'){
        return 12;
    }else if (id=='l'){
        return 13;
    }else if (id=='m'){
        return 14;
    }else if (id=='n'){
        return 15;
    }else if (id=='o'){
        return 16;
    }else if (id=='p'){
        return 17;
    }else if (id=='q'){
        return 18;
    }else if (id=='r'){
        return 19;
    }else if (id=='s'){
        return 20;
    }else if (id=='t'){
        return 21;
    }else if (id=='u'){
        return 22;
    }else if (id=='v'){
        return 23;
    }else if (id=='w'){
        return 24;
    }else if (id=='x'){
        return 25;
    }else if (id=='y'){
        return 26;
    }else if (id=='z'){
        return 27;
    }

    return 0;
}

//*******************************************************************************************************************************
//----------------------------------------------------METODO PARA EJECUTAR COMANDOS----------------------------------------------
//*******************************************************************************************************************************

void ejecutarexec(char path[50]){
    printf("<<ENTRO A EXEC>>>\n");
    char comanditos[300];
    FILE *execfile = fopen(path,"r");
    if (execfile!=NULL){

        //aca va el codigo para ejecutar
        while(feof(execfile)==0){
        fgets(comanditos,300,execfile);
            TokenC = str_split(comanditos, '\n');
            if(TokenC){
                int i;
                for (i = 0; *(TokenC + i); i++){
                   // printf("Comando Reconocido=[%s]\n", *(TokenC + i));
                    if(strncasecmp((*(TokenC + i)), "#",1) ==0 ){
                        printf("\n\n\n%s \n", *(TokenC + i));
                    }else{
                        printf("\n\n\n%s \n", *(TokenC + i));
                        automata(*(TokenC+i));
                    }
                }
            }
        }
        strcpy(Token,"");
    }else
        printf(">>ERROR: Dirección de archivo incorrecta.\n");

}

//*******************************************************************************************************************************
//-----------------------------------------------------------MAIN----------------------------------------------------------------
//*******************************************************************************************************************************

int main()
{
    while (salida==0){
        inicializarmatriz();
        printf("\n ***Ingresar Comando***\n");
        fgets(Comando,100,stdin);
    //strcpy(Comando,"mkdisk -size::15 -path::\"/home/mitchel/Escritorio/prueba/\" -name::\"Disco1.dsk\"");
       //strcpy(Comando,"fdisk -size::2 +unit::M -path::\"/home/mitchel/Escritorio/prueba/Disco1.dsk\" -name::\"Particion3\" +type::P");
      //  strcpy(Comando,"mount -path::\"/home/mitchel/Escritorio/Disco1.dsk\" -name::\"part1\"");
       strcpy(Comando,"exec -path::\"/home/mitchel/Escritorio/Comandos.sh\"");
       // strcpy(Comando,"mount -name::\"Particion1P\" -path::\"/home/mitchel/Escritorio/prueba/Disco1.dsk\" ");
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

