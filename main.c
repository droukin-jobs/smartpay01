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

#define MAX_TERMINALS 100
#define MAX_TRANSACTIONS 100
#define MAX_PAGE_SIZE 1024



const char *terminal_create = "{\"cardType\":[\"Visa\",\"MasterCard\",\"EFTPOS\"],\"TransactionType\":[\"Cheque\",\"Savings\",\"Credit\"]}";   
enum cards {Visa,MAster,EFTPOS};
enum accts {Credit,Cheque,Savings};

//json helper functions
void json_int(char* tmp, const char* var, const int n){
	sprintf(tmp,"\"%s\":\"%d\"",var,n);
}
void json_float(char* tmp, const char* var, const float n){
	sprintf(tmp,"\"%s\":\"%f\"",var,n);
}
void json_str(char* tmp, const char* var, const char* n){
	sprintf(tmp,"\"%s\":\"%s\"",var,n);
}
void json_error(char* tmp, const char* n){
	sprintf(tmp, "\"error\":\"%s\"",n);
}
typedef struct {
int id;
int card;
int acct;
} transaction_type;

typedef struct {
int id;
transaction_type transactions[MAX_TRANSACTIONS];
int last_transaction;
} terminal_type;

terminal_type terminals[MAX_TERMINALS];

int last_terminal = -1;

int add_terminal(){
	if(MAX_TERMINALS == last_terminal - 1) return -1;
	int i = ++last_terminal;
	printf("creating terminal index %d\n",i);
	terminals[i].id = i+1;
	terminals[i].last_transaction = -1;
	return i;
}

int add_transaction(int terminal, int card, int acct){
	int id = terminal;
	int i = id - 1;
	if(terminal > last_terminal) return -1;
	if(MAX_TRANSACTIONS == terminals[i].last_transaction - 1) return -1;
	if(card > 3 || acct > 3 || card < 0 || acct < 0) return -1;
	int j = ++terminals[i].last_transaction;	
	terminals[i].transactions[j].id = j;
	terminals[i].transactions[j].card = card;
	terminals[i].transactions[j].acct = acct;
	return j;
}

void list_terminals(char *data){
	int i;
	char tmp[24];
	strcpy(data,"\"terminals\":[\n");
	for(i=0;i<last_terminal;i++){
		sprintf(tmp,"{\"TerminalID\":\"%04d\"},\n",terminals[i].id);
		if(i == last_terminal - 1) tmp[21] = ' ';
		printf("gathering info for term index%d, total length %d \n",i, strlen(tmp));
		memcpy(data + i*23 + 13, tmp, strlen(tmp));
	}
	data[last_terminal*23 + 12] = ']';
	data[last_terminal*23 + 13] = '\0';
}

void show_terminal_info(char* tmp, int id){
	//at the moment dummy onl
	if(id > last_terminal){
		json_error(tmp,"Invalid terminal");
		return;
	}
	sprintf(tmp,"\"terminalID\":\"%d\",\"transactions\":[\
		{\"cardType\":\"Visa\",\"TransactionType\":\"Credit\"},\
		{\"cardType\":\"EFTPOS\",\"TransactionType\":\"Savings\"}]",id);
}


struct connenction_info_struct
{
	int connectiontype;
	struct MHD_PostProcessor *postprocessor;
	FILE *fp;
	char *answerstring;
	int answercode;
};

#define METHOD_GET 1
#define METHOD_POST 2
#define URL_TERMINALS 4
#define URL_TERMINAL 8
#define URL_TERMINAL_ID 16 
#define URL_ERROR 256
#define URL_VALID (URL_TERMINAL | URL_TERMINALS | URL_TERMINAL_ID)

