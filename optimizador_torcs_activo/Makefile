SOURCE = principal.cpp GeneticoSimple2.cpp Individuo.cpp Estadisticas.cpp ./problemas/ProblemaOptim.cpp ./problemas/TorcsFun.cpp


instalar: genetico_torcs
	cp genetico_torcs ../launchtorcs
	@echo -e "\n=== Deben correr el ejecutable desde el directorio launchtorcs ===\n"

genetico_torcs: ${SOURCE}
	mpic++ -I./problemas/ ./problemas/*.cpp *.cpp -lfmt -o genetico_torcs

clean:
	rm -r genetico_torcs
