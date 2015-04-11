#include <ui.h>
#include <linux/module.h>

int ui_listview_new(struct ice_listview** hview, unsigned left, unsigned top,
                    unsigned right, unsigned bottom, unsigned max_line, struct kfb_handle* handle) {
    struct ice_listview* view;

    if(!hview)
        return UI_FAILED;

    view = kmalloc(sizeof(struct ice_listview), GFP_ATOMIC);

    if(!view)
        return UI_FAILED;

    view->list_rect.left_top.x = left;
    view->list_rect.left_top.y = top;
    view->list_rect.bottom_right.x = right;
    view->list_rect.bottom_right.y = bottom;

    INIT_LIST_HEAD(&view->list_head);

    view->top_line = &view->list_head;

    view->max_line = (bottom - top) / get_current_font_height() + 1;
    view->handle = handle;
	view->item_counts = 0;
	
    *hview = view;

    return UI_OK;
}

void ui_listview_additem(struct ice_listview* view, const char* text, unsigned long extra,
                     struct kfb_color fg_color, struct kfb_color bg_color) {
    struct ice_listview_item* citem = kmalloc(sizeof(struct ice_listview_item), GFP_ATOMIC);

    if(view == NULL) {
        printk(KERN_ALERT "LinuxICE: ui_add_item params error\n");
        kfree(citem);
        return;
    }

    if(view->top_line == NULL)
        view->top_line = view->list_head.next;

    if(text == NULL) {
        printk(KERN_ALERT "LinuxICE: ui_add_item params error\n");
        kfree(citem);
        return;
    }

    citem->text = kmalloc(strlen(text) + 1, GFP_ATOMIC);

    if(citem->text == NULL) {
        printk(KERN_ALERT "LinuxICE: ui_add_item cannot alloc memory\n");
        kfree(citem);
        return;
    }

    strcpy(citem->text, text);
    citem->fgcolor = fg_color;
    citem->bgcolor = bg_color;
    citem->extra = extra;
    list_add_tail(&citem->list, &view->list_head);
    view->item_counts++;
}

void ui_listview_refresh(struct ice_listview* view) {
    struct ice_listview_item *v, *n;
    unsigned int y = view->list_rect.left_top.y;
    unsigned int x = view->list_rect.left_top.x;
    list_for_each_entry_safe(v, n, view->top_line/*&view->list_head*/, list) {
        view->handle->ops->fill_rect(view->list_rect.left_top.x, y, view->list_rect.bottom_right.x,
                                     y + get_current_font_height(), v->bgcolor, view->handle);
        view->handle->ops->print_str(v->text, x, view->list_rect.bottom_right.x, y,
                                     v->fgcolor, v->bgcolor, 0, 0, view->handle);
        y += get_current_font_height();

        if(y + get_current_font_height() > view->list_rect.bottom_right.y) {
            break;
        }
    }
}

void ui_listview_scrolldown(struct ice_listview* view){
	unsigned int s = 0;
	struct ice_listview_item *v, *n;
	
	list_for_each_entry_safe(v, n, view->top_line, list) {
       s++;
    }
	
	if(s > (view->list_rect.bottom_right.y - view->list_rect.left_top.y) / get_current_font_height()){
		view->top_line = view->top_line->next;
	}
}

void ui_listview_clear(struct ice_listview* view) {
    struct ice_listview_item *v, *n;

    if(!view)
        return;

    list_for_each_entry_safe(v, n, &view->list_head, list) {
        list_del(&v->list);
        kfree(v->text);
        kfree(v);
    }
}

int ui_listview_del(struct ice_listview** hview) {
    struct ice_listview_item *v, *n;

    if(!hview || !*hview)
        return UI_FAILED;

    list_for_each_entry_safe(v, n, &(*hview)->list_head, list) {
        list_del(&v->list);
        kfree(v->text);
        kfree(v);
    }
    kfree(*hview);
    *hview = NULL;
    return UI_OK;
}
