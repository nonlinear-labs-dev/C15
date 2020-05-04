#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include "espi_driver.h"
#include "espi_fb.h"

static struct fb_fix_screeninfo oleds_fb_fix = {
  .id        = "NL C15",
  .type      = FB_TYPE_PACKED_PIXELS,
  .visual    = FB_VISUAL_TRUECOLOR,
  .xpanstep  = 0,
  .ypanstep  = 0,
  .ywrapstep = 0,
  .accel     = FB_ACCEL_NONE,
};

static struct fb_var_screeninfo oleds_fb_var = {
  /* Actually we are calculating with 4 in the driver,
	 * but the interface needs to transfer 1 Byte min.*/
  .bits_per_pixel = 8,
};

static void oleds_fb_update_display(struct oleds_fb_par *par)
{
  ssd1322_update_display(par);
  ssd1305_update_display(par);
}

static ssize_t oleds_fb_write(struct fb_info *info, const char __user *buf, size_t count, loff_t *ppos)
{
  unsigned long total_size;
  unsigned long p = *ppos;
  u8 __iomem *dst;

  total_size = info->fix.smem_len;

  if (p > total_size)
    return -EINVAL;

  // WTF is this?
  if (count + p > total_size)
    count = total_size - p;

  if (!count)
    return -EINVAL;

  dst = (void __force *) (info->screen_base + p);

  if (copy_from_user(dst, buf, count))
    return -EFAULT;

  *ppos += count;

  return count;
}

static void oleds_fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
  struct oleds_fb_par *par = info->par;
  sys_fillrect(info, rect);
  oleds_fb_update_display(par);
}

static void oleds_fb_copyarea(struct fb_info *info, const struct fb_copyarea *area)
{
  struct oleds_fb_par *par = info->par;
  sys_copyarea(info, area);
  oleds_fb_update_display(par);
}

static void oleds_fb_imageblit(struct fb_info *info, const struct fb_image *image)
{
  struct oleds_fb_par *par = info->par;
  sys_imageblit(info, image);
  oleds_fb_update_display(par);
}

static int oleds_fb_setcmap(struct fb_cmap *cmap, struct fb_info *info)
{
  return 0;
}

static int oleds_fb_setcolreg(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp, struct fb_info *info)
{
  return 0;
}

static int oleds_fb_blank(int blank, struct fb_info *info)
{
  return 0;
}

static int oleds_fb_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
  unsigned long start;
  u32           len;

  start = info->fix.smem_start;
  len   = info->fix.smem_len;

  vma->vm_page_prot = __pgprot_modify(vma->vm_page_prot, L_PTE_MT_MASK, L_PTE_MT_WRITETHROUGH);

  return vm_iomap_memory(vma, start, len);
}

static struct fb_ops oleds_fb_ops = {
  .owner        = THIS_MODULE,
  .fb_read      = fb_sys_read,
  .fb_write     = oleds_fb_write,
  .fb_fillrect  = oleds_fb_fillrect,
  .fb_copyarea  = oleds_fb_copyarea,
  .fb_imageblit = oleds_fb_imageblit,
  .fb_setcmap   = oleds_fb_setcmap,
  .fb_setcolreg = oleds_fb_setcolreg,
  .fb_blank     = oleds_fb_blank,
  .fb_mmap      = oleds_fb_mmap,
};

s32 espi_driver_oleds_fb_setup(struct espi_driver *sb)
{
  struct fb_info *     info;
  struct oleds_fb_par *par;
  u8 *                 vmem;
  u32                  vmem_size;
  s32                  ret;

  info = framebuffer_alloc(sizeof(struct oleds_fb_par), sb->dev);
  if (!info)
    return -ENOMEM;

  par         = info->par;
  par->info   = info;
  par->espi   = sb;
  sb->oleds   = par;
  par->width  = 256;
  par->height = 96;

  vmem_size = par->width * par->height * oleds_fb_var.bits_per_pixel / 8;
  vmem      = (u8 *) __get_free_pages(GFP_KERNEL | __GFP_ZERO, get_order(vmem_size));  //vzalloc(vmem_size);//
  if (!vmem)
  {
    ret = -ENOMEM;
    goto oleds_fb_alloc_error;
  }

  info->fbops = &oleds_fb_ops;
  info->fix   = oleds_fb_fix;

  info->fix.line_length = par->width * oleds_fb_var.bits_per_pixel / 8;

  info->var              = oleds_fb_var;
  info->var.xres         = par->width;
  info->var.xres_virtual = par->width;
  info->var.yres         = par->height;
  info->var.yres_virtual = par->height;
  info->var.nonstd       = 1;

  info->var.green.length = info->var.transp.length = info->var.blue.length = info->var.red.length = info->var.bits_per_pixel;

  info->var.red.offset = info->var.green.offset = info->var.blue.offset = info->var.transp.offset = 0;

  info->screen_base    = (u8 __force __iomem *) vmem;
  info->fix.smem_start = __pa(vmem);
  info->fix.smem_len   = vmem_size;

  ret = ssd1322_fb_init(par);
  if (ret)
    goto oleds_fb_reset_error;

  ret = ssd1305_fb_init(par);
  if (ret)
    goto oleds_fb_reset_error;

  ret = register_framebuffer(info);
  if (ret)
    goto oleds_fb_reset_error;

  return 0;

oleds_fb_reset_error:
oleds_fb_alloc_error:
  framebuffer_release(info);
  return ret;
}

s32 espi_driver_oleds_fb_cleanup(struct espi_driver *sb)
{
  struct fb_info *info = sb->oleds->info;

  ssd1322_fb_deinit(sb);
  ssd1305_fb_deinit(sb);

  __free_pages(__va(info->fix.smem_start), get_order(info->fix.smem_len));  //vfree(info->screen_base);//

  unregister_framebuffer(info);
  framebuffer_release(info);

  return 0;
}
