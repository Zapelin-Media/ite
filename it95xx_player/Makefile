all: zmi

zmi: zmi.c api.c
	$(CC) -o zmi zmi.c api.c -w -I.
	
install: zmi
	cp zmi /usr/bin/zmi
	
uninstall: zmi
	rm /usr/bin/zmi
	
clean: 
	rm zmi
	
