#include "server/grid/lbs_bitmap.h"
#include <string.h>
#define EIGHT 3
#define MASK 0x07

/**初始化 Bitmap **/
int lbs_bitmap_init(lbs_bitmap_t* lbs_bitmap, uint32_t bits_num){
  lbs_bitmap->bits_num = bits_num/8 + 1;
	lbs_bitmap->bits = (uint8_t *)malloc(lbs_bitmap->bits_num*sizeof(uint8_t));
  return 0;
}
/**销毁**/
int lbs_bitmap_destroy(lbs_bitmap_t* lbs_bitmap){
  lbs_bitmap->bits_num = 0;
  free(lbs_bitmap->bits);
	return 0;
}
/**设置Bit**/
int lbs_bitmap_setbit(lbs_bitmap_t* lbs_bitmap, uint32_t pos){
  lbs_bitmap->bits[pos >> EIGHT] |= (1 << (7- pos & MASK));
  return 0;
}
/**取消设置**/
int lbs_bitmap_unsetbit(lbs_bitmap_t* lbs_bitmap, uint32_t pos){
  lbs_bitmap->bits[pos >> EIGHT] &= ~(1 << (7- pos & MASK));
  return 0;
}
/**判定是否设置Bit**/
int lbs_bitmap_isset(lbs_bitmap_t* lbs_bitmap, uint32_t pos){
  return lbs_bitmap->bits[pos >> EIGHT] & (1 << (7- pos & MASK));
  //if iset == true then return 1
}
