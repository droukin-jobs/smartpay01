
#define MAX_TRANSACTIONS 100
#define MAX_TERMINALS 100

static const char *terminal_create = "{\"cardType\":[\"Visa\",\"MasterCard\",\"EFTPOS\"],\"TransactionType\":[\"Cheque\",\"Savings\",\"Credit\"]}";   
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


int add_terminal();
int add_transaction(int terminal, int card, int acct);
void list_terminals(char *data);
void show_terminal_info(char* tmp, int id);
