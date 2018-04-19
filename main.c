#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <string.h>
#include <microhttpd.h>
#include <stdlib.h>

#include <stdio.h>
#define PORT 8888

#include "json.h"
#include "url.h"
#include "terminal.h"

#define MAX_PAGE_SIZE MAX_TERMINALS * MAX_TERMINAL_SIZE + 1024

struct connenction_info_struct
{
	int connectiontype;
	struct MHD_PostProcessor *postprocessor;
	FILE *fp;
	char *answerstring;
	int answercode;
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
	//free(data);
	return ret;
}

int main ()
{
	struct MHD_Daemon *daemon;
	daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
			&answer_to_connection, NULL, MHD_OPTION_END);
	if (NULL == daemon)
		return 1;
	(void) getchar ();
	MHD_stop_daemon (daemon);
	return 0;
}
