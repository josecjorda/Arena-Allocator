// The MIT License (MIT)
// 
// Copyright (c) 2022 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//Jose Jordan 1001837992
#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
enum TYPE
{
    FREE = 0,
    USED
};

struct Node {
  size_t size;
  enum TYPE type;
  void * arena; 
  struct Node * next;
  struct Node * prev;
};

struct Node *alloc_list;
struct Node *previous_node;


void * arena;
//memoryUsed keeps track of all memory used that way I can add to arean if needed
//size_t memoryUsed;
enum ALGORITHM allocation_algorithm = FIRST_FIT;

int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
  arena = malloc( ALIGN4( size ) );
  
  allocation_algorithm = algorithm;

  alloc_list = ( struct Node * )malloc( sizeof( struct Node ));

  alloc_list -> arena = arena;
  alloc_list -> size  = ALIGN4(size);
  alloc_list -> type  = FREE;
  alloc_list -> next  = NULL;
  alloc_list -> prev  = NULL;

  previous_node  = alloc_list;

  return 0;
}

void mavalloc_destroy( )
{
  free( arena );
  
  // iterate over the linked list and free the nodes
  //temp node keeps track of next node. After node is freed node will be set to the next node using tempnode
  struct Node * tempnode =alloc_list;
  
  while( alloc_list != NULL )
  {
    tempnode = alloc_list;
    alloc_list = alloc_list->next;
    free(tempnode);
  }

  alloc_list = NULL;
  return;
}

void * mavalloc_alloc( size_t size )
{
  struct Node * node;
  /*memoryUsed += size;
  if(memoryUsed>=*(size_t*)arena)
  {
    arena = malloc(ALIGN4(*(size_t*)arena-memoryUsed));
  }*/
  if( allocation_algorithm != NEXT_FIT )
  { 
    node = alloc_list;
  }
  else if ( allocation_algorithm == NEXT_FIT )
  {
    node = previous_node;
  }
  else
  {
    printf("ERROR: Unknown allocation algorithm!\n");
    exit(0);
  }

  size_t aligned_size = ALIGN4( size );

  if( allocation_algorithm == FIRST_FIT && alloc_list!= NULL)
  {
    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;
  
        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;
  
        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
  
          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;
  
          node -> next = leftover_node;
          leftover_node->prev = node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
    }
  }
  else if( allocation_algorithm == NEXT_FIT && alloc_list!= NULL)// Implement Next Fit
  {
    //If spotfound is false it will tell me that a spot wasn't found after previous node and should loop 
    //back around to the beginning of the linked list
    bool spotfound = false;
    //If no spot found after searching from beginning, exit
    bool finished = false;
    while( node || spotfound == false || finished == false)
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        spotfound = true;
        int leftover_size = 0;
  
        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;
  
        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
  
          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;
  
          node -> next = leftover_node;
          leftover_node->prev = node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
      if(node == NULL && spotfound == false && finished == false)
      {
        node = alloc_list;
        finished = true;
      }
    }
    
  }
  else if( allocation_algorithm == BEST_FIT && alloc_list!= NULL)// Implement Best Fit
  {
    //best size will look for smallest possible size for allocated memory
    struct Node * bestsizednode = NULL;
    //in case theres no memory available
    bool foundOne = false;
    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        if(bestsizednode == NULL)
        {
          bestsizednode = node;
          foundOne = true;
        }
        if(node->size - aligned_size <= bestsizednode->size - aligned_size)
        {
          bestsizednode = node;
          foundOne = true;
        }
      }
      node = node -> next;
    }
    if(foundOne == true)
    {
      node = bestsizednode;
      int leftover_size = 0;

      node -> type  = USED;
      leftover_size = node -> size - aligned_size;
      node -> size =  aligned_size;
    
      if( leftover_size > 0 )
      {
        struct Node * previous_next = node -> next;
        struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
    
        leftover_node -> arena = node -> arena + size;
        leftover_node -> type  = FREE;
        leftover_node -> size  = leftover_size;
        leftover_node -> next  = previous_next;
    
        node -> next = leftover_node;
        leftover_node->prev = node;
      }
      previous_node = node;
      return ( void * ) node -> arena;
    } 
  }
  else if( allocation_algorithm == WORST_FIT && alloc_list!= NULL)// Implement Worst Fit, similar structure to best fit
  {
    struct Node * bestsizednode = NULL;
    //in case theres no memory available
    bool foundOne = false;
    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        if(bestsizednode == NULL)
        {
          bestsizednode = node;
          foundOne = true;
        }
        if(node->size - aligned_size >= bestsizednode->size - aligned_size)
        {
          bestsizednode = node;
          foundOne = true;
        }
      }
      node = node -> next;
    }
    if(foundOne == true)
    {
      node = bestsizednode;
      int leftover_size = 0;

      node -> type  = USED;
      leftover_size = node -> size - aligned_size;
      node -> size =  aligned_size;
    
      if( leftover_size > 0 )
      {
        struct Node * previous_next = node -> next;
        struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));
    
        leftover_node -> arena = node -> arena + size;
        leftover_node -> type  = FREE;
        leftover_node -> size  = leftover_size;
        leftover_node -> next  = previous_next;
    
        node -> next = leftover_node;
        leftover_node->prev = node;
      }
      previous_node = node;
      return ( void * ) node -> arena;
    } 
  }


  return NULL;
}

void mavalloc_free( void * ptr )
{
  struct Node * node = alloc_list; 
  while(node->arena != ptr)
  {
    node = node->next;
  }
  node->type = FREE;
  //memoryUsed -= node->size;
  //checking next node
  if(node->type == FREE && node->next != NULL && node->next->type == FREE)
  {
    node->size += node->next->size;
    node->next = node->next->next;
  }


  //checking prior node if prior node was the head then change current node to head
  if(node->type == FREE && node->prev != NULL && node->prev->type == FREE)
  {
    node->size += node->prev->size;
    node->prev = node->prev->prev;
    if(node->prev == NULL)
    {
      alloc_list = node;
    }
  }

  return;
}

int mavalloc_size( )
{
  //Just gets size of list
  int number_of_nodes = 0;
  struct Node * ptr = alloc_list;

  while( ptr != NULL)
  {
    number_of_nodes ++;
    ptr = ptr -> next; 
  }

  return number_of_nodes;
}