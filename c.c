// #include <cpr/cpr.h>
// #include <bits/stdc++.h>
#include <curl/curl.h>
#include <stdio.h>
int main()
{
   
    CURL *curl;
    CURLcode response ;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl){
        curl_easy_setopt(curl,CURLOPT_URL,"https://google.com");
        response =curl_easy_perform(curl);
        // if(response!=CURLE_OK){
        //     fprintf(stderr,"Fail request :%s\n",curl_easy_strerror(response));
        // }
        // else{
        //     printf("HIHI %s",response);
        // }
        // curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return 0;
}