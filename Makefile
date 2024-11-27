BDIR = build
SUBDIRS = biosboot boot mbr installboot installmbr 

all: $(BDIR) $(SUBDIRS) 

$(BDIR):
	mkdir -p $(BDIR)

clean: $(SUBDIRS)
	rm -rf $(BDIR)

$(SUBDIRS):
	@make -sC $@ $(MAKECMDGOALS) BDIR=$(PWD)/$(BDIR) IDIR=$(PWD)

.PHONY: biosboot boot installboot mbr installmbr
