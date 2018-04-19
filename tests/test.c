#include <stdio.h>
#include <string.h>
#include "../json.h"
#include "../url.h"
#include "../terminal.h"
#include "CUnit/Basic.h"

void test_json_int(void)
{
   	int i1 = 10;
	char tmp[20];
	char *out="\"test\":\"10\"";
	json_int(tmp,"test",10);
      CU_ASSERT(strcmp(tmp,out) == 0);
}
void test_json_str(void)
{
	char tmp[20];
	char *out="\"test\":\"value\"";
	json_str(tmp,"test","value");
      CU_ASSERT(strcmp(tmp,out) == 0);
}
void test_json_float(void)
{
	char tmp[20];
	char *out="\"test\":\"5.5\"";
	json_float(tmp,"test",5.5);
      CU_ASSERT(strcmp(tmp,out) == 0);
}
void test_json_error(void)
{
	char tmp[20];
	char *out="\"error\":\"value\"";
	json_error(tmp,"value");
      CU_ASSERT(strcmp(tmp,out) == 0);
}
void test_url_get_info_post(void){
	//test for method = POST
	char url[100];
	char method[10];
	int result = 0;	
	sprintf(method,"POST");

	//test empty url
	sprintf(url,"");
	result = url_get_info(url,method);
	CU_ASSERT(result & METHOD_POST);
	CU_ASSERT_FALSE(result & URL_VALID);
	CU_ASSERT_FALSE(result & METHOD_GET);

	//test /terminal 
	sprintf(url,"/terminal");
	result = url_get_info(url,method);
	CU_ASSERT(result & URL_TERMINAL);
	CU_ASSERT(result & URL_VALID);
	CU_ASSERT_FALSE(result & URL_TERMINALS);
	CU_ASSERT_FALSE(result & URL_TERMINAL_ID);
}
void test_url_get_info_get(void){
	//test for method = GET
	char url[100];
	char method[10];
	int result = 0;	
	sprintf(method,"GET");

	//test empty url
	sprintf(url,"");
	result = url_get_info(url,method);
	CU_ASSERT(result & METHOD_GET);
	CU_ASSERT_FALSE(result & URL_VALID);
	CU_ASSERT_FALSE(result & METHOD_POST);

	//test /terminals 
	sprintf(url,"/terminals");
	result = url_get_info(url,method);
	CU_ASSERT(result & URL_TERMINALS);
	CU_ASSERT(result & URL_VALID);
	CU_ASSERT_FALSE(result & URL_TERMINAL);
	CU_ASSERT_FALSE(result & URL_TERMINAL_ID);

	//test /terminal_id 
	sprintf(url,"/terminals/3");
	result = url_get_info(url,method);
	CU_ASSERT(result & URL_TERMINAL_ID);
	CU_ASSERT_FALSE(result & URL_TERMINAL);
	CU_ASSERT_FALSE(result & URL_TERMINALS);
}

int main()
{
   CU_pSuite suite_json = NULL;
   CU_pSuite suite_url = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   suite_json = CU_add_suite("json", NULL, NULL);
   if (NULL == suite_json) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   suite_url = CU_add_suite("url", NULL, NULL);
   if (NULL == suite_url) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(suite_json, "test of json_int()", test_json_int)) ||
       (NULL == CU_add_test(suite_json, "test of json_str()", test_json_str)) ||
       (NULL == CU_add_test(suite_json, "test of json_float()", test_json_float)) ||
       (NULL == CU_add_test(suite_json, "test of json_error()", test_json_error)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(suite_url, "test of url_get_info(POST)", test_url_get_info_post)) ||
   	(NULL == CU_add_test(suite_url, "test of url_get_info(GET)", test_url_get_info_get)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
CU_automated_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}


