all:
		make -C Jotunn
		make -C TestApp		
clean:
		make clean -C Jotunn
		make clean -C TestApp