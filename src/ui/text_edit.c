#include <linuxice.h>
#include <ui.h>
#include <input.h>

int ui_textedit_new(struct ice_textedit** hview, unsigned left, unsigned top,
                    unsigned right, unsigned bottom, struct kfb_color fg_color, struct kfb_color bg_color, struct kfb_handle* handle) {
    struct ice_textedit *view;

    if(!hview)
        return UI_FAILED;

    view = kmalloc(sizeof(struct ice_textedit), GFP_ATOMIC);

    if(!view)
        return UI_FAILED;

    view->text_rect.left_top.x = left;
    view->text_rect.left_top.y = top;
    view->text_rect.bottom_right.x = right;
    view->text_rect.bottom_right.y = bottom;

    view->handle = handle;
    view->fgcolor = fg_color;
    view->bgcolor = bg_color;

    view->next = 0;
    view->cursor = 0;
    view->text[0] = '\0';

    *hview = view;

    return UI_OK;
}

void ui_textedit_refresh(struct ice_textedit* view) {
    view->handle->ops->fill_rect(view->text_rect.left_top.x, view->text_rect.left_top.y, view->text_rect.bottom_right.x,
                                 view->text_rect.bottom_right.y, view->bgcolor, view->handle);
    view->handle->ops->print_str(view->text, view->text_rect.left_top.x, view->text_rect.bottom_right.x, view->text_rect.left_top.y,
                                 view->fgcolor, view->bgcolor, 0, 0, view->handle);
    view->handle->ops->print_char(view->text[view->cursor], view->text_rect.left_top.x + view->cursor * get_current_font_width(), view->text_rect.left_top.y, view->bgcolor, view->fgcolor, view->handle);
}

void ui_textedit_addchar(char ch, struct ice_textedit* view) {
    if(view->next == 255 || (view->next + 1) * get_current_font_width() > view->text_rect.bottom_right.x - view->text_rect.left_top.x) {
        return;
    }

    view->text[view->next++] = ch;
    view->text[view->next] = '\0';
    view->cursor = view->next;
}

void ui_textedit_delchar(struct ice_textedit* view) {
    if(view->next == 0) {
        return;
    }

    view->text[--view->next] = '\0';
    view->cursor = view->next;
}

void ui_textedit_clear(struct ice_textedit* view) {
    view->next = 0;
    view->cursor = 0;
    view->text[0] = '\0';
}

int ui_textedit_del(struct ice_textedit** hview) {
    if(!hview || !*hview)
        return UI_FAILED;


    kfree(*hview);
    *hview = NULL;
    return UI_OK;
}
