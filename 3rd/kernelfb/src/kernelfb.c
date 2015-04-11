
#include "kernelfb.h"
#include "dbg_print.h"
#include <asm/uaccess.h>
#include <linux/fb.h>

static struct kfb_ops* get_proper_ops(kfb_handle_t *handle){
	switch(handle->fb->var.bits_per_pixel){
	case 32:
		return kfb_ops_var[1];
		break;
	case 4:
		if(handle->fb->var.xres == handle->fb->fix.line_length * 8){ //1bit per pixel
			handle->fb->var.bits_per_pixel = 1;
			return kfb_ops_var[0];
		}
	default:
		return NULL;
	}
}

kfb_result_t kfb_create(const char* dev_path, kfb_handle_t ** kfb_handle){
	kfb_handle_t *handle;
	mm_segment_t old_fs;
    int i;
	
	if(!kfb_handle)
		return KFB_FAILED;
	
	handle = kmalloc(sizeof(kfb_handle_t), GFP_ATOMIC);
	memset(handle, 0, sizeof(kfb_handle_t));
    
    for (i = 0; i < FB_MAX; i++){
        if (registered_fb[i] && MINOR(registered_fb[i]->dev->devt) == 0){
            handle->fb = registered_fb[i];
            break;
        }
    }
    
    if(NULL == (handle->ops = get_proper_ops(handle))){
		kfb_printe("Failed to find a proper handler\n");
		goto FAILED2;
	}
    
    handle->finfo = handle->fb->fix;
    handle->vinfo = handle->fb->var;
    
	handle->buffer_length = handle->fb->var.xres * handle->fb->var.yres / 8 * handle->fb->var.bits_per_pixel;
	handle->buffer_length = (handle->buffer_length > handle->fb->fix.smem_len) ? handle->fb->fix.smem_len : handle->buffer_length;
	handle->buffer = kmalloc(handle->buffer_length, GFP_ATOMIC);
	/*handle->fp = filp_open(dev_path, O_RDWR, 0);

    if (IS_ERR(handle->fp)){  
		kfb_printe("Failed to open device: %s\n", dev_path);
		goto FAILED;
	}

	old_fs = get_fs();
	set_fs(get_ds());

	if (handle->fp->f_op->unlocked_ioctl(handle->fp, FBIOGET_FSCREENINFO, (unsigned long)&handle->finfo)){  
		kfb_printe("Failed to get fix_screeninfo\n");
	    goto FAILED2;
	}  
          
	if (handle->fp->f_op->unlocked_ioctl(handle->fp, FBIOGET_VSCREENINFO, (unsigned long)&handle->vinfo)){  
		kfb_printe("Failed to get car_screeninfo\n");  
		goto FAILED2;
	}

	if(NULL == (handle->ops = get_proper_ops(handle))){
		kfb_printe("Failed to find a proper handler\n");
		goto FAILED2;
	}
	
	handle->buffer_length = handle->vinfo.xres * handle->vinfo.yres / 8 * handle->vinfo.bits_per_pixel;
	handle->buffer_length = (handle->buffer_length > handle->finfo.smem_len) ? handle->finfo.smem_len : handle->buffer_length;
	handle->buffer = kmalloc(handle->buffer_length, GFP_ATOMIC);
	
	set_fs(old_fs);
    */
	*kfb_handle = handle;
	return KFB_OK;
	
FAILED2:
	//set_fs(old_fs);
	//filp_close(handle->fp, NULL);
FAILED:
	kfree(handle);
	*kfb_handle = NULL;
	return KFB_FAILED;
}

kfb_result_t kfb_delete(kfb_handle_t ** kfb_handle){
	if(!kfb_handle || !*kfb_handle)
		return KFB_FAILED;
	
	if((*kfb_handle)->buffer){
		kfree((*kfb_handle)->buffer);
	}
	
	kfree(*kfb_handle);
	*kfb_handle = NULL;
	return KFB_OK;
}

kfb_result_t kfb_flush(kfb_handle_t* kfb_handle){
	mm_segment_t old_fs;
	unsigned int i;
	unsigned int l;
	loff_t pos;
    
	if(!kfb_handle)
		return KFB_FAILED;
	
    l = kfb_handle->fb->var.xres * kfb_handle->fb->var.bits_per_pixel / 8;
	old_fs = get_fs();
	set_fs(get_ds());
	for(i = 0; i < kfb_handle->fb->var.yres; i++){
        memcpy(kfb_handle->fb->screen_base + i * kfb_handle->fb->fix.line_length, kfb_handle->buffer + i * l, l);
       
	}
	set_fs(old_fs);
	return KFB_OK;
}
