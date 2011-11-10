RGB_TEXTURES := barrel_rusty01.png crate01.png skybox.png grass06.png
RGBA_TEXTURES := buttons.png
TMP_DIR := $(shell readlink -f ./tmp)
OUT_DIR := $(shell readlink -f ../../processed/textures)
ATC_TEXTURES := $(shell for i in $(RGB_TEXTURES:.png=.dds) $(RGBA_TEXTURES:.png=.dds); do echo "$(TMP_DIR)/atc/$$i"; done)
S3TC_TEXTURES := $(shell for i in $(RGB_TEXTURES:.png=.dds) $(RGBA_TEXTURES:.png=.dds); do echo "$(TMP_DIR)/s3tc/$$i"; done)
ETC_TEXTURES := $(shell for i in $(RGB_TEXTURES:.png=.pvr); do echo "$(TMP_DIR)/etc/$$i"; done)
PVRTC_TEXTURES := $(shell for i in $(RGB_TEXTURES:.png=.pvr) $(RGBA_TEXTURES:.png=.pvr); do echo "$(TMP_DIR)/pvrtc/$$i"; done)

ifeq ($(PLATFORM), WIN32)
	ATC_TOOL := .\\tools\\TheCompressonator.exe
	PVRTC_TOOL := ./tools/PVRTexTool.exe
	S3TC_TOOL := ./tools/nvcompress.exe
	CONVERTER := ./tools/converter.exe
else
	PVRTC_TOOL := ./tools/PVRTexTool
	S3TC_TOOL := nvcompress
	CONVERTER := ./tools/converter
endif

all: $(RGB_TEXTURES) $(RGBA_TEXTURES) make_dirs pvrtc s3tc etc

make_dirs:
	$(shell for i in "atc" "s3tc" "pvrtc" "etc"; do [ -d "$(OUT_DIR)/$$i" ] || mkdir -p "$(OUT_DIR)/$$i"; done)
	$(shell for i in "atc" "s3tc" "pvrtc" "etc"; do [ -d "$(TMP_DIR)/$$i" ] || mkdir -p "$(TMP_DIR)/$$i"; done)

atc: $(RGB_TEXTURES:.png=.atc_rgb) $(RGBA_TEXTURES:.png=.atc_rgba) $(ATC_TEXTURES:.dds=.texture)

pvrtc: $(RGB_TEXTURES:.png=.pvrtc_rgb) $(RGBA_TEXTURES:.png=.pvrtc_rgba) $(PVRTC_TEXTURES:.pvr=.texture)

s3tc: $(RGB_TEXTURES:.png=.s3tc_rgb) $(RGBA_TEXTURES:.png=.s3tc_rgba) $(S3TC_TEXTURES:.dds=.texture)

etc: $(RGB_TEXTURES:.png=.etc_rgb) $(ETC_TEXTURES:.pvr=.texture)

%.flipped: %.png
	@if [ "$(@:.flipped=.png)" -nt "$(TMP_DIR)/$(@:.flipped=.png)" ]; then \
		echo "flipping $(@:.flipped=.png)"; \
		convert -flip "$(@:.flipped=.png)" "$(TMP_DIR)/$(@:.flipped=.png)"; \
	fi

%.atc_rgb: %.flipped
	@if [ "$(TMP_DIR)/$(@:.atc_rgb=.png)" -nt "$(TMP_DIR)/atc/$(@:.atc_rgb=.dds)" ]; then \
		$(ATC_TOOL) -i "$(TMP_DIR)/$(@:.atc_rgb=.png)" -o "$(TMP_DIR)/atc/$(@:.atc_rgb=.dds)"; \
	fi

%.atc_rgba: %.flipped
	@if [ "$(TMP_DIR)/$(@:.atc_rgba=.png)" -nt "$(TMP_DIR)/atc/$(@:.atc_rgba=.dds)" ]; then \
		$(ATC_TOOL) -i "$(TMP_DIR)/$(@:.atc_rgba=.png)" -o "$(TMP_DIR)/atc/$(@:.atc_rgba=.dds)"; \
	fi

