BDIR = build
SUBDIRS = biosboot boot mbr installboot installmbr 

all: $(BDIR) $(SUBDIRS) 
	cp installboot/filecopy.sh $(BDIR)/

$(BDIR):
	mkdir -p $(BDIR)

clean: $(SUBDIRS)
	rm -rf $(BDIR)

$(SUBDIRS):
	@make -C $@ $(MAKECMDGOALS) BDIR=$(PWD)/$(BDIR) SDIR=$(PWD)

.PHONY: biosboot boot installboot mbr installmbr
