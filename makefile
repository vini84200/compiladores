
TARGET = Compiladores

all:
	$(MAKE) -C build && cp build/$(TARGET) ./etapa7

clean:
	rm -f ./etapa7 && $(MAKE) clean -C build
