all:
	make -C microkernel
	make -C soft
	make -C servers

clear:
	make -C microkernel clear
	make -C soft clear
	make -C servers clear