int url_get_info(const char* url, const char* method){
	//return int with bitmasks for methods and url components
	int result = 0;
	int len = strlen(url);
	if(strcmp(method,"POST") == 0)
		result |= METHOD_POST;
	else
		result |= METHOD_GET;
	if(len == 0 && len & METHOD_GET) return result | URL_ERROR;
	int i;
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
	if(!(result & URL_VALID) && len != 0) return URL_ERROR;
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
static int conn = 0;
static int answer_to_connection (void *cls, struct MHD_Connection *connection,
	const char *url, const char *method,
	const char *version, const char *upload_data,
	size_t *upload_data_size, void **con_cls)
{
	char page[MAX_PAGE_SIZE];
	char tmp[MAX_PAGE_SIZE];
	strcpy(tmp,"\"a\":\"A\"");
	int i;
	int urllen = strlen(url);
	//check if endpoint is corrent
	//printf("METHOD %s\n",method);
	/*if(strcmp(url,"/terminals") == 0){
		//show all terminals
		if(last_terminal == -1){
			json_str(tmp,
			"Terminals","There are no terminals");
		}else{
			list_terminals(tmp);
		}
		
	}*/
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
			list_terminals(tmp);
		}

	}
/*
else if(strcmp(url,"/terminal") == 0){
		//if POST create terminal
		printf("Method: |%s|\n",method);
		if(strcmp(method,"POST") == 0){
			printf("processing POST\n");
			int term_id = add_terminal();
			if(term_id == -1) {
				json_error(tmp,"Could not create terminal");
			}else{
				json_int(tmp,"terminalID",term_id);
			}
		}else{
			json_error(tmp,"Use POST method");
		}
	}else if(strncmp(url,"/terminals/",11) == 0){
		if(urllen > 11){
			char *id = (char*)malloc(sizeof(char) * urllen - 11 + 1);
			strcpy(id, url + 11);
			//valid id = 0 < int <= MAX_TERMINALS
			//if id not valid display error
			int term_id = atoi(id);
			if(term_id > last_terminal || term_id < 0){
				json_error(tmp,"Invalid terminal id");
				//sprintf(tmp,"\"error\" : \"Invalid terminal id %s\"",id);
			}else{	
				if(term_id <= last_terminal){
				//id exists: show terminal transactions
					if(strcmp(method,"POST") == 0){
						//process post data to update terminal
						printf("add transactions\n");
					}else{
						printf("show term info\n");
						show_terminal_info(tmp, term_id);
					}
				}
			}
			free(id);
		}
	}else{
		sprintf(tmp,"error : \"Invalid URL %s\"",url);
	}*/
	int last_mem = 0;
	sprintf(page,"{\n");
	last_mem = strlen(page);
	memcpy(page + last_mem,tmp,strlen(tmp));
	last_mem = strlen(page);
	//printf("page: |%s|, len: %d\n",page,strlen(page));
	memcpy(page + last_mem,"\n}",3);
	//printf("page: |%s|, len: %d\n",page,strlen(page));
	
	char *data=(char*)malloc(2 + strlen(tmp) + 2);
	//char *data=(char*)malloc(20);
	sprintf(data,"{%s}",tmp);
	//printf("data|%s|, len=%d\n",data,strlen(data));
	//sprintf(data,"12345678901234567890");
	printf("data|%s|, len=%d\n",data,strlen(data));
	struct MHD_Response *response;
	int ret;
	response = MHD_create_response_from_buffer (strlen (data), (void *) data,
				MHD_RESPMEM_PERSISTENT);
	//add json headers
	MHD_add_response_header(response,"Content-Type", "application/json");
	MHD_add_response_header(response,"Accept", "application/json");
	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	MHD_destroy_response (response);
	free(data);
	return ret;
}

