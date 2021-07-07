ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif
include $(BOLOS_SDK)/Makefile.defines

APPNAME = "Nervos"

APP_LOAD_PARAMS= --appFlags 0 --curve secp256k1 --path "44'/309'" $(COMMON_LOAD_PARAMS) --tlvraw 9F:01
DEFINES += HAVE_PENDING_REVIEW_SCREEN

GIT_DESCRIBE ?= $(shell git describe --tags --abbrev=8 --always --long --dirty 2>/dev/null)

VERSION_TAG ?= $(shell echo "$(GIT_DESCRIBE)" | cut -f1 -d-)
APPVERSION_M=0
APPVERSION_N=5
APPVERSION_P=1
APPVERSION=$(APPVERSION_M).$(APPVERSION_N).$(APPVERSION_P)

# Only warn about version tags if specified/inferred
#ifeq ($(VERSION_TAG),)
#  $(warning VERSION_TAG not checked)
#else
#  ifneq (v$(APPVERSION), $(VERSION_TAG))
#    $(warning Version-Tag Mismatch: v$(APPVERSION) version and $(VERSION_TAG) tag)
#  endif
#endif

COMMIT ?= $(shell echo "$(GIT_DESCRIBE)" | sed 's/-dirty/*/')
ifeq ($(COMMIT),)
  $(warning COMMIT not specified and could not be determined with git from "$(GIT_DESCRIBE)")
else
  $(info COMMIT=$(COMMIT))
endif

ifeq ($(TARGET_NAME),TARGET_NANOX)
ICONNAME=icons/nano-x-nervos.gif
else
ICONNAME=icons/nano-s-nervos.gif
endif

################
# Default rule #
################
all: show-app default


.PHONY: show-app
show-app:
	@echo ">>>>> Building $(APP) at commit $(COMMIT)"


############
# Platform #
############

DEFINES   += OS_IO_SEPROXYHAL
DEFINES   += HAVE_BAGL HAVE_SPRINTF
DEFINES   += HAVE_IO_USB HAVE_L4_USBLIB IO_USB_MAX_ENDPOINTS=6 IO_HID_EP_LENGTH=64 HAVE_USB_APDU
DEFINES   += HAVE_LEGACY_PID
DEFINES   += VERSION=\"$(APPVERSION)\" APPVERSION_M=$(APPVERSION_M)
DEFINES   += COMMIT=\"$(COMMIT)\" APPVERSION_N=$(APPVERSION_N) APPVERSION_P=$(APPVERSION_P)
# DEFINES   += _Static_assert\(...\)=

ifeq ($(TARGET_NAME),TARGET_NANOX)
APP_LOAD_PARAMS += --appFlags 0x240 # with BLE support
DEFINES   += IO_SEPROXYHAL_BUFFER_SIZE_B=300
DEFINES   += HAVE_BLE BLE_COMMAND_TIMEOUT_MS=2000
DEFINES   += HAVE_BLE_APDU # basic ledger apdu transport over BLE

DEFINES   += HAVE_GLO096 HAVE_UX_FLOW
DEFINES   += HAVE_BAGL BAGL_WIDTH=128 BAGL_HEIGHT=64
DEFINES   += HAVE_BAGL_ELLIPSIS # long label truncation feature
DEFINES   += HAVE_BAGL_FONT_OPEN_SANS_REGULAR_11PX
DEFINES   += HAVE_BAGL_FONT_OPEN_SANS_EXTRABOLD_11PX
DEFINES   += HAVE_BAGL_FONT_OPEN_SANS_LIGHT_16PX

SDK_SOURCE_PATH  += lib_blewbxx lib_blewbxx_impl
SDK_SOURCE_PATH  += lib_ux
else
DEFINES   += IO_SEPROXYHAL_BUFFER_SIZE_B=128
DEFINES   += HAVE_UX_FLOW
SDK_SOURCE_PATH  += lib_ux
endif

# Enabling debug PRINTF
DEBUG ?= 0
ifneq ($(DEBUG),0)

        DEFINES += NERVOS_DEBUG
        DEFINES += STACK_MEASURE

        ifeq ($(TARGET_NAME),TARGET_NANOX)
                DEFINES   += HAVE_PRINTF PRINTF=mcu_usb_printf
        else
                DEFINES   += HAVE_PRINTF PRINTF=screen_printf
        endif
else
        DEFINES   += PRINTF\(...\)=
endif



##############
# Compiler #
##############
ifneq ($(BOLOS_ENV),)
$(info BOLOS_ENV=$(BOLOS_ENV))
CLANGPATH := $(BOLOS_ENV)/clang-arm-fropi/bin/
GCCPATH := $(BOLOS_ENV)/gcc-arm-none-eabi-5_3-2016q1/bin/
CFLAGS += -idirafter $(BOLOS_ENV)/gcc-arm-none-eabi-5_3-2016q1/arm-none-eabi/include
else
$(info BOLOS_ENV is not set: falling back to CLANGPATH and GCCPATH)
endif
ifeq ($(CLANGPATH),)
$(info CLANGPATH is not set: clang will be used from PATH)
endif
ifeq ($(GCCPATH),)
$(info GCCPATH is not set: arm-none-eabi-* will be used from PATH)
endif

CC       := $(CLANGPATH)clang

CFLAGS   += -O3 -Os -Wall -Wextra

AS     := $(GCCPATH)arm-none-eabi-gcc

LD       := $(GCCPATH)arm-none-eabi-gcc
LDFLAGS  += -O3 -Os
LDLIBS   += -lm -lgcc -lc

# import rules to compile glyphs(/pone)
include $(BOLOS_SDK)/Makefile.glyphs

### computed variables
APP_SOURCE_PATH  += src
SDK_SOURCE_PATH  += lib_stusb lib_stusb_impl

### U2F support (wallet app only)
SDK_SOURCE_PATH  += lib_u2f lib_stusb_impl

DEFINES   += USB_SEGMENT_SIZE=64

DEFINES   += U2F_PROXY_MAGIC=\"CKB\"
DEFINES   += HAVE_IO_U2F HAVE_U2F

load: all
	python -m ledgerblue.loadApp $(APP_LOAD_PARAMS)

delete:
	python -m ledgerblue.deleteApp $(COMMON_DELETE_PARAMS)

# import generic rules from the sdk
include $(BOLOS_SDK)/Makefile.rules

#add dependency on custom makefile filename
dep/%.d: %.c Makefile

.phony: watch
watch:
	ls src/*.c src/*.h tests/*.js tests/hw-app-ckb/src/*.js | entr make test

.phony: test
test: tests/*.js tests/package.json bin/app.elf
	env LEDGER_APP=./bin/app.elf COMMIT=$(COMMIT) run-ledger-tests.sh ./tests/

.phony: listvariants
listvariants:
	@echo VARIANTS COIN CKB
