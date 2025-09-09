# Makefile for MyObject GObject library and test program

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -fPIC
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG

# pkg-config for GLib
GLIB_CFLAGS = $(shell pkg-config --cflags glib-2.0 gobject-2.0)
GLIB_LIBS = $(shell pkg-config --libs glib-2.0 gobject-2.0)

# GObject Introspection tools
GI_SCANNER = g-ir-scanner
GI_COMPILER = g-ir-compiler
VAPIGEN = vapigen

# Directories
SRCDIR = .
BUILDDIR = build
LIBDIR = $(BUILDDIR)/lib
GIRDIR = $(BUILDDIR)/gir
TYPELIBDIR = $(BUILDDIR)/typelib

# Library info
LIBRARY_NAME = myobject
LIBRARY_VERSION = 1.0
NAMESPACE = My
NSVERSION = 1.0

# Source files
SOURCES = myobject.c
HEADERS = myobject.h
OBJECTS = $(SOURCES:%.c=$(BUILDDIR)/%.o)

# Library files
STATIC_LIB = $(LIBDIR)/lib$(LIBRARY_NAME).a
SHARED_LIB = $(LIBDIR)/lib$(LIBRARY_NAME).so.$(LIBRARY_VERSION)
SHARED_LIB_LINK = $(LIBDIR)/lib$(LIBRARY_NAME).so

# GIR and typelib files
GIR_FILE = $(GIRDIR)/$(NAMESPACE)-$(NSVERSION).gir
TYPELIB_FILE = $(TYPELIBDIR)/$(NAMESPACE)-$(NSVERSION).typelib

# Test program
TEST_PROGRAM = $(BUILDDIR)/test

# Default target
all: debug

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: directories $(STATIC_LIB) $(SHARED_LIB) $(TEST_PROGRAM) gir

# Release build
release: CFLAGS += $(RELEASE_FLAGS)
release: directories $(STATIC_LIB) $(SHARED_LIB) $(TEST_PROGRAM) gir

# Create build directories
directories:
	@mkdir -p $(BUILDDIR) $(LIBDIR) $(GIRDIR) $(TYPELIBDIR)

# Compile object files
$(BUILDDIR)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(GLIB_CFLAGS) -c $< -o $@

# Build static library
$(STATIC_LIB): $(OBJECTS)
	ar rcs $@ $^
	@echo "Static library created: $@"

# Build shared library
$(SHARED_LIB): $(OBJECTS)
	$(CC) -shared -Wl,-soname,lib$(LIBRARY_NAME).so.$(LIBRARY_VERSION) -o $@ $^ $(GLIB_LIBS)
	ln -sf lib$(LIBRARY_NAME).so.$(LIBRARY_VERSION) $(SHARED_LIB_LINK)
	@echo "Shared library created: $@"

# Build test program
$(TEST_PROGRAM): test.c $(STATIC_LIB)
	$(CC) $(CFLAGS) $(GLIB_CFLAGS) -I$(SRCDIR) -o $@ $< -L$(LIBDIR) -l$(LIBRARY_NAME) $(GLIB_LIBS)
	@echo "Test program created: $@"

# Generate GIR file for GObject Introspection
gir: $(GIR_FILE)

$(GIR_FILE): $(SOURCES) $(HEADERS)
	$(GI_SCANNER) \
		--warn-all \
		--namespace=$(NAMESPACE) \
		--nsversion=$(NSVERSION) \
		--identifier-prefix=My \
		--symbol-prefix=my \
		--include=GObject-2.0 \
		--library=$(LIBRARY_NAME) \
		--library-path=$(LIBDIR) \
		--output=$@ \
		--pkg=glib-2.0 \
		--pkg=gobject-2.0 \
		--c-include="myobject.h" \
		$(GLIB_CFLAGS) \
		$(HEADERS) \
		$(SOURCES)
	@echo "GIR file generated: $@"

# Generate typelib file from GIR
typelib: $(TYPELIB_FILE)

$(TYPELIB_FILE): $(GIR_FILE)
	$(GI_COMPILER) --output=$@ $<
	@echo "Typelib file generated: $@"

# Generate Vala bindings (optional)
vapi: $(GIRDIR)/$(NAMESPACE)-$(NSVERSION).vapi

$(GIRDIR)/$(NAMESPACE)-$(NSVERSION).vapi: $(GIR_FILE)
	$(VAPIGEN) --library=$(NAMESPACE)-$(NSVERSION) --pkg=glib-2.0 --pkg=gobject-2.0 --directory=$(GIRDIR) $<
	@echo "Vala bindings generated: $@"

# Run tests
test: $(TEST_PROGRAM)
	@echo "Running tests..."
	LD_LIBRARY_PATH=$(LIBDIR):$$LD_LIBRARY_PATH $(TEST_PROGRAM)

