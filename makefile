build: server client

server:
	@cd Server && $(MAKE)

client:
	@cd Client && $(MAKE) 

.PHONY: clean mrproper
clean:
	@cd Server && $(MAKE) $@
	@cd Client && $(MAKE) $@
mrproper: clean
	@cd Server && $(MAKE) $@
	@cd Client && $(MAKE) $@