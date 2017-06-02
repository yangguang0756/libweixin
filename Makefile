include $(APP_DIR)/app.mk
TARGET = libweixin.so
#cJSON
LDFLAGS += -L$(APP_DIR)/lib/weixin -lcJSON
#xml2
CFLAGS += -I$(APP_DIR)/lib/weixin/libxml2
LDFLAGS += -lxml2
#curl
LDFLAGS += -lcurl
SOURCE= \
	lib_weixin.c

OBJS = $(SOURCE:%.c=obj/%.o)

$(TARGET): $(OBJS)
	$(CC) -fPIC -shared -o $@ $(OBJS) $(LDFLAGS)

weixin: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

obj/%.o:%.c
	@mkdir -p obj
	$(CC) -fPIC $(CFLAGS) -c $< -o $@ 
	
install:
	cp -f $(TARGET) $(ROOTFS_DIR)/apache/lib/
	cp -f libcJSON.so $(ROOTFS_DIR)/apache/lib/
	cp -f libxml2.so $(ROOTFS_DIR)/apache/lib/
	cp -f libcurl.so $(ROOTFS_DIR)/apache/lib/
	cp -f getUserAttentionState $(INST_DIR)/bin/

clean:
	rm -fr obj $(OBJS) $(TARGET)
