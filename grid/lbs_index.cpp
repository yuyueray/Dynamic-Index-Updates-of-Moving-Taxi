#include "server/grid/lbs_index.h"

#include <stdio.h>
#include <stdlib.h>

#include "common/log.h"
#include "lbs_grid.h"
#include "server/grid/lbs_distance.h"

#include "lbs_bitmap.cpp"
#include "lbs_nnheap.cpp"

#define LBS_LON_MIN 116
#define LBS_LON_MAX 117
#define LBS_LAT_MIN 39
#define LBS_LAT_MAX 41

#define LBS_ROW_NUM 200
#define LBS_COL_NUM 100

static lbs_grid_t lbs_grid;

// 初始化网格索引
int lbs_grid_index_init() {
  // TODO: by student
  lbs_grid_init(&lbs_grid, LBS_LON_MIN, LBS_LON_MAX, LBS_LAT_MIN, LBS_LAT_MAX, LBS_ROW_NUM, LBS_COL_NUM);
  return 0;
}

// 更新接口[出租车位置更新]
int lbs_grid_index_update(double lon,
                           double lat,
                           uint64_t timestamp,
                           uint32_t id) {
  // TODO: by student
  lbs_grid_update(&lbs_grid, lon, lat, timestamp, id);
  return 0;
}

// 范围查询接口[查询某一范围内的所有出租车信息]
int lbs_grid_index_range_query(double lon1,
                               double lon2,
                               double lat1,
                               double lat2,
                               lbs_res_node_t* out) {
  // TODO: by student
  int i, j, cell_row1, cell_row2, cell_col1, cell_col2, cell_id;
  cell_row1 = lbs_grid_cell_row(&lbs_grid, lat1);
  cell_row2 = lbs_grid_cell_row(&lbs_grid, lat2);
  cell_col1 = lbs_grid_cell_col(&lbs_grid, lon1);
  cell_col2 = lbs_grid_cell_col(&lbs_grid, lon2);
  lbs_mov_node_t *mov_node, *head_node;
  lbs_res_node_t *out_node;
  for(i = cell_row1; i <= cell_row2; i++){
    for(j = cell_col1; j <= cell_col2; j++){
      cell_id = lbs_grid_cell_id(&lbs_grid, i, j);
      head_node = &(lbs_grid.cell[cell_id].dummy_node);
      mov_node = (lbs_mov_node_t*)head_node->queue.next;


      if(i == cell_row1 || i == cell_row2 || j == cell_col1 || j == cell_col2){
        while(mov_node != head_node){
          if(mov_node->lon <= lon2 && mov_node->lon >= lon1 && mov_node->lat >= lat1 && mov_node->lat <= lat2){

            out_node = (lbs_res_node_t*)malloc(sizeof(lbs_res_node_t));
            out_node->lon = mov_node->lon;
            out_node->lat = mov_node->lat;
            lbs_queue_insert_head(&(out->queue), &(out_node->queue));
          }
            mov_node = (lbs_mov_node_t*)mov_node->queue.next;
        }
      }
      else{
        while(mov_node != head_node){
            out_node = (lbs_res_node_t*)malloc(sizeof(lbs_res_node_t));
            out_node->lon = mov_node->lon;
            out_node->lat = mov_node->lat;
          lbs_queue_insert_head(&(out->queue), &(out_node->queue));
          mov_node = (lbs_mov_node_t*)mov_node->queue.next;
        }

      }
    }
  }
  return 0;
}

