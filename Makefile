INCLUDE_INSTALL_PATH=/usr/local/include
LIBRARY_INSTALL_PATH=/usr/local/lib

LIBRARY_NAME=cvl


all: development


development:
	@rm -rf $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)
	@ln -s $(shell pwd)/include $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)

install:
	@rm -rf $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)
	@mkdir -p $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)
	@cp -R ./include/* $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)

uninstall:
	@rm -rf $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)
