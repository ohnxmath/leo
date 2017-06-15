INCLUDES=-Iinclude/
CFLAGS=$(INCLUDES) -Wall -Werror -pedantic
# LIBFLAGS=-Llibs/ -lstring

OBJ=
OUTPUT=libleo.a

default: $(OUTPUT)
debug: CFLAGS += -g -O0 -D__DEBUG
debug: $(OUTPUT)

################################################################################
#                                LIBRARY STUFF                                 #
################################################################################

libstring: libs/libstring.a

libs/libstring.a:
	@echo "Building libstring...";
	@mkdir -p libs; \
	cd libs; \
	git clone https://github.com/ohnx/libstring; \
	cd libstring; \
	make; \
	cp libstring.a ..;
	@cd libs; \
	cp libstring/libstring.a .; \

libs:
	@: # normally would say libstring

cleanlibs: 
	rm -f libs/libstring.a


################################################################################
#                                SOURCES STUFF                                 #
################################################################################

objs/%.o: src/%.c
	@mkdir -p objs/
	$(CC) -c -o $@ $< $(CFLAGS) $(EXTRA)

$(OUTPUT): libs $(OBJ)
	ar rcs $(OUTPUT) $(OBJ)

clean:
	-rm -f $(OBJ) objs/commandline.o
	-rm -f $(OUTPUT)
	-rm -f leo

################################################################################
#                             COMMAND LINE STUFF                               #
################################################################################

repl: $(OUTPUT) objs/repl.o
	$(CC) objs/repl.o -L. -lleo $(LIBFLAGS) -o leo

