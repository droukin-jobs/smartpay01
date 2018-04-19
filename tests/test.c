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

void test_url

int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("json", NULL, NULL);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "test of json_int()", test_json_int)) ||
       (NULL == CU_add_test(pSuite, "test of json_str()", test_json_str)) ||
       (NULL == CU_add_test(pSuite, "test of json_float()", test_json_float)) ||
       (NULL == CU_add_test(pSuite, "test of json_error()", test_json_error)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}


