#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char* mem_chunk;

//Creates the memory location of N bytes
void INITIALIZE(unsigned char* mem_chunk, int chunk_size)
{
    int i;
    *mem_chunk = (unsigned char*)malloc(chunk_size * sizeof(unsigned char));
    for (i = 0; i < chunk_size; i++) {
        mem_chunk[i] = 0;
    }
}

//Ends the program
void FINALIZE(unsigned char* mem_chunk)
{
    free(mem_chunk);
}

//Allocates a block of SIZE bytes and prints the memory location
//of the first byte in the block or 0 if there isn't enough space
int ALLOC(unsigned char* mem_chunk, int chunk_size, unsigned int SIZE, int* alloc_blocks_ptr)
{
    int *int_cast_ptr_offset, i, *int_cast_ptr = &(mem_chunk[0]), *cast_helper;
    if (*int_cast_ptr == 0) {
        if (SIZE + 12 > chunk_size - 4) {
            printf("0\n");
            return 1;
        }
        else {
            *(int_cast_ptr + 1) = 0;
            *(int_cast_ptr + 2) = 0;
            *(int_cast_ptr + 3) = SIZE + 12;
            *int_cast_ptr = 4;
            printf("16\n");
            *alloc_blocks_ptr = *alloc_blocks_ptr + 1;
            return 0;
        }
    }
    else {
        if (*int_cast_ptr - 4 >= 12 + SIZE) {
            int_cast_ptr_offset = &(mem_chunk[*int_cast_ptr]);
            *(int_cast_ptr + 1) = *int_cast_ptr;
            *(int_cast_ptr + 2) = 0;
            *(int_cast_ptr + 3) = 12 + SIZE;
            *(int_cast_ptr_offset + 1) = 4;
            *int_cast_ptr = 4;
            printf("16\n");
            (*alloc_blocks_ptr)++;
            return 0;
        }
        else {
            int_cast_ptr_offset = &(mem_chunk[*int_cast_ptr]);
            i = *int_cast_ptr;
            int_cast_ptr = int_cast_ptr_offset;
            while (*int_cast_ptr_offset != 0) {
                int_cast_ptr = &(mem_chunk[*int_cast_ptr_offset]);
                if (SIZE + 12 <= *int_cast_ptr_offset - *(int_cast_ptr + 1) - *(int_cast_ptr_offset + 2)) {
                    cast_helper = &(mem_chunk[*(int_cast_ptr_offset + 2) + *(int_cast_ptr + 1)]);
                    *cast_helper = *int_cast_ptr_offset;
                    *(cast_helper + 1) = *(int_cast_ptr + 1);
                    *(cast_helper + 2) = SIZE + 12;
                    *int_cast_ptr_offset = *(int_cast_ptr_offset + 2) + *(int_cast_ptr + 1);
                    *(int_cast_ptr + 1) = *int_cast_ptr_offset;
                    printf("%d\n", *int_cast_ptr_offset + 12);
                    (*alloc_blocks_ptr)++;
                    return 0;
                }
                else {
                    i = *int_cast_ptr_offset;
                    int_cast_ptr_offset = &mem_chunk[*int_cast_ptr_offset];
                }
            }
            if (SIZE + 12 <= chunk_size - i - *(int_cast_ptr_offset + 2)) {
                cast_helper = &mem_chunk[i + *(int_cast_ptr_offset + 2)];
                *cast_helper = 0;
                *(cast_helper + 1) = i;
                *(cast_helper + 2) = 12 + SIZE;
                *int_cast_ptr_offset = i + *(int_cast_ptr_offset + 2);
                printf("%d\n", *int_cast_ptr_offset + 12);
                (*alloc_blocks_ptr)++;
                return 0;
            }
        }
    }
    printf("0\n");
    return -1;
}

