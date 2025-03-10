INCLUDE_INSTALL_PATH=/usr/local/include
LIBRARY_INSTALL_PATH=/usr/local/lib

LIBRARY_NAME=cvl


all: install


install:
	@rm -rf $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)
	@mkdir -p $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)
	@cp -R ./include/* $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)

uninstall:
	@rm -rf $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)
