#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))


void usage(char* pname){
    fprintf(stderr, "USAGE:%s -n Name -p OCTAL -s SIZE\n", pname);
    exit(EXIT_FAILURE);

}

void make_file(char* name, ssize_t size, mode_t perms, int percent){
    FILE *file;
    int i;
    umask(~perms & 0777);
    if ((file =fopen(name,"w+"))==NULL){
        ERR("fopen");
    }
    for (i=0;i<size;i++){
        fputc(0,file);
    }
    for (i=0;i<(size*percent)/100;i++){
        if(fseek(file,rand()%size,SEEK_SET)){
            ERR("fseek");
        }
        fprintf(file,"%c", 'A'+(i%('Z'-'A'+1)));
    }
    if (fclose(file)){
        ERR("fclose");
    }
    }
int main(int argc, char** argv){

}


