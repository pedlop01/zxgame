# Define the source file and output names
SRC = juegozx.c
BIN = juegozx.bin
TAP = juegozx.tap
ORG = 24200

# Z88DK and AppMake settings
ZCC = $(Z88DK_PATH)/bin/zcc +zx
ZCCFLAGS = -vn -lndos -zorg=$(ORG)
APMAKE = $(Z88DK_PATH)/src/appmake/z88dk-appmake +zx

# Default rule to build the .tap file
all: $(TAP)

# Rule to compile the C source into a binary
$(BIN): $(SRC)
	$(ZCC) $(ZCCFLAGS) -o $(BIN) $(SRC)

# Rule to convert the binary into a .tap file
$(TAP): $(BIN)
	$(APMAKE) -b $(BIN) -o $(TAP) --org $(ORG)

# Clean up generated files
clean:
	rm -f $(BIN) $(TAP)