//Prints the memory in hex
int DUMP(unsigned char* mem_chunk, int chunk_size)
{
    int i;
    for (i = 0; i < chunk_size; i++) {
        if (i == 0) {
            printf("%08X\t", i);
        }
        if (i % 16 == 0 && i != 0) {
            printf("\n%08X\t", i);
        }
        if ((i + 8) % 16 == 0) {
            printf(" ");
        }
        printf("%02X ", mem_chunk[i]);
    }
    printf("\n%08X\n", chunk_size);
    return 0;
}

//Writes X in memory S times, beginning at the byte I
int FILL(unsigned char* mem_chunk, int chunk_size, int i, int x, int s)
{
    int j;
    for (j = i; j < i + s && j < chunk_size; j++) {
        mem_chunk[j] = x;
    }
    return 0;
}

//Frees the block starting at the byte I
int FREE(unsigned char* mem_chunk, int i, int* alloc_blocks_ptr)
{
    int *int_cast_ptr, *next_ptr, *prev_ptr, size;
    int_cast_ptr = &(mem_chunk[i - 12]);
    size = *(int_cast_ptr + 2);
    prev_ptr = &(mem_chunk[*int_cast_ptr + 4]);
    *prev_ptr = *(int_cast_ptr + 1);
    next_ptr = &(mem_chunk[*(int_cast_ptr + 1)]);
    *next_ptr = *int_cast_ptr;
    *alloc_blocks_ptr = *alloc_blocks_ptr - 1;
    return 0;
}

//Prints no of blocks and bytes free
int SHOW_FREE(unsigned char* mem_chunk, int chunk_size, int* alloc_blocks_ptr)
{
    int *int_cast_ptr, free_bytes = 0, free_blocks = 0, *cast_offset, ok = 0;
    int_cast_ptr = &(mem_chunk[0]);
    if (*int_cast_ptr == 0) {
        printf("1 blocks (%d bytes) free\n", chunk_size - 4);
        return 0;
    }
    if (*int_cast_ptr > 4) {
        free_blocks++;
    }
    while (*int_cast_ptr != 0) {
        cast_offset = &(mem_chunk[*int_cast_ptr]);
        free_bytes = free_bytes + *(cast_offset + 2) - 12;
        if (*int_cast_ptr - *(cast_offset + 1) - *(int_cast_ptr + 2) > 0 && ok == 1) {
            free_blocks++;
        }
        int_cast_ptr = cast_offset;
        ok = 1;
    }
    cast_offset = &(mem_chunk[*(int_cast_ptr + 1)]);
    if (*(cast_offset + 1) + *(int_cast_ptr + 2) < chunk_size) {
        free_blocks++;
    }
    printf("%d blocks (%d bytes) free\n", free_blocks, chunk_size - free_bytes - 4 - (*alloc_blocks_ptr) * 12);
    return 0;
}

//Prints no of blocks and bytes used and their efficiency and fragmentation
int SHOW_USAGE(unsigned char* mem_chunk, int chunk_size, int* alloc_blocks_ptr)
{
    int *int_cast_ptr, used_bytes = 0, free_blocks = 0, i, *cast_offset, e, f, ok = 0;
    float E, F;
    int_cast_ptr = &(mem_chunk[0]);
    if (*int_cast_ptr > 4) {
        free_blocks++;
    }
    while (*int_cast_ptr != 0) {
        cast_offset = &(mem_chunk[*int_cast_ptr]);
        used_bytes = used_bytes + *(cast_offset + 2) - 12;
        if (*int_cast_ptr - *(cast_offset + 1) - *(int_cast_ptr + 2) > 0 && ok == 1) {
            free_blocks++;
        }
        int_cast_ptr = cast_offset;
        ok = 1;
    }
    cast_offset = &(mem_chunk[*(int_cast_ptr + 1)]);
    if (*(cast_offset + 1) + *(int_cast_ptr + 2) < chunk_size) {
        free_blocks++;
    }
    printf("%d blocks (%d bytes) used\n", *alloc_blocks_ptr, used_bytes);
    E = used_bytes;
    E = (E / (E + 4.0 + (*alloc_blocks_ptr) * 12)) * 100;
    e = E;
    printf("%d", e);
    printf("%% efficiency\n");
    F = free_blocks - 1.0;
    F = (F / (*alloc_blocks_ptr)) * 100;
    f = F;
    if (*alloc_blocks_ptr == 0) {
        printf("0");
        printf("%% fragmentation\n");
    }
    else {
        printf("%d", f);
        printf("%% fragmentation\n");
    }
    return 0;
}

