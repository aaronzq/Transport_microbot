# invoke SourceDir generated makefile for empty.pem4f
empty.pem4f: .libraries,empty.pem4f
.libraries,empty.pem4f: package/cfg/empty_pem4f.xdl
	$(MAKE) -f C:\Users\AaronW\Documents\GitHub\Transport_microbot\LineMouse_KS/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\AaronW\Documents\GitHub\Transport_microbot\LineMouse_KS/src/makefile.libs clean

