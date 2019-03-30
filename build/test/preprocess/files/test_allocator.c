#include "build/temp/_test_allocator.c"
#include "my_allocator.h"
#include "unity.h"














static char pull[100];

static char *ptr;

static int allocation_cnt;



void setUp(void)

{

}

void tearDown(void)

{

}



void *try_to_alloc(void *arg)

{

 char *local_ptr = myalloc(20);

 if (local_ptr)

 {

  myfree(ptr);

  allocation_cnt++;

 }

 return 

       ((void *)0)

           ;

}



void test_pull(void)

{

 int result = mysetup(pull, sizeof(pull));

 UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((result)), (("Pull is to small!")), (UNITY_UINT)(35), UNITY_DISPLAY_STYLE_INT);

}



void test_alloc(void)

{

 int result = 0;

 ptr = (char *) myalloc(20);

 if (ptr)

 {

  size_t block_size_begining, block_size_end;

  memcpy(&block_size_begining, &pull[1], sizeof(size_t));

  memcpy(&block_size_end, &pull[1 + sizeof(size_t) + 20], sizeof(size_t));



  if ((pull[0] == 0) && (block_size_begining == 20)

  && (block_size_end == 20)

  && (pull[1 + sizeof(size_t) + 20 + sizeof(size_t)] == 0)) result = 1;

 }

 UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((result)), (("Failed to allocate block")), (UNITY_UINT)(52), UNITY_DISPLAY_STYLE_INT);

}



void test_alloc_null(void)

{

 int result = 0;

 char *local_ptr = (char *) myalloc(2 * 20);

 if (local_ptr) result = 1;

 UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((result)), (("Had to fail this allocation")), (UNITY_UINT)(60), UNITY_DISPLAY_STYLE_INT);

}



void test_free(void)

{

 int result = 0;

 if (ptr)

 {

  myfree(ptr);

  if ((pull[0] == 1) && (pull[100 - 1] == 1)) result = 1;

 }

 UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((result)), (("Failed to free block")), (UNITY_UINT)(71), UNITY_DISPLAY_STYLE_INT);

}



void test_threads(void)

{

 pthread_t tid[10];

 for (int i = 0; i < 10; i++)

 {

  pthread_create(&tid[i], 

                         ((void *)0)

                             , try_to_alloc, 

                                             ((void *)0)

                                                 );

 }

 for (int i = 0; i < 10; i++)

 {

  pthread_join(tid[i], 

                      ((void *)0)

                          );

 }

 UnityAssertEqualNumber((UNITY_INT)((10)), (UNITY_INT)((allocation_cnt)), (("Thread Mess!")), (UNITY_UINT)(85), UNITY_DISPLAY_STYLE_INT);

}
