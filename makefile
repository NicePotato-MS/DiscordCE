# Main Makefile

# List of subdirectories to build
SUBDIRS := CE
SEND := false

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ clean
	$(MAKE) -C $@ gfx
	$(MAKE) -C $@
	@if [ "$(SEND)" = "true" ]; then \
		# Find and send .8xp files \
		for file in $(shell find $@/bin -name '*.8xp'); do \
			tilp -s -n $$file; \
		done; \
		# Find and send .8xv files \
		for file in $(shell find $@/bin -name '*.8xv'); do \
			tilp -s -n $$file; \
		done; \
	fi
	
clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
		rm -f $$dir/src/gfx/*.c; \
		rm -f $$dir/src/gfx/*.h; \
	done