//Prints all blocks, used and free,  and the totals
int SHOW_ALLOCATIONS(unsigned char* mem_chunk, int chunk_size)
{
    int *int_cast_ptr = &(mem_chunk[0]), i, *cast_offset, d;
    if (*int_cast_ptr == 0) {
        printf("OCCUPIED 4 bytes\nFREE %d bytes\n", chunk_size - 4);
    }
    else {
        i = *int_cast_ptr;
        printf("OCCUPIED 4 bytes\n");
        if ((*int_cast_ptr - 4) != 0) {
            printf("FREE %d bytes\n", *int_cast_ptr - 4);
        }
        int_cast_ptr = &mem_chunk[*int_cast_ptr];
        while (*int_cast_ptr != 0) {
            cast_offset = &(mem_chunk[*int_cast_ptr]);
            d = *int_cast_ptr - *(cast_offset + 1) - *(int_cast_ptr + 2);
            printf("OCCUPIED %d bytes\n", *(int_cast_ptr + 2));
            if (d > 0) {
                printf("FREE %d bytes\n", d);
            }
            i = *int_cast_ptr;
            int_cast_ptr = cast_offset;
        }
        printf("OCCUPIED %d bytes\nFREE %d bytes\n", *(int_cast_ptr + 2), chunk_size - i - *(int_cast_ptr + 2));
    }
    return 0;
}

int main()
{
    int chunk_size, i, exit = 1, SIZE, *alloc_blocks_ptr, alloc_blocks = 0, s, x;
    alloc_blocks_ptr = &alloc_blocks;

    char comm[30], command[30];


    //Command interpreter
    while (exit != 0) {
        scanf("%s", command);
        if (strcmp(command, "INITIALIZE") == 0) {
            scanf("%d", &chunk_size);
            INITIALIZE(&mem_chunk, chunk_size);
        }
        if (strcmp(command, "ALLOC") == 0) {
            scanf("%d", &SIZE);
            ALLOC(&mem_chunk, chunk_size, SIZE, alloc_blocks_ptr);
        }
        if (strcmp(command, "DUMP") == 0) {
            DUMP(&mem_chunk, chunk_size);
        }
        if (strcmp(command, "FREE") == 0) {
            scanf("%d", &i);
            FREE(&mem_chunk, i, alloc_blocks_ptr);
        }
        if (strcmp(command, "FINALIZE") == 0) {
            FILL(&mem_chunk, chunk_size, 0, 0, chunk_size);
            FINALIZE(mem_chunk);
            exit = 0;
        }
        if (strcmp(command, "SHOW") == 0) {
            scanf("%s", comm);
            if (strcmp(comm, "FREE") == 0) {
                SHOW_FREE(&mem_chunk, chunk_size, alloc_blocks_ptr);
            }
            if (strcmp(comm, "USAGE") == 0) {
                SHOW_USAGE(&mem_chunk, chunk_size, alloc_blocks_ptr);
            }
            if (strcmp(comm, "ALLOCATIONS") == 0) {
                SHOW_ALLOCATIONS(&mem_chunk, chunk_size);
            }
        }
        if (strcmp(command, "FILL") == 0) {
            scanf("%d %d %d", &i, &s, &x);
            FILL(&mem_chunk, chunk_size, i, x, s);
        }
    }
    return 0;
}