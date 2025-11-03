#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

void print(char* path, FILE* output){
    DIR* dir;
    struct dirent* entry;
    struct stat stats; //used for stat or lstat, contains size and type of file
    if ((dir=opendir(path))==NULL){
        ERR("opendir");
    }
    fprintf(output,"Path: %s\n",path); //stdout, prints something in file(output var)
    fprintf(output,"List of Files:\n");
    do {
        errno = 0;
        //we skip . and .. directories
        if ((entry = readdir(dir)) != NULL){
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        //creating full path
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if(lstat(full_path,&stats)!=0){
            ERR("lstat");
            }
        if(S_ISDIR(stats.st_mode)){//returns 0 if object is not dir
            fprintf(output,"/%s\t%lld\n",entry->d_name,(long long)stats.st_size);
            }
        if(S_ISREG(stats.st_mode)){
            fprintf(output,"%s\t%lld\n",entry->d_name,(long long)stats.st_size);
            //stats.st_size - return file size, lld format for long long
            }
            
              
        }
        
    }
    while(entry!=NULL);
    closedir(dir);//Always close dirs!
}

int main(int argc, char**argv){
    int opt;
    char** paths = NULL;
    int path_count = 0;
    char* filename = NULL;
    while((opt = getopt(argc,argv,"p:o:"))!=-1){//":" means that option must take an argument must 
        switch(opt){
            case 'p':
                paths = realloc(paths, (path_count+1)*sizeof(char*));
                if(paths==NULL){
                    ERR("realloc");
                }
                paths[path_count++] = strdup(optarg);
                break;
            case 'o':
                if(filename!=NULL){
                    fprintf(stderr,"flag -o can appear only once\n");
                    free(filename);
                }
                filename = strdup(optarg); //copying -o argument to file variable, strdup allocates memory authomatically
                break;
            case '?':
                //usage(argv[0]);
                break;
        }
    }
    FILE* output;
    if(filename!=NULL){
        output = fopen(filename,"w");
        if(output==NULL){
            ERR("fopen");
            for(int i =0;i<path_count;i++){
            free(paths[i]);
        }
            free(paths);
            free(filename);
            exit(EXIT_FAILURE);
        }
    }
    //going through each path
    for(int i =0;i<path_count;i++){
        print(paths[i],output);
        if(i<path_count-1){
            fprintf(output,"\n");
        }
    }
}