inline double lbs_cell_distance(int cell_id, double lon, double lat){
 int lon_max, lat_max, lon_min, lat_min, cell_row, cell_col;

 lbs_grid_cell_row_col(&lbs_grid, cell_id, &cell_row, &cell_col);
 lon_min = cell_col*lbs_grid.cell_width+lbs_grid.lon_min;
 lon_max = lon_min+lbs_grid.cell_width;
 lat_min = cell_row*lbs_grid.cell_height+lbs_grid.lat_min;
 lat_max = lat_min+lbs_grid.cell_height;
 if(lon_min>=lon&&lat_min>=lat){
   return lbs_distance(lon_min, lat_min, lon, lat);
 }
 else if(lon_min<lon&&lon_max>lon&&lat<lat_min){
   return lbs_distance(lon, lat, lon, lat_min);
 }
 else if(lat_min>=lat&&lon_max<=lon){
   return lbs_distance(lon, lat, lon_max, lat_min);
 }
 else if(lat_min<lat&&lat_max>lat&&lon_max<lon){
   return lbs_distance(lon, lat, lon_max, lat);
 }
 else if(lat_max<=lat&&lon_max<=lon) {
   return lbs_distance(lon, lat, lon_max, lat_max);
 }
 else if(lon_min<lon&&lon_max>lon&&lat>lat_max){
   return lbs_distance(lon, lat, lon, lat_max);
 }
 else if(lon<=lon_min&&lat>=lat_max){
   return lbs_distance(lon, lat, lon_min, lat_max);
 }
 else if(lat<lat_max&&lat>lat_min&&lon<lon_min){
   return lbs_distance(lon, lat, lon_min, lat);
 }
 else{
   return 0;
 }
}


// NN查询接口[查询离lon,lat最近的出租车]
int lbs_grid_index_nn_query(double lon, double lat, lbs_res_node_t* out) {
  // TODO: by student
  lbs_bitmap_t map;
  lbs_nnheap_t heap;
  lbs_bitmap_init(&map, LBS_ROW_NUM*LBS_COL_NUM);
  lbs_nnheap_init(&heap);
  int cell_row=lbs_grid_cell_row(&lbs_grid,lat);
	int cell_col=lbs_grid_cell_col(&lbs_grid,lon);
	int cell_id=lbs_grid_cell_id(&lbs_grid, cell_row, cell_col);
	lbs_cell_t* cell0=lbs_grid_cell(&lbs_grid,cell_id);
	lbs_bitmap_setbit(&map, cell_id);
	lbs_nnheap_insert(&heap,&cell0->dummy_node,cell_id,1,0);

  while (lbs_nnheap__top(&heap)->is_grid == 1) {
    /* code */
    lbs_heapnode_t* peak = lbs_nnheap__top(&heap);
    int cid = peak->cell_id;
    lbs_nnheap_pop(&heap);
    lbs_mov_node_t* cdummy = &(lbs_grid.cell[cid].dummy_node);
    lbs_mov_node_t* cnode = (lbs_mov_node_t*)cdummy->queue.next;

    while (cnode != cdummy) {
      int distance = lbs_distance(lon, lat, cnode->lon, cnode->lat);
      lbs_nnheap_insert(&heap, cnode, cid, 0, distance);
      cnode = (lbs_mov_node_t*)cnode->queue.next;
    }

    int i = 0;
    int cell_num = LBS_COL_NUM*LBS_ROW_NUM;
    int idarr[8] = {cid-LBS_COL_NUM-1, cid-LBS_COL_NUM, cid-LBS_COL_NUM+1, cid-1, cid+1, cid+LBS_COL_NUM-1, cid+LBS_COL_NUM, cid+LBS_COL_NUM+1};
    for(;i < 8; i++){
      int num = idarr[i];
      if(num<cell_num && num>0 && lbs_bitmap_isset(&map, num) == 0){
        lbs_cell_t* ccell = lbs_grid_cell(&lbs_grid, num);
        lbs_nnheap_insert(&heap, &(ccell->dummy_node), num, 0, lbs_cell_distance(num, lon, lat));
        lbs_bitmap_setbit(&map, num);
      }
    }

  }

  lbs_res_node_t* ret = (lbs_res_node_t*)malloc(sizeof(lbs_res_node_t));
  lbs_mov_node_t* top = lbs_nnheap__top(&heap)->node;
  ret->lon = top->lon;
  ret->lat = top->lat;
  ret->id = top->id;
  ret->timestamp = top->timestamp;
  lbs_queue_insert_head(&(out->queue), &(ret->queue));
  lbs_bitmap_destroy(&map);
  return 0;
}
