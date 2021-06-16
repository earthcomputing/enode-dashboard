/*---------------------------------------------------------------------------------------------
 *  Copyright © 2016-present Earth Computing Corporation. All rights reserved.
 *  Licensed under the MIT License. See LICENSE.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
 
int main(int argc, char** argv){
  
  CURL *curl;
  CURLcode res;
  char ping[127];
  char c;
  int i = 0;

  curl = curl_easy_init();
  if(!curl) {
    
    while (c = getc(stdin)) {
      
      ping[i++] = c;
      if (c = '\n') {
	i = 0;
	printf("%s", ping);
	//putc(c, stdout);
	
	  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/enp2s0Up");
	  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ping);
	  
	  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/enp2s0Down");
	  // if we don't provide POSTFIELDSIZE, libcurl will strlen() by itself  
	  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(ping));
	  
	  // Perform the request, res will get the return code 
	  res = curl_easy_perform(curl);
	  // Check for errors 
	  if(res != CURLE_OK)
	  fprintf(stderr, "curl_easy_perform() failed: %s\n",
	  curl_easy_strerror(res));
	  
	
      }
    }
    // always cleanup
    curl_easy_cleanup(curl);
  }
  return 0;
}
