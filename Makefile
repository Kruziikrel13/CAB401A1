stub:
	@echo "Do not run $(MAKE) directly without any arguments."

release:
	cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Release -S . -B ./build
	cmake --build ./build --config Release --target all -j`nproc 2>/dev/null || getconf NPROCESSORS_CONF`

debug:
	cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_INSTALL_PREFIX:STRING=${PREFIX} -S . -B ./build
	cmake --build ./build --config Debug --target all -j`nproc 2>/dev/null || getconf NPROCESSORS_CONF`

clear:
	rm -rf build/

all:
	$(MAKE) clear
	$(MAKE) release

nix:
	echo "use nix" > .envrc
	direnv allow

asan:
	cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DWITH_ASAN:STRING=True -S . -B ./build
	cmake --build ./build --config Debug --target all
