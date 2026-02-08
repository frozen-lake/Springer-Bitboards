#include "tests.h"
#include "../src/transposition_table.h"

int test_create_table(){
    return -1;
}  

void transposition_table_tests(){
    int num_tests = 1;

	int (*test_cases[num_tests])();
	char* test_case_names[num_tests];

    
	test_cases[0] = test_create_table;

	test_case_names[0] = "test_create_table";

    run_tests(test_cases, test_case_names, num_tests);
}