%.s3tc_rgb: %.flipped
	@if [ "$(TMP_DIR)/$(@:.s3tc_rgb=.png)" -nt "$(TMP_DIR)/s3tc/$(@:.s3tc_rgb=.dds)" ]; then \
		echo "converting $(@:.s3tc_rgb=.png) to s3tc [rgb]"; \
		$(S3TC_TOOL) -color -bc3 "$(TMP_DIR)/$(@:.s3tc_rgb=.png)" "$(TMP_DIR)/s3tc/$(@:.s3tc_rgb=.dds)" >/dev/null; \
	fi

%.s3tc_rgba: %.flipped
	@if [ "$(TMP_DIR)/$(@:.s3tc_rgba=.png)" -nt "$(TMP_DIR)/s3tc/$(@:.s3tc_rgba=.dds)" ]; then \
		echo "converting $(@:.s3tc_rgba=.png) to s3tc [rgba]"; \
		$(S3TC_TOOL) -alpha -bc3 "$(TMP_DIR)/$(@:.s3tc_rgba=.png)" "$(TMP_DIR)/s3tc/$(@:.s3tc_rgba=.dds)" >/dev/null; \
	fi

%.pvrtc_rgb: %.flipped
	@if [ "$(TMP_DIR)/$(@:.pvrtc_rgb=.png)" -nt "$(TMP_DIR)/pvrtc/$(@:.pvrtc_rgb=.pvr)" ]; then \
		echo "converting $(@:.pvrtc_rgb=.png) to PVRTC [rgb]"; \
		$(PVRTC_TOOL) -fPVRTC2 -m -yflip0 -silent -i "$(TMP_DIR)/$(@:.pvrtc_rgb=.png)" -o "$(TMP_DIR)/pvrtc/$(@:.pvrtc_rgb=.pvr)"; \
	fi

%.pvrtc_rgba: %.flipped
	@if [ "$(TMP_DIR)/$(@:.pvrtc_rgba=.png)" -nt "$(TMP_DIR)/pvrtc/$(@:.pvrtc_rgba=.pvr)" ]; then \
		echo "converting $(@:.pvrtc_rgba=.png) to PVRTC [rgba]"; \
		$(PVRTC_TOOL) -fPVRTC4 -m -yflip0 -silent -i "$(TMP_DIR)/$(@:.pvrtc_rgba=.png)" -o "$(TMP_DIR)/pvrtc/$(@:.pvrtc_rgba=.pvr)"; \
	fi

%.etc_rgb: %.flipped
	@if [ "$(TMP_DIR)/$(@:.etc_rgb=.png)" -nt "$(TMP_DIR)/etc/$(@:.etc_rgb=.pvr)" ]; then \
		echo "converting $(@:.etc_rgb=.png) to ETC"; \
		$(PVRTC_TOOL) -fETC -m -yflip0 -silent -i "$(TMP_DIR)/$(@:.etc_rgb=.png)" -o "$(TMP_DIR)/etc/$(@:.etc_rgb=.pvr)"; \
	fi

%.texture: %.pvr
	@if [ "$(@:.texture=.pvr)" -nt "$(shell echo $@ | sed 's|$(TMP_DIR)|$(OUT_DIR)|g')" ]; then \
		echo "converting $(@:.texture=.pvr) to $(shell echo $@ | sed 's|$(TMP_DIR)|$(OUT_DIR)|g')"; \
		$(CONVERTER) -i "$(@:.texture=.pvr)" -o "$(shell echo $@ | sed 's|$(TMP_DIR)|$(OUT_DIR)|g')" >/dev/null; \
	fi

%.texture: %.dds
	@if [ "$(@:.texture=.dds)" -nt "$(shell echo $@ | sed 's|$(TMP_DIR)|$(OUT_DIR)|g')" ]; then \
		echo "converting $(@:.texture=.dds) to $(shell echo $@ | sed 's|$(TMP_DIR)|$(OUT_DIR)|g')"; \
		$(CONVERTER) -i "$(@:.texture=.dds)" -o "$(shell echo $@ | sed 's|$(TMP_DIR)|$(OUT_DIR)|g')" >/dev/null; \
	fi

clean:
	@echo "removing temporary files"
	@rm -Rf $(TMP_DIR)

