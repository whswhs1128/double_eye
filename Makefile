PORTING_ROOT_DIR ?= $(shell cd $(CURDIR)/../.. && /bin/pwd)

include $(PORTING_ROOT_DIR)/porting_base.mk

TARGET := double_eye
PORTING_CFLAGS += -lbin -lpq_bin
CFLAGS := $(PORTING_CFLAGS) $(PORTING_LIBS) -L./ -lrtsp -I./adp

SRCS := $(wildcard ./*.c)
SRCS += $(wildcard ./adp/*.c)

OBJS := $(patsubst %.c, %.o, $(SRCS))

.PHONY: all clean

all: $(OBJS)
	$(AT)$(CC) -o $(TARGET) $^ $(CFLAGS)

%.o : %.c
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(AT)rm -rf $(OBJS) $(TARGET)

