SUBDIR = tex_mc\ 
SRCS =	prj_cfg.mtp prj_comp.mtp sub_comp.mtp tmc_cfg.mtp tmc_inst.mtp \
		tmc_krnl.mtp update.mtp
ITPS = instinfo.itp message.itp
!INCLUDE	..\makefile.in

all	:	this
	cd $(SRCDIR)\$(SUBDIR)ADOBE_AR
	$(MAKE) all
	cd $(SRCDIR)\$(SUBDIR)DVIOUT
	$(MAKE) all
	cd $(SRCDIR)\$(SUBDIR)DVI2TTY
	$(MAKE) all
	cd $(SRCDIR)\$(SUBDIR)DVI_PS
	$(MAKE) all
	cd $(SRCDIR)\$(SUBDIR)WinDVI
	$(MAKE) all
	cd $(SRCDIR)\$(SUBDIR)pTeX2
	$(MAKE) all
	cd $(SRCDIR)\$(SUBDIR)pTeX2GUI
	$(MAKE) all
	cd $(SRCDIR)\$(SUBDIR)WinTeX
	$(MAKE) all

clean	:	clean_this
	cd $(SRCDIR)\$(SUBDIR)ADOBE_AR
	$(MAKE) clean
	cd $(SRCDIR)\$(SUBDIR)DVIOUT
	$(MAKE) clean
	cd $(SRCDIR)\$(SUBDIR)DVI2TTY
	$(MAKE) clean
	cd $(SRCDIR)\$(SUBDIR)DVI_PS
	$(MAKE) clean
	cd $(SRCDIR)\$(SUBDIR)WinDVI
	$(MAKE) clean
	cd $(SRCDIR)\$(SUBDIR)pTeX2
	$(MAKE) clean
	cd $(SRCDIR)\$(SUBDIR)pTeX2GUI
	$(MAKE) clean
	cd $(SRCDIR)\$(SUBDIR)WinTeX
	$(MAKE) clean

