#include "kernelfb.h"
#include "general.h"
#include "dbg_print.h"
void rgb888_32_set_pixel(unsigned int i, unsigned int j, struct kfb_color color, struct kfb_handle* handle);
void rgb888_32_get_pixel(unsigned int i, unsigned int j, struct kfb_color* color, struct kfb_handle* handle);
u_int32_t rgb888_32_color_to_uint32(struct kfb_color color);
struct kfb_color rgb888_32_uint32_to_color(u_int32_t color);

struct kfb_ops kfb_ops_rgb888_32 = {
	.set_pixel = rgb888_32_set_pixel,
	.get_pixel = rgb888_32_get_pixel,
	.color_to_uint32 = rgb888_32_color_to_uint32,
	.uint32_to_color = rgb888_32_uint32_to_color,
	.print_char = general_print_char,
	.print_str = general_print_str,
	.draw_rect = general_draw_rect,
	.draw_hline = general_draw_hline,
	.draw_vline = general_draw_vline,
	.fill_rect = general_fill_rect
};

void rgb888_32_set_pixel(unsigned int i, unsigned int j, struct kfb_color color, struct kfb_handle* handle){
	unsigned int index;
	u_int32_t icolor;
	
	if(i >= handle->vinfo.xres || j >= handle->vinfo.yres)
		return;
	
	index = (j * handle->vinfo.xres + i) * 4;
	
	if(index + 4 > handle->buffer_length){
		return;
	}
		
	icolor = rgb888_32_color_to_uint32(color);
	handle->buffer[index] = (unsigned char)((icolor >> 24) & 0x000000FF);
	handle->buffer[index + 1] = (unsigned char)((icolor >> 16) & 0x000000FF);
	handle->buffer[index + 2] = (unsigned char)((icolor >> 8) & 0x000000FF);
	handle->buffer[index + 3] = (unsigned char)(icolor & 0x000000FF);
	//kfb_printe("%X\n", *(u_int32_t*)&handle->buffer[index]);
}

void rgb888_32_get_pixel(unsigned int i, unsigned int j, struct kfb_color* color, struct kfb_handle* handle){
	unsigned int index;
	u_int32_t icolor = 0;
	
	if(i >= handle->vinfo.xres || j >= handle->vinfo.yres)
		return;
	
	index = (j * handle->vinfo.xres + i) * 4;
	
	if(index + 4 > handle->buffer_length){
		return;
	}
	icolor |= (handle->buffer[index] << 24) & 0xFF000000;
	icolor |= (handle->buffer[index + 1] << 16) & 0x00FF0000;
	icolor |= (handle->buffer[index + 2] << 8) & 0x0000FF00;
	icolor |= (handle->buffer[index + 3]) & 0x000000FF;
	
	*color = rgb888_32_uint32_to_color(icolor);
}

u_int32_t rgb888_32_color_to_uint32(struct kfb_color color){
	u_int32_t mask = 0x0000FF00;
	u_int32_t icolor = 0x00000000;
	icolor |= mask & (color.r << 8);
	mask <<= 8;
	icolor |= mask & (color.g << 16);
	mask <<= 8;
	icolor |= mask & (color.b << 24);
	return icolor;
}

struct kfb_color rgb888_32_uint32_to_color(u_int32_t color){
	struct kfb_color kcolor;
	u_int32_t mask = 0x000000FF;
	kcolor.r = (color >> 8) & mask;
	kcolor.g = (color >> 16) & mask;
	kcolor.b = (color >> 24) & mask;
	kcolor.a = 0;
	return kcolor;
}
