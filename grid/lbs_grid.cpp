#include <pthread.h>
#include "server/grid/lbs_defs.h"
#include "server/grid/lbs_queue.h"
#include "server/grid/lbs_hashtable.h"
#include <stdlib.h>
#include <stdio.h>
#include "server/grid/lbs_grid.h"

//网格初始化
int lbs_grid_init(lbs_grid_t* lbs_grid, double lon1, double lon2, double lat1, double lat2, int row_num, int col_num){
  lbs_grid->row_num = row_num;
  lbs_grid->col_num = col_num;
  lbs_grid->cell_width = (lon2 - lon1)/col_num;
	lbs_grid->cell_height = (lat2 - lat1)/row_num;
	lbs_grid->lon_min=lon1;
	lbs_grid->lat_min=lat1;
	lbs_hashtable_init(&lbs_grid->hash_table);
	lbs_grid->cell=(lbs_cell_t*)malloc((row_num*col_num)*sizeof(lbs_cell_t));
  int i = 0;
	for(i;i<row_num*col_num;i++){
		lbs_queue_init(&(lbs_grid->cell[i].dummy_node.queue));
  }
  return 0;
}
//网格的删除
int lbs_grid_destroy(lbs_grid_t* lbs_grid){
  return 0;
}
//更新移动位置
int lbs_grid_update(lbs_grid_t* lbs_grid, double lon, double lat, uint64_t timestamp, uint32_t id){
  int cell_row, cell_col, cell_id;
  cell_row = lbs_grid_cell_row(lbs_grid, lat);
  cell_col = lbs_grid_cell_col(lbs_grid, lon);
  cell_id = lbs_grid_cell_id(lbs_grid, cell_row, cell_col);
  lbs_hashnode_t* p = lbs_hashtable_get(&(lbs_grid->hash_table), id);
  if(p != NULL){
    p->mov_node->lon = lon;
    p->mov_node->lat = lat;
    p->mov_node->timestamp = timestamp;
    if(cell_id == p->cell_id){
      return 0;
    }
    else{
      lbs_queue_remove(&(p->queue));
      lbs_queue_insert_head(&(lbs_grid->cell[cell_id].dummy_node.queue), &(p->queue));
      p->cell_id = cell_id;
      return 0;
    }
  }
  else{
    lbs_mov_node_t* p0 = (lbs_mov_node_t*)malloc(sizeof(lbs_mov_node_t));
    p0->lon = lon;
    p0->lat = lat;
    p0->id = id;
    p0->timestamp = timestamp;
    lbs_queue_insert_head(&(lbs_grid->cell[cell_id].dummy_node.queue), &p0->queue);
    lbs_hashtable_set(&(lbs_grid->hash_table), id, p0, cell_id);
    return 0;

  }
}
//计算Cell Row
int lbs_grid_cell_row(lbs_grid_t* lbs_grid, double lat){
	int cell_row=(lat-lbs_grid->lat_min)/lbs_grid->cell_height;
	return cell_row;
}
//计算Cell Col
int lbs_grid_cell_col(lbs_grid_t* lbs_grid, double lon){
  int cell_col=(lon-lbs_grid->lon_min)/lbs_grid->cell_width;
	return cell_col;
}
//计算Cell id
int lbs_grid_cell_id(lbs_grid_t* lbs_grid, int cell_row, int cell_col){
  int cell_id = cell_row*lbs_grid->col_num + cell_col;
  return cell_id;
}
//计算row和col
void lbs_grid_cell_row_col(lbs_grid_t* lbs_grid, int cell_id, int* cell_row, int* cell_col){
  *cell_row = cell_id/lbs_grid->col_num;
  *cell_col = cell_id%lbs_grid->col_num;
}
//获取Cell id里面的Cell
lbs_cell_t * lbs_grid_cell(lbs_grid_t* lbs_grid, int cell_id){
   return &(lbs_grid->cell[cell_id]);
}
