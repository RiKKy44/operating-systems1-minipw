#define _XOPEN_SOURCE 700

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

struct Book{
    char* title;
    char* author;
    char* genre;

}typedef book_t;


// join 2 path. returned pointer is for newly allocated memory and must be freed
char* join_paths(const char* path1, const char* path2)
{
    char* res;
    const int l1 = strlen(path1);
    if (path1[l1 - 1] == '/')
    {
        res = malloc(strlen(path1) + strlen(path2) + 1);
        if (!res)
            ERR("malloc");
        strcpy(res, path1);
    }
    else
    {
        res = malloc(strlen(path1) + strlen(path2) + 2);  // additional space for "/"
        if (!res)
            ERR("malloc");
        strcpy(res, path1);
        res[l1] = '/';
        res[l1 + 1] = 0;
    }
    return strcat(res, path2);
}

void usage(int argc, char** argv)
{
    (void)argc;
    fprintf(stderr, "USAGE: %s path\n", argv[0]);
    exit(EXIT_FAILURE);
}
book_t parser(FILE* file){
    size_t len = 0; //buffor size, getline will authomatically change this variable
    char* key;
    char* value;
    char* title = NULL;
    char* genre = NULL;
    char* author = NULL;
    char* line;
    while(getline(&line,&len,file)!=-1){
        size_t size = strlen(line);
        line[size-1] = '\0';
        key = strtok(line,":");
        value = strtok(NULL,":");
        if(value == NULL){
            continue;
        }
        if(strcmp(key,"author")==0){
            author = strdup(value);
        }
        if(strcmp(key,"genre")==0){
            genre = strdup(value);
        }
        if(strcmp(key,"title")==0){
            title = strdup(value);
        }
    }
    free(line);
    if(author!=NULL){
        printf("author: %s\n",author);

    }
    else{
        printf("author: missing!\n");
    }
    if(title != NULL){
        printf("title: %s\n", title);
    }
    else{
        printf("title: missing!\n");
    }
    if(genre != NULL){
        printf("genre: %s\n", genre);
    }
    else{
        printf("genre missing!\n");
    }
    book_t book;
    book.genre = genre;
    book.title = title;
    book.author = author;
    return book;
}
int walk(const char *path, const struct stat *s, int type, struct FTW *f)
{
    //struct FTW {
    //int base;    // Indeks w 'path' gdzie zaczyna się nazwa pliku
    //int level;   // Głębokość w drzewie katalogów (0 dla root)}
    if(type == FTW_F){
        if(chdir("index/by_visible_title") == -1){
            ERR("chdir");
        }
        char* str = join_paths("../../",path);
        symlink(str, &path[f->base]);
        if(chdir("../..") == -1){
            ERR("chdir");
        }
        free(str);
        FILE *fptr;
        fptr = fopen(path, "r");
        if(fptr == NULL){
            ERR("fopen");
        }
        book_t book = parser(fptr);
        fclose(fptr);
        char trun_title[65];
        if(book.title != NULL){
            if(chdir("index/by-title") == -1){
                ERR("chdir");
            }
            strncpy(trun_title, book.title, 64);
            trun_title[64] = '\0';
            char* str2 = join_paths("../../",path);
            symlink(str2, trun_title);
            if(chdir("../..") == -1){
                ERR("chdir");
            }
            free(str2);
        }
        free(book.author);
        free(book.title);
        free(book.genre);

        
    }
    return 0;
}


int main(int argc, char** argv) { 
    if(argc!=2){
        usage(argc, argv); 
    }
    FILE* file;
    file = fopen(argv[1],"r");
    if(file==NULL){
        ERR("fopen");
    }
    book_t book = parser(file);
    fclose(file);
    free(book.author);
    free(book.genre);
    free(book.title);
    exit(EXIT_SUCCESS);
    }


