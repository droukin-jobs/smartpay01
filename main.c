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

enum cards {Visa,MAster,EFTPOS};
enum accts {Credit,Cheque,Savings};

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
	sprintf(tmp,"\"terminalID\":\"%d\",\"transactions\":[\
		{\"cardType\":\"Visa\",\"TransactionType\":\"Credit\"},\
		{\"cardType\":\"EFTPOS\",\"TransactionType\":\"Savings\"}]",id);
}

struct post_status {
	int status;
	char *buff;
};

void json_error(char* tmp, const char* err){
	sprintf("\"error\":\"%s\"",err);
}

struct connenction_info_struct
{
	int connectiontype;
	struct MHD_PostProcessor *postprocessor;
	FILE *fp;
	char *answerstring;
	int answercode;
};
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
	//printf("METHOD %s\n",method);
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
		printf("Method: |%s|\n",method);
		if(strcmp(method,"POST") == 0){
			printf("processing POST\n");
			int term_id = add_terminal();
			if(term_id == -1) {
				json_error(tmp,"Could not create terminal");
			}else{
				sprintf(tmp,"\"terminalID\":\"%d\"",term_id);
			}
		}
	}else if(strncmp(url,"/terminals/",11) == 0){
		if(urllen > 11){
			char *id = (char*)malloc(sizeof(char) * urllen - 11 + 1);
			strcpy(id, url + 11);
			//valid id = 0 < int <= MAX_TERMINALS
			//if id not valid display error
			int term_id = atoi(id);
			if(term_id > MAX_TERMINALS || term_id < 0){
				sprintf(tmp,
				"\"error\" : \"Invalid terminal id %s\"",
				id);
			}else{	
				if(term_id < last_terminal + 1){
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
