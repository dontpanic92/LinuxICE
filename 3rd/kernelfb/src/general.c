#include "kernelfb.h"
#include "general.h"
#include "pixel_font.h"
#include "dbg_print.h"

static void uintswap(unsigned int* x, unsigned int *y){
	unsigned int t;
	t = *x;
	*x = *y;
	*y = t;
}

struct kfb_point general_print_char(char c, unsigned int i, unsigned int j, 
							struct kfb_color fore_color, 
							struct kfb_color bg_color, 
							struct kfb_handle* handle){
	kfb_point_t point = {i + DEFAULT_FONT_WIDTH, j};
	int k, l;
	unsigned char mask;
	for(k = 0; k < 16; k++){
		mask = 0x01;
		for(l = 7; l >= 0; l--){
			if(mask & default_font[(unsigned char)c][k]){
				handle->ops->set_pixel(i + l, j + k, fore_color, handle);
			}else{
				handle->ops->set_pixel(i + l, j + k, bg_color, handle);
			}
			mask <<= 1;
			
		}
	}
	return point;
}
struct kfb_point general_print_str(const char* str, unsigned int x1, unsigned int x2,
								unsigned int y, 
								struct kfb_color fore_color, 
								struct kfb_color bg_color, 
								int wrap_text, unsigned int wrap_start_x,
								struct kfb_handle* handle){
	kfb_point_t point = {x1, y};
	unsigned int i, j;
	
	i = x1, j= y;

	if(!wrap_text)
		wrap_start_x = x1;
	else if(wrap_start_x > handle->vinfo.xres - 1)
		wrap_start_x = 0;
		
	//if(!wrap_text || wrap_end_x > handle->vinfo.xres - 1)
		//wrap_end_x = handle->vinfo.xres - 1;
	if(x2 > handle->vinfo.xres - 1)	
		x2 = handle->vinfo.xres - 1;
	
	while(*str != '\0'){
		if(j > handle->vinfo.yres)
			break;
			
		if(i > x2 || *str == '\n'){
			if(!wrap_text)
				break;
			i = wrap_start_x;
			j += DEFAULT_FONT_HEIGHT;
		}else if (*str == '\r'){
			if(wrap_text)
				i = wrap_start_x;
		}else{
			handle->ops->print_char(*str, i, j, fore_color, bg_color, handle);
			i += DEFAULT_FONT_WIDTH;
		}
		str++;
	}
	
	point.x = i, point.y = j;
	return point;
}

void general_draw_rect(unsigned int x1, unsigned int y1, unsigned int x2,unsigned int y2,
						struct kfb_color border_color, unsigned int border_width, struct kfb_color fill_color, 
						struct kfb_handle* handle){
	unsigned int tmp = (border_width - 1) / 2;
	
	if(x1 > x2)
		uintswap(&x1, &x2);
	if(y1 > y2)
		uintswap(&y1, &y2);
		
	handle->ops->draw_hline(x1 - tmp, x2 - tmp + border_width, y1, border_color, border_width, handle);
	handle->ops->draw_hline(x1 - tmp, x2 - tmp + border_width, y2, border_color, border_width, handle);
	handle->ops->draw_vline(x1, y1 - tmp + border_width + 1, y2 - tmp - 1, border_color, border_width, handle);
	handle->ops->draw_vline(x2, y1 - tmp + border_width + 1, y2 - tmp - 1, border_color, border_width, handle);
	
	handle->ops->fill_rect(x1 - tmp + border_width, y1 - tmp + border_width, x2 - tmp, y2 - tmp, fill_color, handle);
}
						
void general_fill_rect(unsigned int x1, unsigned int y1, unsigned int x2,unsigned int y2,
						struct kfb_color fill_color, 
						struct kfb_handle* handle){
	unsigned int i, j;
	
	if(x1 >= handle->vinfo.xres)
		x1 = handle->vinfo.xres - 1;
	if(x2 >= handle->vinfo.xres)
		x2 = handle->vinfo.xres - 1;
		
	if(y1 >= handle->vinfo.yres)
		y1 = handle->vinfo.yres - 1;
	if(y2 >= handle->vinfo.yres)
		y2 = handle->vinfo.yres - 1;
		
	if(x1 > x2)
		uintswap(&x1, &x2);
	if(y1 > y2)
		uintswap(&y1, &y2);
		
	for(i = x1; i <= x2; i++)
		for(j = y1; j <= y2; j++)
			handle->ops->set_pixel(i, j, fill_color, handle);
}
						
						
void general_draw_hline(unsigned int x1, unsigned int x2, unsigned int y,
						struct kfb_color line_color, unsigned int width,
						struct kfb_handle* handle){
	unsigned int tmp = (width - 1) / 2;
	handle->ops->fill_rect(x1, y - tmp, x2, y - tmp + width - 1, line_color, handle);
}

void general_draw_vline(unsigned int x, unsigned int y1, unsigned int y2,
						struct kfb_color line_color, unsigned int width,
						struct kfb_handle* handle){
	unsigned int tmp = (width - 1 ) / 2;
	handle->ops->fill_rect(x - tmp, y1, x - tmp + width - 1, y2, line_color, handle);
}
