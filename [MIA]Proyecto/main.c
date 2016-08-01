#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>


char com[1000];
char com3[1000];
int salida=0;
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

