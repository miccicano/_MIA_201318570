#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

char** str_split(char* a_str, const char a_delim);
void automata(char Comando[300]);
char SinComilla (char frase[100]);
void CrearDisco(int tam, char unidad[1], char path[100], char name[100]);
void EliminarDisco(char path[100]);
void  CrearParticion(int tam, char unidad[1], char path[100], char name[100], char type[1], char fit[2]);
void  EliminarParticion(char path[100], char name[100], char type[1]);
void ejecutarexec(char path[50]);
//*******************************************************************************************************************************
//--------------------------------------------------------STRUCTS Y VARIBLES------------------------------------------------------
//*******************************************************************************************************************************

typedef struct ComandoMKdisk{
    int size;
    char unit[2];
    char path[100];
    char name[20];
}ComandoMKdisk;

typedef struct ComandoFdisk{
    int size;
    char unit[2];
    char path[100];
    char name[10];
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

typedef struct clear{
    char space[1024];
}clear;

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
FILE *file;


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

           // fwrite(SizeArch,sizeof(SizeArch),1,file);
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

        printf(">>AVISO::Datos de Particiones\n");
        //obtener las particiones primarias
        int z=0;
        for(z=0;z<4;z++){
            int k=0;
            int l=0;
            while(name[k]!=NULL){
                if(LecturaMBR.mbrPartition[z].pName[k]==name[k]){
                    l++;
                }
                k++;
            }
            if(l==k && LecturaMBR.mbrPartition[z].pStatus!='0'){// si las coincidencias son iguales y el status 0
                equalname=1;
                printf("\nERROR: Coincidencia de nombres.  %i \n\n",equalname);
            }

            if(LecturaMBR.mbrPartition[z].pType=='p'||LecturaMBR.mbrPartition[z].pType=='P'){//si el tipo es primaria
                primarias++;
            }
            if(LecturaMBR.mbrPartition[z].pType=='e'||LecturaMBR.mbrPartition[z].pType=='E'){//si el tipo es extendida
                extendidas++;
            }
        }//fin de Recorrido de PArticiones




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

            int cantidadpuesta = 0;

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
                                        cantidadpuesta=LecturaMBR.mbrPartition[u].pSize+cantidadpuesta;
                                    }else {printf("Particion vacia [%d]\n",u);}
                                }
                                start=sizeof(MasterBootRecord)+cantidadpuesta +1;
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

            printf(">>AVISO::Datos de Particiones\n");
            //obtener las particiones primarias
            int z=0;
            for(z=0;z<4;z++){
                int k=0;
                int l=0;
                while(name[k]!=NULL){
                    if(LecturaMBR.mbrPartition[z].pName[k]==name[k]){
                        l++;
                    }
                    k++;
                }
                if(l==k && LecturaMBR.mbrPartition[z].pStatus!='0'){// si las coincidencias son iguales y el status 0
                    equalname=1;
                    printf("\nERROR: Coincidencia de nombres.  %i \n\n",equalname);
                }

                printf("Bit Inicial: %i \n",LecturaMBR.mbrPartition[z].pStart);
                printf("Nombre: %s \n",LecturaMBR.mbrPartition[z].pName);
                printf("Tipo Estado: %c \n",LecturaMBR.mbrPartition[z].pStatus);
                printf("Tipo Particion: %c \n",LecturaMBR.mbrPartition[z].pType);
                printf("---------------------------------------------------------\n");

                if(LecturaMBR.mbrPartition[z].pType=='p'||LecturaMBR.mbrPartition[z].pType=='P'){//si el tipo es primaria
                    primarias++;
                }
                if(LecturaMBR.mbrPartition[z].pType=='e'||LecturaMBR.mbrPartition[z].pType=='E'){//si el tipo es extendida
                    extendidas++;
                }
                /* printf("Tipo De Ajuste: %c \n",LecturaMBR.mbrPartition[z].pFit);
                               printf("Tamaño Particion %i \n", LecturaMBR.mbrPartition[z].pSize);*/
            }//fin de Recorrido de Particiones

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
                                        cantidadpuesta=LecturaMBR.mbrPartition[u].pSize+cantidadpuesta;
                                    }else {printf("Particion vacia [%d]\n",u);}
                                }
                                start=sizeof(MasterBootRecord)+cantidadpuesta +1;
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

            printf(">>AVISO::Datos de Particiones\n");
            //obtener las particiones

            for(z=0;z<4;z++){
                int k=0;
                int l=0;
                while(name[k]!=NULL){
                    if(LecturaMBR.mbrPartition[z].pName[k]==name[k]){
                        l++;
                    }
                    k++;
                }
                if(l==k && LecturaMBR.mbrPartition[z].pStatus!='0'){// si las coincidencias son iguales y el status 0
                    equalname=1;
                    printf("\nERROR: Coincidencia de nombres.  %i \n\n",equalname);
                }

                printf("Bit Inicial: %i \n",LecturaMBR.mbrPartition[z].pStart);
                printf("Nombre: %s \n",LecturaMBR.mbrPartition[z].pName);
                printf("Tipo Estado: %c \n",LecturaMBR.mbrPartition[z].pStatus);
                printf("Tipo Particion: %c \n",LecturaMBR.mbrPartition[z].pType);
                printf("Tipo De Ajuste: %c \n",LecturaMBR.mbrPartition[z].pFit);
                printf("Tamaño Particion %i \n", LecturaMBR.mbrPartition[z].pSize);
                printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

                if(LecturaMBR.mbrPartition[z].pType=='p'||LecturaMBR.mbrPartition[z].pType=='P'){//si el tipo es primaria

                }
                if(LecturaMBR.mbrPartition[z].pType=='e'||LecturaMBR.mbrPartition[z].pType=='E'){//si el tipo es extendida
                    printf("~~~~~~~~~~~~~~~~~~~~~~~Extendidas\n");
                    ExtendedBootRecord mostrar;
                    fseek(file2,LecturaMBR.mbrPartition[z].pStart,SEEK_SET); //escribir el bit ebr inicial
                    fread(&mostrar, sizeof(ExtendedBootRecord), 1, file2);
                    printf("Inicio EBR: %i \n",mostrar.ebrStart);
                    printf("Siguiente ebr: %i \n",mostrar.ebrNext);
                    printf("Estado ebr: %c \n",mostrar.ebrStatus);
                }

            }//fin de Recorrido de Particiones
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PARTICIÓN LOGICA
            if (strncasecmp(type, "l",1)==0){
                printf("<<ENTRO A PARTICION LOGICA>>\n\n");
                if (extendidas==0){
                    printf(">>ERROR: NO existe una partición extendida, no puede crear partición lógica.");
                }else {
                    if(size<=espaciolibre){//verifica si el espacio disponible es suficiente
                        printf("////////////////////////si hay espacio disponible %d\n", logicas);
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
                                        cantidadpuesta=LecturaMBR.mbrPartition[u].pSize+cantidadpuesta;
                                    }else {printf("Particion vacia [%d]\n",u);}
                                }
                                start=sizeof(MasterBootRecord)+cantidadpuesta +1;
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
        }



    }

}

