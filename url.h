#define METHOD_GET 1<<0
#define METHOD_POST 1<<1
#define URL_TERMINALS 1<<2
#define URL_TERMINAL 1<<3
#define URL_TERMINAL_ID 1<<4 
#define URL_ERROR 1<<5
#define URL_VALID (URL_TERMINAL | URL_TERMINALS | URL_TERMINAL_ID)

int url_get_info(const char* url, const char* method);
int url_get_id(const char* url);
