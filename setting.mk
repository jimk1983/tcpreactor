#公共部分
CC=gcc
CFLAGS=-c 
MAKE=make
AR=ar cr
RM=-rm -rf
CFLAGS +=-Wall -g #-Werror -O3  	#将-Werror将告警当作错误处理


###########################
#公共属性定义
TARGET=
OUTPUTOBJSDIR=$(ROOTDIR)/output/objs
OUTPUTBINSDIR=$(ROOTDIR)/output/bin

##################################
#添加系统配置的头文件目录
CFLAGS+=-I$(ROOTDIR)
CFLAGS+=-I$(ROOTDIR)/libs/include/vos
##################################
#添加内部库的头文件目录
#lib头文件包含目录
CFLAGS+=-I$(INLIBDIR)/include/vos

LIFLAGS=-L$(INLIBDIR)/libs/bin -lrct -lvos  
##################################
#外部库头文件目录指定
#LSFLAGS +=

LDFLAGS = -ldl -lpthread -lrt -lm #-ldb


















