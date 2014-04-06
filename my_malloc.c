#include <stdio.h>

// global variables
struct s_block
{
	size_t size;
	int free;
	struct s_block *next;
	struct s_block *prev;
	char data[1]; // points to actual data returned to user
};

// BLOCK SIZE contains malloc metadata
// char data[1] in struct s_block, points to user requested allocation. 
// It will have a size of 4 because of padding. Hence we delete
// 4 from the sizeof struct
#define BLOCK_SIZE (sizeof(struct s_block)-4)

// align memory requested to 4 bytes
// ((x-1)/4)*4 + 4
#define align(x)  ((((x-1)>>2)<<2)+4)

// points to start of user's heap memory
void *base = NULL;

// extend heap
struct s_block * extend_heap(struct s_block *last, size_t sz);

// allocate_block
struct s_block * allocate_block(struct s_block *last, size_t sz);

// split_block
void split_block(struct s_block *last, size_t sz);

// malloc
void* my_malloc(size_t sz);

// free
void my_free(void *fr);

// join adjacent free blocks
void fuse_block(struct s_block *last);

struct s_block * extend_heap(struct s_block *last, size_t sz)
{
	struct s_block *temp;
	temp = sbrk(0);
	if ( sbrk(BLOCK_SIZE + sz) == ((void*)-1) ) // all memory exhausted
		return NULL;

	temp->size = sz;
	temp->free = 0;
	temp->next = NULL;
	temp->prev = last;
	if (last)
		last->next = temp;

	return temp;
}

void split_block(struct s_block *last, size_t sz)
{
	struct s_block *new_block;

	if ( (last->size - sz) >= (BLOCK_SIZE + 4) ) // if left over memory big enough, then split
	{
		new_block = last->data + sz;
		new_block->size = last->size - sz - BLOCK_SIZE;
		new_block->free = 1;
		new_block->next = last->next;
		new_block->prev = last;

		last->next = new_block;
		last->size = sz;
	}
}

struct s_block * allocate_block(struct s_block *last, size_t sz)
{
	struct s_block *temp, *prev;
	temp = last;
	prev = NULL;
	while(temp)
	{
		if ( temp->free == 1 && temp->size >= sz )
		{
			split_block(temp, sz); // call split block, split in case large memory
			temp->free = 0;
			return temp;
		}

		prev = temp;
		temp = temp->next;
	}

	return extend_heap(prev, sz); // if no free block large enough found, call extend heap
}

void* my_malloc(size_t sz)
{
	struct s_block *temp;
	size_t sz_align = align(sz);

	if ( base )
	{
		temp = allocate_block(base, sz_align);
	}
	else
	{
		temp = extend_heap(base, sz_align);
		base = temp; // initialize base pointer
	}

	if ( temp )
		return temp->data; // data points to user requested space
	else
		return NULL;
}

void fuse_block(struct s_block *last)
{
	struct s_block *temp;

	// fuse the next blocks is free
	temp = last->next;
	while(temp)
	{
		if ( temp->free == 1 )
		{
			last->size += BLOCK_SIZE + (last->next)->size;
			last->next = temp->next;

			temp = last->next;
		}
		else
			break;
	}

	// fuse the prev blocks is free
	temp = last->prev;
	while(temp)
	{
		if ( temp->free == 1 )
		{
			temp->size += BLOCK_SIZE + last->size;
			temp->next = last->next;
			last = temp;

			temp = last->prev;
		}
		else
			break;
	}
}

void my_free(void *fr)
{
	struct s_block *temp;

	// point to start of meta info
	temp = (struct s_block *)((char *)fr - BLOCK_SIZE);
	temp->free = 1;

	// fuse blocks if possible
	fuse_block(temp);
}
