#include <pthread.h>
#include <stdlib.h>
#include "server/grid/lbs_defs.h"
#include "server/grid/lbs_hashtable.h"
#include "server/grid/lbs_queue.h"


#define LENGTH 20000

int lbs_hashtable_init(lbs_hashtable_t* lbs_hashtable){
  lbs_hashtable->size = 0;
  lbs_hashtable->capacity = LENGTH;
  lbs_hashtable->hash_nodes = (lbs_hashnode_t*)malloc(lbs_hashtable->capacity*sizeof(lbs_hashnode_t));
  int i = 0;
  for (i; i<lbs_hashtable->capacity; i++){
    lbs_queue_init(&(lbs_hashtable->hash_nodes[i].queue));
  }
  return 0;
}

int lbs_hashtable_destroy(lbs_hashtable_t* lbs_hash_table){
  return 0;
}

int lbs_hashtable_set(lbs_hashtable_t* lbs_hash_table, uint32_t id, lbs_mov_node_t* lbs_mov_node, int cell_id){
  int hashnum = id%lbs_hash_table->capacity;
  lbs_hashnode_t* nhn = (lbs_hashnode_t*)malloc(sizeof(lbs_hashnode_t));
  nhn->mov_node = lbs_mov_node;
  nhn->cell_id = cell_id;
  lbs_queue_insert_head(&(lbs_hash_table->hash_nodes[hashnum].queue), &(nhn->queue));
	lbs_hash_table->size++;
	return 0;
}

lbs_hashnode_t* lbs_hashtable_get(lbs_hashtable_t* lbs_hash_table, uint32_t id){
  int hashnum = id%lbs_hash_table->capacity;
  lbs_hashnode_t *loc = &(lbs_hash_table->hash_nodes[hashnum]);
  lbs_hashnode_t *ret = (lbs_hashnode_t*)loc->queue.next;
  while(ret != loc){
    if(ret->mov_node->id == id){
      return ret;
    }
    ret = (lbs_hashnode_t*)ret->queue.next;
  }
  return NULL;
}
