#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H
#include <string.h>
#include <semaphore.h>
int mysetup(void *buf, size_t size);
void *myalloc(size_t size);
void myfree(void *p);
#endif // MY_ALLOCATOR_H
