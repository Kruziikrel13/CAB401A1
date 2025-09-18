stub:
	@echo "Do not run $(MAKE) directly without any arguments."

release: shaders
	cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Release -S . -B ./build
	cmake --build ./build --config Release --target all -j`nproc 2>/dev/null || getconf NPROCESSORS_CONF`

debug: shaders
	cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_INSTALL_PREFIX:STRING=${PREFIX} -S . -B ./build
	cmake --build ./build --config Debug --target all -j`nproc 2>/dev/null || getconf NPROCESSORS_CONF`

trace: shaders
	cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DTRACE:STRING=True -DCMAKE_INSTALL_PREFIX:STRING=${PREFIX} -S . -B ./build
	cmake --build ./build --config Debug --target all -j`nproc 2>/dev/null || getconf NPROCESSORS_CONF`

asan: shaders
	cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DWITH_ASAN:STRING=True -DCMAKE_INSTALL_PREFIX:STRING=${PREFIX} -S . -B ./build
	cmake --build ./build --config Debug --target all -j`nproc 2>/dev/null || getconf NPROCESSORS_CONF`

clear:
	rm -rf build/

reset:
	git clean -ffdx

all:
	$(MAKE) clear
	$(MAKE) release

.PHONY: shaders
shaders:
	mkdir -p build/shaders
	glslangValidator -V shaders/shader.comp -o build/shaders/matrix_comp.spv