//____________________________________________________________________________________________________________________ELIMINAR PARTICION
void  EliminarParticion(char path[100], char name[100], char type[1]){
    printf("\n*****************************\n");
    printf(">>Dato para eliminar partición. \n");
    printf("path -> %s \n", path);
    printf("name -> %s \n", name);
    printf("type -> %s \n", type);
    printf("*****************************\n\n");

    FILE *delfile = fopen(path,"rb+");
    if(delfile!=NULL){
   //acciones para eliminar particion
    }else
        printf(">>ERROR: No existe ruta del disco.\n");
}

//*******************************************************************************************************************************
//---------------------------------------------------METODOS PARA EJECUTAR COMANDOS----------------------------------------------
//*******************************************************************************************************************************

void ejecutarexec(char path[50]){
    printf("<<ENTRO A EXEC>>>\n");
    char comanditos[300];
    FILE *execfile = fopen(path,"r");
    if (execfile!=NULL){
        while(feof(execfile)==0){
        fgets(comanditos,300,execfile);
            TokenC = str_split(comanditos, '\n');
            if(TokenC){
                int i;
                for (i = 0; *(TokenC + i); i++){
                    printf("token=[%s]\n", *(TokenC + i));
                    char com[100];
                    automata(*(TokenC+i));
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
        printf("\n ***Ingresar Comando***\n");
        fgets(Comando,100,stdin);
   // strcpy(Comando,"mkdisk -size::15 -path::\"/home/mitchel/Escritorio/prueba/\" -name::\"Disco1.dsk\"");
     //  strcpy(Comando,"fdisk -size::2 +unit::M -path::\"/home/mitchel/Escritorio/prueba/Disco1.dsk\" -name::\"Particion1\" +type::P");
        strcpy(Comando,"exec -path::\"/home/mitchel/Escritorio/Comandos.sh\"");
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

