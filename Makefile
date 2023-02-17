all:
	make -C Jotunn
	make -C BoilerPlateApp
	make -C TestApp

debug:
	make debug -C Jotunn
	make debug -C BoilerPlateApp
	make debug -C TestApp

release:
	make release -C Jotunn
	make release -C BoilerPlateApp
	make release -C TestApp

clean:
	make clean -C Jotunn
	make clean -C BoilerPlateApp
	make clean -C TestApp