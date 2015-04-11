#ifndef KFB_KERNEL_FB_H
#define KFB_KERNEL_FB_H

#include <linux/fb.h>
#include <linux/syscalls.h>
#include <linux/types.h>

#define KFB_OPS_MAX 4

struct kfb_handle;

struct kfb_color{
	u_int8_t r, g, b, a;
};
typedef struct kfb_color kfb_color_t;

struct kfb_point{
	unsigned int x, y;
};
typedef struct kfb_point kfb_point_t;


struct kfb_ops{
	void (*set_pixel)(unsigned int i, unsigned int j, struct kfb_color color, struct kfb_handle* handle);
	
	void (*get_pixel)(unsigned int i, unsigned int j, struct kfb_color* color, struct kfb_handle* handle);
	
	u_int32_t (*color_to_uint32)(struct kfb_color color);
	
	struct kfb_color (*uint32_to_color)(u_int32_t color);
	
	struct kfb_point (*print_char)(char c, unsigned int i, unsigned int j, 
								struct kfb_color fore_color, 
								struct kfb_color bg_color, 
								struct kfb_handle* handle);
	struct kfb_point (*print_str)(const char* str, unsigned int x1, unsigned int x2, 
								unsigned int y, 
								struct kfb_color fore_color, 
								struct kfb_color bg_color, 
								int wrap_text, unsigned int wrap_start_x,
								struct kfb_handle* handle);
	void (*draw_rect)(unsigned int x1, unsigned int y1, unsigned int x2,unsigned int y2,
						struct kfb_color border_color, unsigned int border_width, struct kfb_color fill_color, 
						struct kfb_handle* handle);
	void (*fill_rect)(unsigned int x1, unsigned int y1, unsigned int x2,unsigned int y2,
						struct kfb_color fill_color, 
						struct kfb_handle* handle);
	void (*draw_hline)(unsigned int x1, unsigned int x2, unsigned int y,
						struct kfb_color line_color, unsigned int width,
						struct kfb_handle* handle);
	void (*draw_vline)(unsigned int x, unsigned int y1, unsigned int y2,
						struct kfb_color line_color, unsigned int width,
						struct kfb_handle* handle);
};

struct kfb_handle{
	//struct file* fp;
    struct fb_info* fb;
	unsigned char* buffer;
	unsigned long buffer_length;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	struct kfb_ops* ops;
};

typedef struct kfb_handle kfb_handle_t;

enum kfb_result{KFB_OK, KFB_FAILED};
typedef enum kfb_result kfb_result_t;

extern struct kfb_ops* kfb_ops_var[KFB_OPS_MAX];

kfb_result_t kfb_create(const char* dev_path, kfb_handle_t ** kfb_handle);
kfb_result_t kfb_delete(kfb_handle_t ** kfb_handle);
kfb_result_t kfb_flush(kfb_handle_t* kfb_handle);

static inline struct kfb_color MAKE_COLOR(u_int8_t r, u_int8_t g, u_int8_t b){
	struct kfb_color k = {r, g, b};
	return k;
}

#define DEFAULT_FONT_WIDTH	8
#define DEFAULT_FONT_HEIGHT	16


static inline unsigned int get_current_font_width(void){return DEFAULT_FONT_WIDTH;}
static inline unsigned int get_current_font_height(void){return DEFAULT_FONT_HEIGHT;}

#endif
