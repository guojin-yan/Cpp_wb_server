.PHONY: xx

"":
	if [ -d "build" ]; then \
		cd build && make; \
	else \
		mkdir build; \
		cd build && cmake ..; \
	fi

%:
	if [ -d "build" ]; then \
		cd build && make $@; \
	else \
		mkdir build; \
		cd build && cmake -DCMAKE_CXX_COMPILER:FILEPATH=$(shell which g++) -DCMAKE_C_COMPILER:FILEPATH=$(shell which gcc) $@ ..; \
	fi
