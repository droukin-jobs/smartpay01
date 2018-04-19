#include <stdio.h>
#include <stdlib.h>
#include "url.h"

int url_get_info(const char* url, const char* method){
	//return int with bitmasks for methods and url components
	int result = 0;
	int len = strlen(url);
	//for some reason empty URL has lenght 1
	if(strcmp(method,"POST") == 0){
		result |= METHOD_POST;
		if(len == 1) return METHOD_POST;
	}else{
		result |= METHOD_GET;
		if(len == 1) return URL_ERROR;
	}
	if( strncmp("/terminals",url, strlen("/terminals")) == 0 ){
		if(len == strlen("/terminals")) 
			result |= URL_TERMINALS;
		else
			result |= URL_TERMINAL_ID;
	}
	if(!( result & URL_TERMINALS )){
		if( strcmp("/terminal",url) == 0 ){
			result |= URL_TERMINAL;
		}
	}
	if(!(result & URL_VALID) && len != 1) return URL_ERROR;
	return result; 
}

int url_get_id(const char* url){
	int len = strlen("/terminals/");
	if(strlen(url) < len) return -1;
	char *id = (char*)malloc(strlen(url) - len + 1);
	sprintf(id,url + len);
	int term_id = atoi(id);
	if(term_id == 0){
		if(strcmp(id,"0") != 0) return -1;
	}
	return term_id;
}
