#ifndef __LINUXICE_UI__
#define __LINUXICE_UI__
#include <kernelfb.h>
#include <linux/list.h>

struct ice_listview_item{
	struct kfb_color fgcolor, bgcolor;
	char* text;
	struct list_head list;
	unsigned long extra;
};

struct ice_listview{
	struct list_head list_head;
	struct list_head* top_line;
	struct kfb_handle* handle;
	
	unsigned int max_line;
	unsigned int item_counts;
	
	struct{
		struct kfb_point left_top;
		struct kfb_point bottom_right;
	}list_rect;
};

struct ice_textedit{
	char text[256];
	int  next;
	int  cursor;
	struct kfb_handle* handle;
	struct kfb_color fgcolor, bgcolor;
	struct{
		struct kfb_point left_top;
		struct kfb_point bottom_right;
	}text_rect;
};

enum UI_{UI_OK, UI_FAILED};

int ui_init(void);
void ui_refresh(void);
void ui_clean(void);
void show_mainui(void);

void output_addline(const char* line);
void output_print(const char* fmt, ...);
const char* get_command(void);
void clear_command(void);


int ui_listview_new(struct ice_listview** hview, unsigned left, unsigned top, 
					unsigned right, unsigned bottom, unsigned max_line, struct kfb_handle* handle);
void ui_listview_additem(struct ice_listview* view, const char* text, unsigned long extra, 
				 struct kfb_color fg_color, struct kfb_color bg_color);
void ui_listview_refresh(struct ice_listview* view);
void ui_listview_scrolldown(struct ice_listview* view);
void ui_listview_clear(struct ice_listview* view);
int ui_listview_del(struct ice_listview** hview);

int ui_textedit_new(struct ice_textedit** hview, unsigned left, unsigned top, 
					unsigned right, unsigned bottom, struct kfb_color fg_color, struct kfb_color bg_color,struct kfb_handle* handle);
void ui_textedit_addchar(char ch, struct ice_textedit* view);
void ui_textedit_delchar(struct ice_textedit* view);
void ui_textedit_clear(struct ice_textedit* view);
void ui_textedit_refresh(struct ice_textedit* view);
int ui_textedit_del(struct ice_textedit** hview);

#endif
