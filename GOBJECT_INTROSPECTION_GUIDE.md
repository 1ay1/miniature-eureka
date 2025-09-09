# Complete Guide to Creating GObjects with GObject Introspection

This guide provides a comprehensive explanation of how to create GObjects that work seamlessly with GObject Introspection (GI), enabling automatic language bindings for Python, JavaScript, Vala, and other languages.

## Table of Contents

1. [Overview](#overview)
2. [Core Components](#core-components)
3. [Implementation Details](#implementation-details)
4. [Build System](#build-system)
5. [Language Bindings](#language-bindings)
6. [Best Practices](#best-practices)
7. [Troubleshooting](#troubleshooting)

## Overview

GObject Introspection is a middleware layer between C libraries and language bindings. It provides:

- **Runtime introspection**: Discover object structure, methods, and properties at runtime
- **Automatic bindings**: Generate language bindings for Python, JavaScript, Vala, etc.
- **Type safety**: Strong typing across language boundaries
- **Memory management**: Proper reference counting and garbage collection

### Architecture

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   C Library     │───→│  GIR File        │───→│  Typelib File   │
│  (myobject.so)  │    │  (My-1.0.gir)   │    │ (My-1.0.typelib)│
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                       │                       │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   C Program     │    │   Development    │    │  Runtime Usage  │
│                 │    │   (gtk-doc)      │    │  (Python, JS)   │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

## Core Components

### 1. Header File (`myobject.h`)

The header file defines the public API and must follow GObject conventions:

```c
#ifndef MY_OBJECT_H
#define MY_OBJECT_H

#include <glib-object.h>

G_BEGIN_DECLS

// Type macros - essential for GObject type system
#define MY_TYPE_OBJECT (my_object_get_type())
#define MY_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), MY_TYPE_OBJECT, MyObject))
#define MY_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), MY_TYPE_OBJECT, MyObjectClass))
#define MY_IS_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MY_TYPE_OBJECT))
#define MY_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MY_TYPE_OBJECT))
#define MY_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), MY_TYPE_OBJECT, MyObjectClass))

// Forward declarations
typedef struct _MyObject MyObject;
typedef struct _MyObjectClass MyObjectClass;
typedef struct _MyObjectPrivate MyObjectPrivate;

// Instance structure
struct _MyObject {
    GObject parent_instance;
    /*< private >*/
    MyObjectPrivate *priv;
};

// Class structure
struct _MyObjectClass {
    GObjectClass parent_class;
    
    // Virtual methods for subclassing
    void (*value_changed) (MyObject *self, gint new_value);
    
    /*< private >*/
    gpointer padding[12]; // For ABI stability
};

// Essential function - registers the type
GType my_object_get_type (void) G_GNUC_CONST;

// Public API with introspection annotations
MyObject *my_object_new (void);
MyObject *my_object_new_with_value (gint initial_value);

G_END_DECLS

#endif
```

**Key Points:**
- Use `G_BEGIN_DECLS`/`G_END_DECLS` for C++ compatibility
- Define type macros following GObject conventions
- Use forward declarations to avoid circular dependencies
- Mark private data with `/*< private >*/` comments
- Add padding to class structure for ABI stability

### 2. Implementation File (`myobject.c`)

The implementation must follow GObject patterns and include introspection documentation:

```c
#include "myobject.h"

/**
 * SECTION:myobject
 * @short_description: Example GObject with introspection
 * @title: MyObject
 * @stability: Stable
 * @include: myobject.h
 *
 * MyObject demonstrates proper GObject implementation with
 * full GObject Introspection support.
 */

// Private structure - not exposed in header
struct _MyObjectPrivate {
    gint value;
    gchar *name;
};

// Property enumeration
enum {
    PROP_0,
    PROP_VALUE,
    PROP_NAME,
    N_PROPERTIES
};

// Signal enumeration
enum {
    VALUE_CHANGED,
    LAST_SIGNAL
};

static GParamSpec *properties[N_PROPERTIES] = { NULL };
static guint signals[LAST_SIGNAL] = { 0 };

// Register type with private data
G_DEFINE_TYPE_WITH_PRIVATE (MyObject, my_object, G_TYPE_OBJECT)
```

**Key Implementation Details:**

#### Property System
```c
static void
my_object_class_init (MyObjectClass *klass)
{
    // Install properties with proper specifications
    properties[PROP_VALUE] = 
        g_param_spec_int ("value",
                         "Value",
                         "The integer value stored in the object",
                         G_MININT, G_MAXINT, 0,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
    
    g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}
```

#### Signal System
```c
// Define signals with proper marshalling
signals[VALUE_CHANGED] = 
    g_signal_new ("value-changed",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST,
                 G_STRUCT_OFFSET (MyObjectClass, value_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__INT,
                 G_TYPE_NONE, 1, G_TYPE_INT);
```

#### Introspection Annotations
```c
/**
 * my_object_new_with_value:
 * @initial_value: the initial value to set
 *
 * Creates a new #MyObject with the specified initial value.
 *
 * Returns: (transfer full): a new #MyObject
 */
MyObject *
my_object_new_with_value (gint initial_value)
{
    return g_object_new (MY_TYPE_OBJECT, "value", initial_value, NULL);
}
```

**Common Annotations:**
- `(transfer full)`: Caller owns the returned reference
- `(transfer none)`: Caller doesn't own the returned reference
- `(nullable)`: Parameter or return value can be NULL
- `(array)`: Parameter is an array
- `(element-type)`: Specifies container element type
- `(in)`, `(out)`, `(inout)`: Parameter directions

### 3. Build System Integration

#### Makefile with GIR Generation
```makefile
# Generate GIR file
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

# Generate typelib from GIR
$(TYPELIB_FILE): $(GIR_FILE)
	$(GI_COMPILER) --output=$@ $<
```

**Build Process:**
1. **Compile C library** → `libmyobject.so`
2. **Generate GIR file** → `My-1.0.gir` (XML metadata)
3. **Compile typelib** → `My-1.0.typelib` (binary metadata)
4. **Install files** → System directories for runtime access

## Language Bindings

### Python (PyGObject)

```python
import gi
gi.require_version('My', '1.0')
from gi.repository import My

# Create objects
obj = My.Object.new_with_value(42)
obj.set_name("Python Object")

# Connect to signals
def on_value_changed(obj, new_value):
    print(f"Value changed to: {new_value}")

obj.connect("value-changed", on_value_changed)

# Use properties
obj.props.value = 100
print(f"Value: {obj.props.value}")

# Call methods
obj.increment()
print(obj.to_string())
```

### JavaScript (GJS)

```javascript
imports.gi.versions.My = '1.0';
const { My } = imports.gi;

// Create objects
let obj = new My.Object({ value: 42, name: "JS Object" });

// Connect to signals
obj.connect('value-changed', (obj, newValue) => {
    print(`Value changed to: ${newValue}`);
});

// Use properties
obj.value = 100;
print(`Value: ${obj.value}`);

// Call methods
obj.increment();
print(obj.to_string());
```

### Vala

```vala
using My;

int main() {
    var obj = new My.Object.with_value(42);
    obj.name = "Vala Object";
    
    obj.value_changed.connect((new_value) => {
        stdout.printf("Value changed to: %d\n", new_value);
    });
    
    obj.value = 100;
    obj.increment();
    stdout.printf("%s\n", obj.to_string());
    
    return 0;
}
```

## Best Practices

### 1. API Design

- **Follow GObject conventions**: Use consistent naming patterns
- **Design for introspection**: Consider how APIs will look in other languages
- **Use properties**: Make object state accessible via GObject properties
- **Emit signals**: Notify about important state changes
- **Document everything**: Use gtk-doc style comments

### 2. Memory Management

```c
// Proper dispose implementation
static void
my_object_dispose (GObject *object)
{
    MyObject *self = MY_OBJECT (object);
    
    // Release references to other objects
    g_clear_object (&self->priv->some_object);
    
    // Chain up to parent
    G_OBJECT_CLASS (my_object_parent_class)->dispose (object);
}

// Proper finalize implementation
static void
my_object_finalize (GObject *object)
{
    MyObject *self = MY_OBJECT (object);
    
    // Free allocated memory
    g_free (self->priv->name);
    
    // Chain up to parent
    G_OBJECT_CLASS (my_object_parent_class)->finalize (object);
}
```

### 3. Error Handling

```c
// Use return values for error indication
gboolean
my_object_save_to_file (MyObject *self, 
                       const gchar *filename,
                       GError **error)
{
    g_return_val_if_fail (MY_IS_OBJECT (self), FALSE);
    g_return_val_if_fail (filename != NULL, FALSE);
    g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
    
    // Implementation...
    
    return TRUE;
}
```

### 4. Thread Safety

```c
// Use thread-safe property access
G_LOCK_DEFINE (my_object_lock);

static void
my_object_set_value_thread_safe (MyObject *self, gint value)
{
    g_return_if_fail (MY_IS_OBJECT (self));
    
    G_LOCK (my_object_lock);
    
    if (self->priv->value != value) {
        self->priv->value = value;
        g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_VALUE]);
    }
    
    G_UNLOCK (my_object_lock);
}
```

## Troubleshooting

### Common Build Issues

1. **Missing headers**
```bash
# Install development packages
sudo apt-get install libglib2.0-dev gobject-introspection libgirepository1.0-dev
```

2. **g-ir-scanner not found**
```bash
# Install introspection tools
sudo apt-get install gobject-introspection-dev
```

3. **Library not found at runtime**
```bash
# Set library path
export LD_LIBRARY_PATH=/path/to/lib:$LD_LIBRARY_PATH
export GI_TYPELIB_PATH=/path/to/typelib:$GI_TYPELIB_PATH
```

### Runtime Issues

1. **Symbol not found**
   - Check that library exports all symbols
   - Verify GIR file contains expected functions
   - Ensure typelib is properly generated

2. **Type not registered**
   - Verify `get_type()` function is called
   - Check type registration in module init
   - Ensure proper linking

3. **Memory leaks**
   - Implement proper dispose/finalize methods
   - Use weak references where appropriate
   - Test with valgrind

### Debugging Introspection

1. **Inspect GIR file**
```bash
cat build/gir/My-1.0.gir | grep -A 5 -B 5 "function name"
```

2. **Verify typelib**
```bash
g-ir-inspect --print-typelibs My
```

3. **Test with Python**
```python
import gi
gi.require_version('My', '1.0')
from gi.repository import My
help(My.Object)
```

## Advanced Topics

### Custom Boxed Types

```c
// Define boxed type for complex data structures
typedef struct {
    gchar *name;
    gint age;
} MyPersonData;

static MyPersonData *
my_person_data_copy (MyPersonData *data)
{
    MyPersonData *copy = g_slice_new (MyPersonData);
    copy->name = g_strdup (data->name);
    copy->age = data->age;
    return copy;
}

static void
my_person_data_free (MyPersonData *data)
{
    g_free (data->name);
    g_slice_free (MyPersonData, data);
}

G_DEFINE_BOXED_TYPE (MyPersonData, my_person_data, 
                     my_person_data_copy, my_person_data_free)
```

### Interface Implementation

```c
// Define interface
#define MY_TYPE_DRAWABLE (my_drawable_get_type())
G_DECLARE_INTERFACE (MyDrawable, my_drawable, MY, DRAWABLE, GObject)

struct _MyDrawableInterface {
    GTypeInterface parent_iface;
    void (*draw) (MyDrawable *self, cairo_t *cr);
};

// Implement interface in your object
static void my_object_drawable_init (MyDrawableInterface *iface);

G_DEFINE_TYPE_WITH_CODE (MyObject, my_object, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (MY_TYPE_DRAWABLE,
                                               my_object_drawable_init))
```

### Async Operations

```c
// Async method with GTask
void
my_object_compute_async (MyObject *self,
                        GCancellable *cancellable,
                        GAsyncReadyCallback callback,
                        gpointer user_data)
{
    GTask *task = g_task_new (self, cancellable, callback, user_data);
    
    // Set up async operation
    g_task_run_in_thread (task, compute_thread_func);
    g_object_unref (task);
}

gint
my_object_compute_finish (MyObject *self,
                         GAsyncResult *result,
                         GError **error)
{
    return g_task_propagate_int (G_TASK (result), error);
}
```

## Conclusion

Creating GObjects with proper introspection support involves:

1. **Following GObject conventions** for type system integration
2. **Using proper annotations** for API documentation
3. **Implementing complete build chain** from C to typelib
4. **Testing across multiple languages** to ensure compatibility
5. **Following best practices** for memory management and thread safety

The result is a C library that can be seamlessly used from multiple programming languages with automatic bindings, proper type safety, and native-feeling APIs.

This approach enables you to write performance-critical code in C while providing easy-to-use bindings for rapid application development in higher-level languages.