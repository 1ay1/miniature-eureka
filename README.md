# MyObject - GObject with Introspection Example

This project demonstrates how to create a proper GObject implementation that works with GObject Introspection (GI). The example includes a complete object hierarchy, properties, signals, methods, and all the necessary components for language bindings.

## Overview

MyObject is a simple GObject class that demonstrates:

- **Properties**: `value` (integer) and `name` (string)
- **Methods**: Constructor, getters/setters, increment/decrement, string representation
- **Signals**: `value-changed` signal emitted when the value property changes
- **GObject Introspection**: Full support for automatic language bindings
- **Memory Management**: Proper reference counting and cleanup

## Features

### Core GObject Features
- ✅ Proper GObject class hierarchy
- ✅ Private data structure
- ✅ Properties with notifications
- ✅ Signal system
- ✅ Reference counting
- ✅ Type system integration

### GObject Introspection Features
- ✅ Annotated API documentation
- ✅ GIR file generation
- ✅ Typelib compilation
- ✅ Python bindings support
- ✅ Vala bindings support
- ✅ JavaScript bindings support

## Building

### Prerequisites

Install the required development packages:

```bash
# Ubuntu/Debian
sudo apt-get install build-essential libglib2.0-dev gobject-introspection libgirepository1.0-dev

# Fedora/RHEL
sudo dnf install gcc glib2-devel gobject-introspection-devel

# Arch Linux
sudo pacman -S base-devel glib2 gobject-introspection
```

### Build Commands

```bash
# Check dependencies
make check-deps

# Build debug version with introspection data
make debug

# Build release version
make release

# Run tests
make test

# Generate typelib for runtime introspection
make typelib

# Test with Python (requires PyGI)
make test-python

# Install system-wide
make install

# Generate documentation (requires gtk-doc)
make docs

# Clean build artifacts
make clean
```

## Usage

### C Usage

```c
#include <glib.h>
#include "myobject.h"

int main() {
    // Create object
    MyObject *obj = my_object_new_with_value(42);
    my_object_set_name(obj, "Example Object");
    
    // Connect to signals
    g_signal_connect(obj, "value-changed", 
                     G_CALLBACK(on_value_changed), NULL);
    
    // Use methods
    my_object_increment(obj);
    gint value = my_object_get_value(obj);
    gchar *str = my_object_to_string(obj);
    
    g_print("Object: %s\n", str);
    
    // Cleanup
    g_free(str);
    g_object_unref(obj);
    return 0;
}
```

### Python Usage (with PyGI)

```python
#!/usr/bin/env python3
import gi
gi.require_version('My', '1.0')
from gi.repository import My

# Create object
obj = My.Object.new_with_value(42)
obj.set_name("Python Object")

# Connect to signals
def on_value_changed(obj, new_value):
    print(f"Value changed to: {new_value}")

obj.connect("value-changed", on_value_changed)

# Use methods
obj.increment()
print(f"Value: {obj.get_value()}")
print(f"Name: {obj.get_name()}")
print(f"String: {obj.to_string()}")

# Properties can be accessed directly
obj.props.value = 100
print(f"New value: {obj.props.value}")
```

### JavaScript Usage (with GJS)

```javascript
#!/usr/bin/env gjs

const { GObject, My } = imports.gi;

// Create object
let obj = new My.Object({ value: 42, name: "JavaScript Object" });

// Connect to signals
obj.connect('value-changed', (obj, newValue) => {
    print(`Value changed to: ${newValue}`);
});

// Use methods
obj.increment();
print(`Value: ${obj.get_value()}`);
print(`Name: ${obj.get_name()}`);
print(`String: ${obj.to_string()}`);
```

### Vala Usage

```vala
using My;

int main() {
    var obj = new My.Object.with_value(42);
    obj.name = "Vala Object";
    
    obj.value_changed.connect((new_value) => {
        stdout.printf("Value changed to: %d\n", new_value);
    });
    
    obj.increment();
    stdout.printf("Value: %d\n", obj.value);
    stdout.printf("Name: %s\n", obj.name);
    stdout.printf("String: %s\n", obj.to_string());
    
    return 0;
}
```

## Architecture

### File Structure

