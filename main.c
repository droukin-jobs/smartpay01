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
#define MAX_PAGE_SIZE 255

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
	char tmp[22];
	strcpy(data,"\"terminals\":[\n");
	for(i=0;i<last_terminal;i++){
		sprintf(tmp,"{\"terminal\":\"%04d\"},\n",terminals[i].id);
		if(i == last_terminal - 1) tmp[19] = ' ';
		printf("gathering info for term index%d, total length %d \n",i, strlen(tmp));
		memcpy(data + i*21 + 13, tmp, strlen(tmp));
	}
	data[last_terminal*21 + 12] = ']';
	data[last_terminal*21 + 13] = '\0';
}

static int conn = 0;
static int answer_to_connection (void *cls, struct MHD_Connection *connection,
		const char *url, const char *method,
		const char *version, const char *upload_data,
		size_t *upload_data_size, void **con_cls)
{
	char page[MAX_PAGE_SIZE];
	char tmp[MAX_PAGE_SIZE];
	int i;
	int urllen = strlen(url);
	for(i=0;i<MAX_PAGE_SIZE-1;i++){
		page[i] = '0';
	}
	page[MAX_PAGE_SIZE-1]='\0';
	sprintf(page,"{\n");
	//check if endpoint is corrent
	if(strncmp(url,"/terminals",10) == 0){
		if(last_terminal == -1){
			sprintf(tmp,
			"\"Terminals\":\"There are no terminals\"");
		}else{
			list_terminals(tmp);
		}
		
	}else if(strncmp(url,"/terminal",9) == 0){
		//check if want ot create or view terminal
		if(urllen == 9 || ( urllen == 10 && url[9] == '/')){
			//list all terminals
			sprintf(tmp,
			"\"url\" : \"Connection %d at URL %s\"",
			conn++,url);
		}else{
			if(urllen > 10){
				char *id = (char*)malloc(sizeof(char) * urllen - 10 + 1);
				strcpy(id, url + 10);
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
			
		}
	}else{
		sprintf(tmp,"error : \"Invalid URL %s\"",url);
	}
	strcat(page,tmp);
	strcat(page,"\n}");
	//page[strlen(page)] = '\0';
	struct MHD_Response *response;
	int ret;
	response = MHD_create_response_from_buffer (strlen (page), (void *) page,
				MHD_RESPMEM_PERSISTENT);
	//add json headers
	//MHD_add_response_header(response,"Content-Type", "text/html");
	MHD_add_response_header(response,"Content-Type", "application/json");
	MHD_add_response_header(response,"Accept", "application/json");
	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	MHD_destroy_response (response);
	printf("tmp %s\n",tmp);
	printf("raw data %s, strlen = %d\n",page, strlen(page));
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