/*
static int conn = 0;
static int answer_to_connection (void *cls, struct MHD_Connection *connection,
		const char *url, const char *method,
		const char *version, const char *upload_data,
		size_t *upload_data_size, void **con_cls)
{
	char page[MAX_PAGE_SIZE];
	char tmp[MAX_PAGE_SIZE];
	strcpy(tmp,"\"a\":\"A\"");
	char *terminal_create = "{\"cardType\":[\"Visa\",\"MasterCard\",\"EFTPOS\"],\"TransactionType\":[\"Cheque\",\"Savings\",\"Credit\"]}";   
	int i;
	int urllen = strlen(url);
	//check if endpoint is corrent
	printf("METHOD %s\n",method);
	if(strcmp(url,"/terminals") == 0){
		//show all terminals
		if(last_terminal == -1){
			sprintf(tmp,
			"\"Terminals\":\"There are no terminals\"");
		}else{
			list_terminals(tmp);
		}
		
	}else if(strcmp(url,"/terminal") == 0){
		//if POST create terminal
		if(strcmp(method,"POST") == 0){
			printf("processing POST\n");
			//process post request
			//struct post_status *post = NULL;
			  //post = (struct post_status*)*con_cls;

			  //if(post == NULL) {
				//post = malloc(sizeof(struct post_status));
				//post->status = 0;
				//*con_cls = post;
			  //}

			  //if(!post->status) {
				//post->status = 1;
				//return MHD_YES;
			  //} else {
			//	if(*upload_data_size != 0) {
					//post->buff = malloc(*upload_data_size + 1);
					//snprintf(post->buff, *upload_data_size+1,"%s",upload_data);
			//		*upload_data_size = 0;
			//		return MHD_YES;
				//} else {
				//	if(post->buff == NULL) printf("postbuff null\n");
				//	if(strlen(post->buff) > 0){
						
				//		sprintf(tmp,"%s",post->buff);
				//		if(strcmp(post->buff, terminal_create) == 0){
				//			printf("add terminal\n");
				//			add_terminal();
				//			strcat(tmp,terminal_create);
				//		}else{
				//			strcat(tmp,"FAIL");
				//		}
				//	}else{
				//		printf("add terminal\n");
				//		add_terminal();
				//	}
				//	free(post->buff);
					//free(post);
				//}
			//  } 

			  //if(post != NULL)
				//free(post);
			//}else if (NULL != con_info->answerstring) {
			//	strcpy(tmp,con_info->answerstring);
			//} 
			//end post
		}
	}else if(strncmp(url,"/terminals/",11) == 0){
		if(urllen > 11){
			char *id = (char*)malloc(sizeof(char) * urllen - 11 + 1);
			strcpy(id, url + 11);
			//valid id = 0 < int <= MAX_TERMINALS
			//if id not valid display error
			int term_id = atoi(id);
			if(term_id == 0){
				sprintf(tmp,
				"\"error\" : \"Invalid terminal id %s\"",
				id);
			}else{	
				if(term_id < last_terminal + 1){
				//id exists: show terminal transactions
					sprintf(tmp,
					"\"info\" : \"Show info for terminal %s\"",
					id);
				}else{
				//else create terminal
					sprintf(tmp,
					"\"info\" : \"Will create terminal %s\"",
					id);
					if(add_terminal() == -1){
						sprintf(tmp,
						"\"error\" : \"Could not create terminal %s\"",
						id);

					};
				}
			}
			free(id);
		}
	}else{
		sprintf(tmp,"error : \"Invalid URL %s\"",url);
	}
	int last_mem = 0;
	sprintf(page,"{\n");
	last_mem = strlen(page);
	memcpy(page + last_mem,tmp,strlen(tmp));
	last_mem = strlen(page);
	//printf("page: |%s|, len: %d\n",page,strlen(page));
	memcpy(page + last_mem,"\n}",3);
	//printf("page: |%s|, len: %d\n",page,strlen(page));
	
	char *data=(char*)malloc(2 + strlen(tmp) + 2);
	//char *data=(char*)malloc(20);
	sprintf(data,"{%s}",tmp);
	//printf("data|%s|, len=%d\n",data,strlen(data));
	//sprintf(data,"12345678901234567890");
	printf("data|%s|, len=%d\n",data,strlen(data));
	struct MHD_Response *response;
	int ret;
	response = MHD_create_response_from_buffer (strlen (data), (void *) data,
				MHD_RESPMEM_PERSISTENT);
	//add json headers
	//MHD_add_response_header(response,"Content-Type", "text/html");
	MHD_add_response_header(response,"Content-Type", "application/json");
	MHD_add_response_header(response,"Accept", "application/json");
	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	//printf("tmp [%s]\n",tmp);
	//printf("raw data [%s], strlen = %d\n",page, strlen(page));
	MHD_destroy_response (response);
	free(data);
	return ret;
}
*/
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
