#include <stdio.h>
#include <stdlib.h>
#include "terminal.h"
#include "json.h"

static terminal_type terminals[MAX_TERMINALS];
static int last_terminal = -1;
int add_terminal(){
	if((MAX_TERMINALS - 1) <= last_terminal) return -1;
	int i = ++last_terminal;
	printf("creating terminal index %d",i);
	terminals[i].id = i;
	terminals[i].last_transaction = -1;
	printf(" done\n");
	return i;
}

int add_transaction(int terminal, int card, int acct){
	int id = terminal;
	int i = id;
	if(terminal > last_terminal) return -1;
	if(MAX_TRANSACTIONS == terminals[i].last_transaction - 1) return -1;
	if(card > 3 || acct > 3 || card < 0 || acct < 0) return -1;
	int j = ++terminals[i].last_transaction;	
	terminals[i].transactions[j].id = j;
	terminals[i].transactions[j].card = card;
	terminals[i].transactions[j].acct = acct;
	return j;
}

int list_terminals(char *data, const int max_data){
	int i;
	char tmp[24];
	if(last_terminal == -1){
		return last_terminal;
	}
	int data_len = sprintf(data,"\"terminals\":[\n");
	for(i=0;i<=last_terminal;i++){
		int len = sprintf(tmp,"{\"TerminalID\":\"%04d\"},\n",terminals[i].id);
		if(i == last_terminal) tmp[len - 2] = ' ';
		printf("gathering info for term index%d, total length %d \n",i, strlen(tmp));
		memcpy(data + data_len , tmp, strlen(tmp));
		data_len += len;
		if(data_len > max_data - len -2 ) break;
	}
	data[data_len] = ']';
	data[data_len+1] = '\0';
	printf("data len=%d of %d\n",strlen(data),max_data);
	return last_terminal;
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