# Install (basic installation)
install: release typelib
	@echo "Installing library and introspection data..."
	sudo cp $(SHARED_LIB) /usr/local/lib/
	sudo cp $(SHARED_LIB_LINK) /usr/local/lib/
	sudo cp $(HEADERS) /usr/local/include/
	sudo cp $(GIR_FILE) /usr/share/gir-1.0/
	sudo cp $(TYPELIB_FILE) /usr/lib/girepository-1.0/
	sudo ldconfig
	@echo "Installation complete!"

# Uninstall
uninstall:
	@echo "Uninstalling library and introspection data..."
	sudo rm -f /usr/local/lib/lib$(LIBRARY_NAME).so*
	sudo rm -f /usr/local/include/myobject.h
	sudo rm -f /usr/share/gir-1.0/$(NAMESPACE)-$(NSVERSION).gir
	sudo rm -f /usr/lib/girepository-1.0/$(NAMESPACE)-$(NSVERSION).typelib
	sudo ldconfig
	@echo "Uninstallation complete!"

# Generate documentation with gtk-doc (if available)
docs:
	@if command -v gtkdoc-scan >/dev/null 2>&1; then \
		echo "Generating documentation..."; \
		mkdir -p $(BUILDDIR)/docs; \
		gtkdoc-scan --module=$(LIBRARY_NAME) --source-dir=. --output-dir=$(BUILDDIR)/docs; \
		gtkdoc-mkdb --module=$(LIBRARY_NAME) --output-dir=$(BUILDDIR)/docs --source-dir=.; \
		gtkdoc-mkhtml $(LIBRARY_NAME) $(BUILDDIR)/docs/$(LIBRARY_NAME)-docs.xml --path=$(BUILDDIR)/docs; \
		echo "Documentation generated in $(BUILDDIR)/docs/"; \
	else \
		echo "gtk-doc not found. Install gtk-doc-tools to generate documentation."; \
	fi

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR)
	@echo "Build directory cleaned"

# Show build information
info:
	@echo "Build Information:"
	@echo "=================="
	@echo "Compiler: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "GLib CFLAGS: $(GLIB_CFLAGS)"
	@echo "GLib LIBS: $(GLIB_LIBS)"
	@echo "Library: $(LIBRARY_NAME) v$(LIBRARY_VERSION)"
	@echo "Namespace: $(NAMESPACE) v$(NSVERSION)"
	@echo ""
	@echo "Targets:"
	@echo "  all/debug  - Build debug version with GIR"
	@echo "  release    - Build optimized version with GIR"
	@echo "  test       - Run test program"
	@echo "  gir        - Generate GObject Introspection files"
	@echo "  typelib    - Generate typelib from GIR"
	@echo "  vapi       - Generate Vala bindings"
	@echo "  docs       - Generate documentation (requires gtk-doc)"
	@echo "  install    - Install library system-wide"
	@echo "  uninstall  - Remove installed files"
	@echo "  clean      - Clean build artifacts"
	@echo "  info       - Show this information"

# Python test with GI (requires Python3 and PyGI)
test-python: gir typelib
	@echo "Testing with Python GObject Introspection..."
	@echo "import sys, os" > $(BUILDDIR)/test_gi.py
	@echo "sys.path.insert(0, '$(GIRDIR)')" >> $(BUILDDIR)/test_gi.py
	@echo "os.environ['GI_TYPELIB_PATH'] = '$(TYPELIBDIR)'" >> $(BUILDDIR)/test_gi.py
	@echo "import gi" >> $(BUILDDIR)/test_gi.py
	@echo "gi.require_version('$(NAMESPACE)', '$(NSVERSION)')" >> $(BUILDDIR)/test_gi.py
	@echo "from gi.repository import $(NAMESPACE)" >> $(BUILDDIR)/test_gi.py
	@echo "obj = $(NAMESPACE).Object.new_with_value(42)" >> $(BUILDDIR)/test_gi.py
	@echo "obj.set_name('Python Test')" >> $(BUILDDIR)/test_gi.py
	@echo "print('Value:', obj.get_value())" >> $(BUILDDIR)/test_gi.py
	@echo "print('Name:', obj.get_name())" >> $(BUILDDIR)/test_gi.py
	@echo "print('String:', obj.to_string())" >> $(BUILDDIR)/test_gi.py
	@echo "obj.increment()" >> $(BUILDDIR)/test_gi.py
	@echo "print('After increment:', obj.get_value())" >> $(BUILDDIR)/test_gi.py
	@LD_LIBRARY_PATH=$(LIBDIR):$$LD_LIBRARY_PATH python3 $(BUILDDIR)/test_gi.py

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@pkg-config --exists glib-2.0 gobject-2.0 || (echo "ERROR: GLib/GObject development packages not found" && exit 1)
	@command -v $(GI_SCANNER) >/dev/null || echo "WARNING: g-ir-scanner not found (gobject-introspection-devel package)"
	@command -v $(GI_COMPILER) >/dev/null || echo "WARNING: g-ir-compiler not found (gobject-introspection package)"
	@echo "Dependencies check complete"

.PHONY: all debug release directories gir typelib vapi test install uninstall docs clean info test-python check-deps
