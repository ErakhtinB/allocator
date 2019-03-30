#include "my_allocator.h"

static char *buffer;
static size_t global_buffer_size;
static char max_address_bytes = sizeof(size_t);
static char service_bytes_size;
static sem_t allocator_semaphore;

int mysetup(void *buf,  size_t size)
{
    //count service bytes
    service_bytes_size= 1 + 2 * max_address_bytes + 1;

    //check if incoming buffer is big enough to allocate even 1-byte of payload
    size_t min_size = service_bytes_size + 1;
    if (size < min_size) return 1;

    //init global vars
    global_buffer_size = size - service_bytes_size;
    buffer = (char *) buf;

    //set flag buffer is free in the beggining
    *buffer = 1;

    //allocate size values in memory
    memcpy(buffer + 1, &global_buffer_size, max_address_bytes);
    memcpy(buffer + size - 1 - max_address_bytes, &global_buffer_size, max_address_bytes);

    //set flag buffer is free in the end
    *(buffer + size - 1) = 1;

    //create semaphore
    if (sem_init(&allocator_semaphore, 0, 1)) return 2;
    return  0;
}

void *myalloc(size_t size)
{
    //wait to take semaphore
    sem_wait(&allocator_semaphore);

    //bufferize size and initial pointer
    if (size > (global_buffer_size - service_bytes_size)) return NULL;
    char *local_buffer = buffer;

    while (1)
    {
        //if buffer is free
        if (*local_buffer == 1)
        {
            //get the size value
            size_t block_size;
            memcpy(&block_size, local_buffer + 1, sizeof(block_size));

            //if it is big enough
            if (block_size > (1 + max_address_bytes + size + max_address_bytes + 1))
            {
                //set local_buffer busy flag
                *local_buffer = 0;

                //set size values to the beginning and the end
                memcpy(local_buffer + 1, &size, sizeof(size));
                memcpy(local_buffer + 1 + max_address_bytes + size, &size, sizeof(size));

                //local_buffer busy flag
                *(local_buffer + 1 + max_address_bytes + size + max_address_bytes) = 0;
                char *return_buffer = local_buffer + 1 + max_address_bytes;

                //lack of buffer is gonna be set as free
                *(local_buffer + 1 + max_address_bytes + size + max_address_bytes + 1) = 1;

                //count the new size
                 size_t new_size = block_size - service_bytes_size - size;

                //set new size to the fields
                memcpy(local_buffer + 1 + max_address_bytes + size + max_address_bytes + 1 + 1, &new_size, sizeof(new_size));
                memcpy(local_buffer + 1 + max_address_bytes + size + max_address_bytes + 1 + 1 + max_address_bytes + new_size, &new_size, sizeof(new_size));

                //set free flag
                *(local_buffer + 1 + max_address_bytes + size + max_address_bytes + 1 + 1 + max_address_bytes + new_size + max_address_bytes) = 1;

                //give semahore and return ptr
                sem_post(&allocator_semaphore);
                return (void *) return_buffer;
            }
            else
            {
                //shift to next block
                local_buffer += block_size + service_bytes_size;

                //check if it's not going out of range
                if (local_buffer > ((buffer + global_buffer_size - 1) - service_bytes_size))
                {
                    //give semahore and return zero-ptr
                    sem_post(&allocator_semaphore);
                    return NULL;
                }
                else
                {
                    continue;
                }
            }
        }
        else //shift to the next block
        {
            size_t block_size;
            memcpy(&block_size, local_buffer + 1, sizeof(block_size));
            local_buffer += block_size + service_bytes_size;
            if (local_buffer > ((buffer + global_buffer_size - 1) - service_bytes_size))
            {
                //give semahore and return zero-ptr
                sem_post(&allocator_semaphore);
                return NULL;
            }
        }
    }
}

void myfree(void *p)
{
    //wait to take semaphore
    sem_wait(&allocator_semaphore);

    char *local_buffer = (char *) p;
    size_t block_size;
    while (1)
    {
        memcpy(&block_size, local_buffer - max_address_bytes, sizeof(block_size));
        //first case, around blocks are not free
        if ((*(local_buffer - max_address_bytes - 1 - 1) ==  0) && (*(local_buffer + block_size + max_address_bytes + 1) == 0))
        {
            //now this block is free
            *(local_buffer - max_address_bytes - 1) = 1;
            *(local_buffer + block_size + max_address_bytes) = 1;
            break;
        }
        //second case, previous is free, so we have to unite with it
        else if (*(local_buffer - max_address_bytes - 1 - 1) ==  1)
        {
            size_t prev_block_size, prev_block_size_buf;
            memcpy(&prev_block_size, local_buffer - service_bytes_size, sizeof(prev_block_size));
            prev_block_size_buf = prev_block_size;
            prev_block_size += block_size + service_bytes_size;
            memcpy(local_buffer - service_bytes_size - prev_block_size_buf - max_address_bytes, &prev_block_size, sizeof(prev_block_size));
            memcpy(local_buffer + block_size, &prev_block_size, sizeof(prev_block_size));
            *(local_buffer + block_size + max_address_bytes) = 1;
            local_buffer = local_buffer - service_bytes_size - prev_block_size_buf;
        }
        //third case, next is free, so we have to unite with it
        else
        {
            size_t prev_block_size, prev_block_size_buf;
            memcpy(&prev_block_size, local_buffer + block_size + max_address_bytes + 1 + 1, sizeof(prev_block_size));
            prev_block_size_buf = prev_block_size;
            prev_block_size += block_size + service_bytes_size;
            memcpy(local_buffer - max_address_bytes, &prev_block_size, sizeof(prev_block_size));
            memcpy(local_buffer + block_size + service_bytes_size + prev_block_size_buf, &prev_block_size, sizeof(prev_block_size));
            *(local_buffer + block_size + service_bytes_size + prev_block_size_buf + max_address_bytes) = 1;
        }
    }
    //give semaphore
    sem_post(&allocator_semaphore);
}
