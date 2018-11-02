#Makefile的设置
include $(ROOTDIR)/setting.mk
dirs:=$(shell find . -maxdepth 1 -type d)
dirs:=$(basename $(patsubst ./%,%,$(dirs)))
dirs:=$(filter-out $(ROOTDIR),$(dirs))
SUBDIRS := $(dirs)
LIBRCTOBJSDIR=$(ROOTDIR)/output/objs
LIBRCTBINDIR=$(ROOTDIR)/output/bin
LIBRCTTARGET=librct.a
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
all:$(CUR_OBJS)
$(CUR_OBJS):%.o:%.c
	$(CC) $(CFLAGS) $^ -o $(LIBRCTOBJSDIR)/$@ $(LDFLAGS) $(LSFLAGS)
ECHO:
	@echo $(SUBDIRS)
LIBRCTOBJS=$(LIBRCTOBJSDIR)/*.o
librct:
	$(AR) $(LIBRCTBINDIR)/$(LIBRCTTARGET) $(LIBRCTOBJS)	
.PHONY:clean
clean:
	rm -rf $(LIBRCTOBJSDIR)/*.o
