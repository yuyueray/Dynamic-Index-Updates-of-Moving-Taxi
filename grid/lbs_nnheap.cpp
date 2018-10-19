#include "server/grid/lbs_nnheap.h"
#include "lbs_defs.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

#define LENGTH 20000


/**初始化**/
int lbs_nnheap_init(lbs_nnheap_t* lbs_nnheap){
  lbs_nnheap->capacity = LENGTH;
  lbs_nnheap->size = 0;
  lbs_nnheap->heap_nodes = (lbs_heapnode_t*)malloc(lbs_nnheap->capacity*sizeof(lbs_heapnode_t));
  return 0;
}
/**销毁**/
int lbs_nnheap_destroy(lbs_nnheap_t* lbs_nnheap){
  return 0;
}
/**插入**/
int lbs_nnheap_insert(lbs_nnheap_t* lbs_nnheap,lbs_mov_node_t* lbs_mov_node,int cell_id,uint8_t is_grid,double distance){
  lbs_nnheap->size++;
  int child = lbs_nnheap->size;
  int parent = child/2;
  lbs_heapnode_t h, temp;
  h.distance = distance;
  h.is_grid = is_grid;
  h.cell_id = cell_id;
  h.node = lbs_mov_node;
  lbs_nnheap->heap_nodes[lbs_nnheap->size]=h;
  while(child >= 1){
    if(lbs_nnheap->heap_nodes[child].distance < lbs_nnheap->heap_nodes[parent].distance){
      temp = lbs_nnheap->heap_nodes[child];
      lbs_nnheap->heap_nodes[child] = lbs_nnheap->heap_nodes[parent];
      lbs_nnheap->heap_nodes[parent] = temp;
    }
    child = parent;
    parent /= 2;
  }
  return 0;
}
/**获取离distance最小的lbs_nnheap_t***/
lbs_heapnode_t* lbs_nnheap__top(lbs_nnheap_t* lbs_nnheap){
  return lbs_nnheap->heap_nodes + 1;
}
/**删除堆顶元素**/
void lbs_nnheap_pop(lbs_nnheap_t* lbs_nnheap){
  lbs_nnheap->heap_nodes[1] = lbs_nnheap->heap_nodes[lbs_nnheap->size];
  lbs_nnheap->size--;
  int parent = 1;
  int lchild = 2*parent;
  int rchild = 2*parent + 1;
  int size = lbs_nnheap->size;
  lbs_heapnode_t temp;
  while(lchild <= size){
    if(rchild > size){
      if(lbs_nnheap->heap_nodes[lchild].distance < lbs_nnheap->heap_nodes[parent].distance){
        temp = lbs_nnheap->heap_nodes[lchild];
        lbs_nnheap->heap_nodes[lchild] = lbs_nnheap->heap_nodes[parent];
        lbs_nnheap->heap_nodes[parent] = temp;
      }
      break;
    }
    else{
      if(lbs_nnheap->heap_nodes[lchild].distance < lbs_nnheap->heap_nodes[rchild].distance){
        temp = lbs_nnheap->heap_nodes[lchild];
        lbs_nnheap->heap_nodes[lchild] = lbs_nnheap->heap_nodes[parent];
        lbs_nnheap->heap_nodes[parent] = temp;
        parent = lchild;
        lchild = 2*parent;
        rchild = 2*parent + 1;
      }
      else{
        temp = lbs_nnheap->heap_nodes[rchild];
        lbs_nnheap->heap_nodes[rchild] = lbs_nnheap->heap_nodes[parent];
        lbs_nnheap->heap_nodes[parent] = temp;
        parent = rchild;
        lchild = 2*parent;
        rchild = 2*parent + 1;
      }
    }
  }
}
