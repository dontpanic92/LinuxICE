#ifndef KFB_GENERAL_H
#define KFB_GENERAL_H
struct kfb_point general_print_char(char c, unsigned int i, unsigned int j, 
							struct kfb_color fore_color, 
							struct kfb_color bg_color, 
							struct kfb_handle* handle);
							
struct kfb_point general_print_str(const char* str, unsigned int x1, unsigned int x2, 
								unsigned int y, 
								struct kfb_color fore_color, 
								struct kfb_color bg_color, 
								int wrap_text, unsigned int wrap_start_x,
								struct kfb_handle* handle);
							
void general_draw_rect(unsigned int x1, unsigned int y1, unsigned int x2,unsigned int y2,
						struct kfb_color border_color, unsigned int border_width, struct kfb_color fill_color, 
						struct kfb_handle* handle);
						
void general_fill_rect(unsigned int x1, unsigned int y1, unsigned int x2,unsigned int y2,
						struct kfb_color fill_color, 
						struct kfb_handle* handle);
						
void general_draw_hline(unsigned int x1, unsigned int x2, unsigned int y,
						struct kfb_color line_color, unsigned int width,
						struct kfb_handle* handle);

void general_draw_vline(unsigned int x, unsigned int y1, unsigned int y2,
						struct kfb_color line_color, unsigned int width,
						struct kfb_handle* handle);
#endif