```
c_prac/
├── myobject.h          # Header file with public API
├── myobject.c          # Implementation file
├── test.c              # Test program
├── Makefile            # Build system
├── myobject.pc.in      # pkg-config template
└── README.md           # This file

build/                  # Generated build artifacts
├── lib/                # Compiled libraries
├── gir/                # GIR files
├── typelib/            # Typelib files
└── docs/               # Generated documentation
```

### Class Hierarchy

```
GObject
└── MyObject
```

### Memory Layout

```c
struct _MyObject {
    GObject parent_instance;    // Parent class data
    MyObjectPrivate *priv;      // Private data pointer
};

struct _MyObjectPrivate {
    gint value;                 // Integer property
    gchar *name;                // String property (owned)
};
```

## Key Design Patterns

### 1. Proper GObject Implementation

- Uses `G_DEFINE_TYPE_WITH_PRIVATE` macro
- Implements all required virtual methods
- Follows GObject naming conventions
- Proper dispose/finalize implementation

### 2. Properties System

```c
// Property definition
properties[PROP_VALUE] = 
    g_param_spec_int("value", "Value", "The integer value",
                     G_MININT, G_MAXINT, 0,
                     G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
```

### 3. Signal System

```c
// Signal definition
signals[VALUE_CHANGED] = 
    g_signal_new("value-changed", G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_LAST, 0, NULL, NULL,
                 g_cclosure_marshal_VOID__INT,
                 G_TYPE_NONE, 1, G_TYPE_INT);
```

### 4. Introspection Annotations

```c
/**
 * my_object_new_with_value:
 * @initial_value: the initial value to set
 *
 * Creates a new #MyObject with the specified initial value.
 *
 * Returns: (transfer full): a new #MyObject
 */
```

## Common Annotations

- `(transfer full)`: Caller owns the returned reference
- `(transfer none)`: Caller doesn't own the returned reference
- `(nullable)`: Parameter or return value can be NULL
- `(array)`: Parameter is an array
- `(element-type)`: Specifies container element type
- `(in)`: Input parameter
- `(out)`: Output parameter
- `(inout)`: Input/output parameter

## Troubleshooting

### Common Build Issues

1. **Missing GLib headers**
   ```bash
   sudo apt-get install libglib2.0-dev
   ```

2. **g-ir-scanner not found**
   ```bash
   sudo apt-get install gobject-introspection libgirepository1.0-dev
   ```

3. **Library not found at runtime**
   ```bash
   export LD_LIBRARY_PATH=./build/lib:$LD_LIBRARY_PATH
   ```

### Debugging Introspection

1. **Check GIR file**
   ```bash
   cat build/gir/My-1.0.gir
   ```

2. **Verify typelib**
   ```bash
   g-ir-inspect --typelib=build/typelib/My-1.0.typelib
   ```

3. **Test with Python**
   ```python
   import gi
   gi.require_version('My', '1.0')
   from gi.repository import My
   help(My.Object)
   ```

## Best Practices

### Documentation
- Use gtk-doc style comments
- Add detailed parameter descriptions
- Include code examples
- Document transfer annotations

### Error Handling
- Use `g_return_if_fail()` for parameter validation
- Provide meaningful error messages
- Handle edge cases gracefully

### Memory Management
- Always use g_malloc/g_free family
- Implement proper dispose/finalize
- Avoid circular references
- Use weak references when needed

### API Design
- Follow GObject conventions
- Make properties bindable
- Emit signals for important state changes
- Provide both convenience and detailed APIs

## Testing

The test suite covers:
- Object creation and destruction
- Property getting and setting
- Method invocation
- Signal emission and handling
- Reference counting
- Type system integration

Run tests with:
```bash
make test
```

For language binding tests:
```bash
make test-python  # Requires PyGI
```

## Contributing

1. Follow GObject coding style
2. Add tests for new features
3. Update documentation
4. Ensure introspection annotations are correct
5. Test with multiple language bindings

## License

This example code is provided for educational purposes. Adapt the license as needed for your project.

## References

- [GObject Reference Manual](https://docs.gtk.org/gobject/)
- [GObject Introspection](https://gi.readthedocs.io/)
- [GIR Format](https://gi.readthedocs.io/en/latest/gir/gir_format.html)
- [Writing GObject Classes](https://docs.gtk.org/gobject/tutorial.html)
- [Vala Documentation](https://wiki.gnome.org/Projects/Vala)