#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
//#define MHD_PLATFORM_H
#include <string.h>
#include <microhttpd.h>
#include <stdlib.h>

#include <stdio.h>
#define PORT 8888

#include "json.h"
#include "url.h"
#include "terminal.h"

#include <stdbool.h>

#define PAGE "<html><head><title>libmicrohttpd demo</title>"\
	"</head><body>libmicrohttpd demo!!</body></html>"

#define MAX_PAGE_SIZE MAX_TERMINALS * MAX_TERMINAL_SIZE + 1024

struct connenction_info_struct
{
	int connectiontype;
	struct MHD_PostProcessor *postprocessor;
	FILE *fp;
	char *answerstring;
	int answercode;
};
struct postStatus {
	bool status;
	char *buff;
};

static int answer_to_connection (void *cls, struct MHD_Connection *connection,
		const char *url, const char *method,
		const char *version, const char *upload_data,
		size_t *upload_data_size, void **con_cls)
{
	char page[MAX_PAGE_SIZE];
	char tmp[MAX_PAGE_SIZE];
	strcpy(tmp,"\"a\":\"A\"");
	int uinfo = url_get_info(url,method);
	if(uinfo & URL_ERROR) json_error(tmp,"Invalid URL");
	else if (uinfo & METHOD_POST){
		if(uinfo & URL_TERMINAL || uinfo == METHOD_POST){
			int id = add_terminal();
			if(id == -1) json_error(tmp,"Could not create terminal");
			else json_int(tmp,"terminalID",id);
		}else if(uinfo & URL_TERMINAL_ID){
			//process terminal transaction
			char* post_data;
			struct postStatus *post = NULL;
			post = (struct postStatus*)*con_cls;

			if(post == NULL) {
				post = malloc(sizeof(struct postStatus));
				post->status = false;
				*con_cls = post;
			}

			if(!post->status) {
				post->status = true;
				return MHD_YES;
			} else {
				if(*upload_data_size != 0) {
					printf("data [%s] size %d\n",upload_data, *upload_data_size);
					post->buff = (char*)malloc(*upload_data_size + 1);
					sprintf(post->buff, "%s",upload_data);
					printf("postbuff [%s] size: %d\n",post->buff,strlen(post->buff));
					*upload_data_size = 0;
					return MHD_YES;
				} else {
					printf("Post data: [%s]\n",post->buff);
					post_data=(char*)malloc(strlen(post->buff)+2);
					sprintf(post_data,"{%s}",post->buff);
					if(post->buff != NULL)
						free(post->buff);
				}
			} 

			if(post != NULL)
				free(post);
		}
	}else{
		if(uinfo & URL_TERMINAL) json_error(tmp,"Use POST method");
		else if(uinfo & URL_TERMINAL_ID){
			int id = url_get_id(url);
			if(id == -1) json_error(tmp,"Invalid terminal ID");
			else show_terminal_info(tmp,id);
		}else if(uinfo & URL_TERMINALS){
			if(list_terminals(tmp, MAX_PAGE_SIZE) == -1) json_error(tmp,"No terminals");
		}

	}

	char *data=(char*)malloc(strlen(tmp) + 3);
	sprintf(data,"{%s}",tmp);
	//char *data = "{'test':'value'}";
	//printf("data|%s|, len=%d\n",data,strlen(data));
	struct MHD_Response *response;
	int ret;
	response = MHD_create_response_from_buffer (strlen (data), (void *) data,
			MHD_RESPMEM_PERSISTENT);
	//add json headers
	MHD_add_response_header(response,"Content-Type", "application/json");
	MHD_add_response_header(response,"Accept", "application/json");
	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	MHD_destroy_response (response);
	//free(data); - causes memory corruption and accasional segfault 
	return ret;
}



static int ahc_echo(void * cls,
		struct MHD_Connection * connection,
		const char * url,
		const char * method,
		const char * version,
		const char * upload_data,
		size_t * upload_data_size,
		void ** ptr) {
	const char * page = cls;
	char *data;
	struct MHD_Response * response;
	int ret;

	if(strcmp(method,"POST") == 0){

		struct postStatus *post = NULL;
		post = (struct postStatus*)*ptr;

		if(post == NULL) {
			post = malloc(sizeof(struct postStatus));
			post->status = false;
			*ptr = post;
		}

		if(!post->status) {
			post->status = true;
			return MHD_YES;
		} else {
			if(*upload_data_size != 0) {
				printf("data [%s] size %d\n",upload_data, *upload_data_size);
				post->buff = (char*)malloc(*upload_data_size + 1);
				sprintf(post->buff, "%s",upload_data);
				printf("postbuff [%s] size: %d\n",post->buff,strlen(post->buff));
				*upload_data_size = 0;
				return MHD_YES;
			} else {
				printf("Post data: [%s]\n",post->buff);
				data=(char*)malloc(strlen(post->buff)+2);
				sprintf(data,"{%s}",post->buff);
				if(post->buff != NULL)
					free(post->buff);
			}
		} 

		if(post != NULL)
			free(post);
	}else{
		data=(char*)malloc(strlen("GET")+1);
		sprintf(data,"GET");
	}
	printf("before response:\n");
	response = MHD_create_response_from_buffer (strlen(data),
			(void*) data,
			MHD_RESPMEM_PERSISTENT);
	ret = MHD_queue_response(connection,
			MHD_HTTP_OK,
			response);
	MHD_destroy_response(response);
	return ret;
}


int main ()
{
	struct MHD_Daemon *daemon;
	//daemon = MHD_start_daemon (MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL,
	//		&ahc_echo, NULL, MHD_OPTION_END);
	daemon = MHD_start_daemon (MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL,
			&answer_to_connection, NULL, MHD_OPTION_END);
	if (NULL == daemon)
		return 1;
	(void) getchar ();
	MHD_stop_daemon (daemon);
	return 0;
}
