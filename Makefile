BUILD_DIR := build
BUILD_DIR_ARM := $(BUILD_DIR)

# ===== all =====
.PHONY: all
all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); cmake ../
	cd $(BUILD_DIR); make #VERBOSE=1

# ===== production =====
.PHONY: production
production:
	make clean-all
	CLGM_ENV=production make;

# ===== clean =====
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BUILD_DIR_ARM)

# ===== clean-all =====
.PHONY: clean-all
clean-all:
	make clean
	cd lib/uWebSockets; make clean
	cd lib/mruby; rake clean

# ===== clean-deep =====
.PHONY: clean-deep
clean-deep:
	make clean-all
	cd lib/mruby; rake deep_clean