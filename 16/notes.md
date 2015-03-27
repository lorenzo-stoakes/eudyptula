--to=sudipm.mukherjee@gmail.com --to=teddy.wang@siliconmotion.com --to=gregkh@linuxfoundation.org --cc=linux-fbdev@vger.kernel.org -cc=devel@driverdev.osuosl.org --cc=linux-kernel@vger.kernel.org


 (open list:STAGING - SILICON...)
 (open list:STAGING SUBSYSTEM)
 (open list)


drivers/staging/sm750fb/ddk750_help.c:12:17: warning: incorrect type in assignment (different address spaces)
drivers/staging/sm750fb/ddk750_help.c:12:17:    expected unsigned char volatile [noderef] <asn:2>*[addressable] [toplevel] mmio750
drivers/staging/sm750fb/ddk750_help.c:12:17:    got unsigned char volatile *addr


* `warns` generated on 9th March 2015 vs next-20150306 (v4-rc3)

[~/cloud/code/eudyptula/16]$ comm -13 files318 files

drivers/staging/fbtft/fb_agm1264k-fl.c
drivers/staging/fbtft/fb_pcd8544.c
drivers/staging/fbtft/fb_ra8875.c
drivers/staging/fbtft/fb_ssd1306.c
drivers/staging/fbtft/fb_tls8204.c
drivers/staging/fbtft/fb_uc1701.c
drivers/staging/fbtft/fb_watterott.c
drivers/staging/fbtft/fbtft-bus.c
drivers/staging/fbtft/fbtft-core.c
drivers/staging/fbtft/fbtft-sysfs.c

drivers/staging/i2o/i2o_config.c
drivers/staging/lustre/lustre/lmv/include/lprocfs_status.h
drivers/staging/sm7xxfb/sm7xx.h
drivers/staging/sm7xxfb/sm7xxfb.c

[~/cloud/code/eudyptula/16]$ comm -13 files318 filesStagingTest
drivers/staging/fbtft/fb_agm1264k-fl.c
drivers/staging/fbtft/fb_pcd8544.c
drivers/staging/fbtft/fb_ra8875.c
drivers/staging/fbtft/fb_ssd1306.c
drivers/staging/fbtft/fb_tls8204.c
drivers/staging/fbtft/fb_uc1701.c
drivers/staging/fbtft/fb_watterott.c
drivers/staging/fbtft/fbtft-bus.c
drivers/staging/fbtft/fbtft-core.c
drivers/staging/fbtft/fbtft-sysfs.c

drivers/staging/i2o/i2o_config.c

drivers/staging/sm750fb/ddk750_chip.c
drivers/staging/sm750fb/ddk750_chip.h
drivers/staging/sm750fb/ddk750_display.c
drivers/staging/sm750fb/ddk750_dvi.c
drivers/staging/sm750fb/ddk750_help.c
drivers/staging/sm750fb/ddk750_hwi2c.c
drivers/staging/sm750fb/ddk750_power.c
drivers/staging/sm750fb/ddk750_sii164.c
drivers/staging/sm750fb/ddk750_swi2c.c
drivers/staging/sm750fb/sm750.c
drivers/staging/sm750fb/sm750_accel.c
drivers/staging/sm750fb/sm750_cursor.c
drivers/staging/sm750fb/sm750_hw.c
drivers/staging/sm750fb/sm750_hw.h

drivers/staging/sm7xxfb/sm7xxfb.c
