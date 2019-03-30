#include "unity.h"
#include "my_allocator.h"

#define PULL_SIZE		100
#define MEMORY_BLOCK_SIZE	20
#define THREADS_NUMBER		10

static char pull[PULL_SIZE];
char *ptr;

void setUp(void)
{
}
void tearDown(void)
{
}

void test_pull(void)
{
	int result = mysetup(pull, sizeof(pull));
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "Pull is to small!");
}

void test_alloc(void)
{
	int result = 0;
	ptr = (char *) myalloc(MEMORY_BLOCK_SIZE);
	if (ptr)
	{
		size_t block_size_begining, block_size_end;
		memcpy(&block_size_begining, &pull[1], sizeof(size_t));
		memcpy(&block_size_end, &pull[1 + sizeof(size_t) + MEMORY_BLOCK_SIZE], sizeof(size_t));
	
		if ((pull[0] == 0) && (block_size_begining == MEMORY_BLOCK_SIZE)
		&& (block_size_end == MEMORY_BLOCK_SIZE)
		&& (pull[1 + sizeof(size_t) + MEMORY_BLOCK_SIZE + sizeof(size_t)] == 0)) result = 1;
	}
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, result, "Failed to allocate block");
}

void test_free(void)
{
	int result = 0;
	if (ptr)
	{
		myfree(ptr);
		if ((pull[0] == 1) && (pull[PULL_SIZE - 1] == 1)) result = 1;
	}
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, result, "Failed to free block");
}





