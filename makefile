SHELL = /bin/sh

PKG_CONFIG ?= pkg-config
GUI_LIBS += lv2 sndfile x11 cairo
LV2_LIBS += lv2 sndfile
ifneq ($(shell $(PKG_CONFIG) --exists fontconfig || echo no), no)
  GUI_LIBS += fontconfig
  GUIPPFLAGS += -DPKG_HAVE_FONTCONFIG
endif

CC ?= gcc
CXX ?= g++
INSTALL ?= install
INSTALL_PROGRAM ?= $(INSTALL)
INSTALL_DATA ?= $(INSTALL) -m644
STRIP ?= strip

PREFIX ?= /usr/local
LV2DIR ?= $(PREFIX)/lib/lv2

CPPFLAGS += -DPIC
CFLAGS += -std=c99 -fvisibility=hidden -fPIC
CXXFLAGS += -std=c++11 -fvisibility=hidden -fPIC
LDFLAGS += -shared -Wl,-z,relro,-z,now
STRIPFLAGS += -s --strip-program=$(STRIP)

GUIPPFLAGS += -DPUGL_HAVE_CAIRO

DSPCFLAGS += `$(PKG_CONFIG) --cflags $(LV2_LIBS)`
GUICFLAGS += `$(PKG_CONFIG) --cflags $(GUI_LIBS)`
DSPLFLAGS += `$(PKG_CONFIG) --libs $(LV2_LIBS)`
GUILFLAGS += `$(PKG_CONFIG) --libs $(GUI_LIBS)`

BUNDLE = BOops.lv2
DSP = BOops
DSP_SRC = ./src/BOops.cpp
GUI = BOopsGUI
GUI_SRC = ./src/BOopsGUI.cpp
OBJ_EXT = .so
DSP_OBJ = $(DSP)$(OBJ_EXT)
GUI_OBJ = $(GUI)$(OBJ_EXT)
B_OBJECTS = $(addprefix $(BUNDLE)/, $(DSP_OBJ) $(GUI_OBJ))
ROOTFILES = \
	*.ttl \
	LICENSE

