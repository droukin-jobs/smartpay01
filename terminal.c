#include <stdio.h>
#include "terminal.h"
#include "json.h"

static terminal_type terminals[MAX_TERMINALS];
static int last_terminal = -1;
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