INCFILES = inc/*.png inc/*.wav

B_FILES = $(addprefix $(BUNDLE)/, $(ROOTFILES) $(INCFILES))

DSP_INCL = \
	src/Message.cpp \
	src/BUtilities/stof.cpp \
	src/Slot.cpp

GUI_CXX_INCL = \
	src/SampleChooser.cpp \
	src/ShapeWidget.cpp \
	src/SelectWidget.cpp \
	src/ValueSelect.cpp \
	src/DownClick.cpp \
	src/UpClick.cpp \
	src/BWidgets/FileChooser.cpp \
	src/BWidgets/HPianoRoll.cpp \
	src/BWidgets/PianoWidget.cpp \
	src/BWidgets/MessageBox.cpp \
	src/BWidgets/TextButton.cpp \
	src/BWidgets/DrawingSurface.cpp \
	src/BWidgets/PopupListBox.cpp \
	src/BWidgets/ListBox.cpp \
	src/BWidgets/ChoiceBox.cpp \
	src/BWidgets/ItemBox.cpp \
	src/BWidgets/Text.cpp \
	src/BWidgets/UpButton.cpp \
	src/BWidgets/DownButton.cpp \
	src/BWidgets/ToggleButton.cpp \
	src/BWidgets/Button.cpp \
	src/BWidgets/HSlider.cpp \
	src/BWidgets/HScale.cpp \
	src/BWidgets/Knob.cpp \
	src/BWidgets/RangeWidget.cpp \
	src/BWidgets/ValueWidget.cpp \
	src/BWidgets/ImageIcon.cpp \
	src/BWidgets/Icon.cpp \
	src/BWidgets/Label.cpp \
	src/BWidgets/Window.cpp \
	src/BWidgets/Widget.cpp \
	src/BWidgets/BStyles.cpp \
	src/BWidgets/BColors.cpp \
	src/BWidgets/BItems.cpp \
	src/BUtilities/to_string.cpp \
	src/BUtilities/stof.cpp

GUI_C_INCL = \
	src/screen.c \
	src/BWidgets/cairoplus.c \
	src/BWidgets/pugl/implementation.c \
	src/BWidgets/pugl/x11_stub.c \
	src/BWidgets/pugl/x11_cairo.c \
	src/BWidgets/pugl/x11.c

$(BUNDLE): check clean $(DSP_OBJ) $(GUI_OBJ)
	@cp $(ROOTFILES) $(BUNDLE)
	@mkdir -p $(BUNDLE)/inc
	@cp $(INCFILES) $(BUNDLE)/inc

all: $(BUNDLE)

$(DSP_OBJ): $(DSP_SRC)
	@echo -n Build $(BUNDLE) DSP...
	@mkdir -p $(BUNDLE)
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(DSPCFLAGS) -Wl,--start-group $(DSPLFLAGS) $< $(DSP_INCL) -Wl,--end-group -o $(BUNDLE)/$@
	@echo \ done.

$(GUI_OBJ): $(GUI_SRC)
	@echo -n Build $(BUNDLE) GUI...
	@mkdir -p $(BUNDLE)
	@mkdir -p $(BUNDLE)/tmp
	@cd $(BUNDLE)/tmp; $(CC) $(CPPFLAGS) $(GUIPPFLAGS) $(CFLAGS) $(GUICFLAGS) $(addprefix ../../, $(GUI_C_INCL)) -c
	@cd $(BUNDLE)/tmp; $(CXX) $(CPPFLAGS) $(GUIPPFLAGS) $(CXXFLAGS) $(GUICFLAGS) $(addprefix ../../, $< $(GUI_CXX_INCL)) -c
	@$(CXX) $(CPPFLAGS) $(GUIPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(GUICFLAGS) -Wl,--start-group $(GUILFLAGS) $(BUNDLE)/tmp/*.o -Wl,--end-group -o $(BUNDLE)/$@
	@rm -rf $(BUNDLE)/tmp
	@echo \ done.

install:
	@echo -n Install $(BUNDLE) to $(DESTDIR)$(LV2DIR)...
	@$(INSTALL) -d $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	@$(INSTALL) -d $(DESTDIR)$(LV2DIR)/$(BUNDLE)/inc
	@$(INSTALL_PROGRAM) -m755 $(B_OBJECTS) $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	@$(INSTALL_DATA) $(addprefix $(BUNDLE)/, $(ROOTFILES)) $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	@$(INSTALL_DATA) $(addprefix $(BUNDLE)/, $(INCFILES)) $(DESTDIR)$(LV2DIR)/$(BUNDLE)/inc
	@echo \ done.

install-strip:
	@echo -n "Install (stripped)" $(BUNDLE) to $(DESTDIR)$(LV2DIR)...
	@$(INSTALL) -d $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	@$(INSTALL) -d $(DESTDIR)$(LV2DIR)/$(BUNDLE)/inc
	@$(INSTALL_PROGRAM) -m755 $(STRIPFLAGS) $(B_OBJECTS) $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	@$(INSTALL_DATA) $(addprefix $(BUNDLE)/, $(ROOTFILES)) $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	@$(INSTALL_DATA) $(addprefix $(BUNDLE)/, $(INCFILES)) $(DESTDIR)$(LV2DIR)/$(BUNDLE)/inc
	@echo \ done.

uninstall:
	@echo -n Uninstall $(BUNDLE)...
	@rm -f $(addprefix $(DESTDIR)$(LV2DIR)/$(BUNDLE)/, $(ROOTFILES) $(INCFILES))
	-@rmdir $(DESTDIR)$(LV2DIR)/$(BUNDLE)/inc
	@rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/$(GUI_OBJ)
	@rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/$(DSP_OBJ)
	@rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/$(GUI_CV_OBJ)
	@rm -f $(DESTDIR)$(LV2DIR)/$(BUNDLE)/$(DSP_CV_OBJ)
	-@rmdir $(DESTDIR)$(LV2DIR)/$(BUNDLE)
	@echo \ done.

check:
ifeq ($(shell $(PKG_CONFIG) --exists 'sndfile > 1.0.18' || echo no), no)
  $(error sndfile >= 1.0.18 not found. Please install sndfile >= 1.0.18 first.)
endif
ifeq ($(shell $(PKG_CONFIG) --exists 'lv2 >= 1.14.0' || echo no), no)
  $(error lv2 >= 1.14.0 not found. Please install lv2 >= 1.14.0 first.)
endif
ifeq ($(shell $(PKG_CONFIG) --exists 'x11 >= 1.6.0' || echo no), no)
  $(error x11 >= 1.6.0 not found. Please install x11 >= 1.6.0 first.)
endif
ifeq ($(shell $(PKG_CONFIG) --exists 'cairo >= 1.12.0' || echo no), no)
  $(error cairo >= 1.12.0 not found. Please install cairo >= 1.12.0 first.)
endif

clean:
	@rm -rf $(BUNDLE)

.PHONY: all install install-strip uninstall clean

.NOTPARALLEL:
