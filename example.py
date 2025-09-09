#!/usr/bin/env python3
"""
Python example demonstrating MyObject usage with GObject Introspection.

This script shows how to use a custom GObject from Python through
GObject Introspection bindings.
"""

import sys
import os

# Add the build directory to the path for our GIR files
build_path = os.path.join(os.path.dirname(__file__), 'build')
gir_path = os.path.join(build_path, 'gir')
typelib_path = os.path.join(build_path, 'typelib')
lib_path = os.path.join(build_path, 'lib')

# Set up the environment for our library
if 'GI_TYPELIB_PATH' in os.environ:
    os.environ['GI_TYPELIB_PATH'] = typelib_path + \
        ":" + os.environ['GI_TYPELIB_PATH']
else:
    os.environ['GI_TYPELIB_PATH'] = typelib_path

if 'LD_LIBRARY_PATH' in os.environ:
    os.environ['LD_LIBRARY_PATH'] = lib_path + \
        ":" + os.environ['LD_LIBRARY_PATH']
else:
    os.environ['LD_LIBRARY_PATH'] = lib_path

try:
    import gi
    gi.require_version('My', '1.0')
    from gi.repository import My, GObject
except ImportError as e:
    print(f"Error importing GObject Introspection: {e}")
    print("Make sure to build the project first with: make debug typelib")
    print("And ensure PyGI is installed: pip install PyGObject")
    sys.exit(1)


def on_value_changed(obj, new_value, user_data=None):
    """Signal handler for value-changed signal."""
    name = obj.get_name() or "Unnamed"
    print(f"🔔 Signal: '{name}' value changed to {new_value}")


def demonstrate_basic_usage():
    """Demonstrate basic object creation and usage."""
    print("=" * 50)
    print("BASIC USAGE DEMONSTRATION")
    print("=" * 50)

    # Create objects using different constructors
    obj1 = My.Object.new()
    obj2 = My.Object.new_with_value(42)

    print(f"obj1 initial value: {obj1.get_value()}")
    print(f"obj2 initial value: {obj2.get_value()}")

    # Set names
    obj1.set_name("Python Object 1")
    obj2.set_name("Python Object 2")

    # Connect to signals
    obj1.connect("value-changed", on_value_changed)
    obj2.connect("value-changed", on_value_changed)

    # Modify values to trigger signals
    print("\nModifying values...")
    obj1.set_value(100)
    obj2.increment()
    obj2.increment()

    # Display string representations
    print(f"\nObject 1: {obj1.to_string()}")
    print(f"Object 2: {obj2.to_string()}")


def demonstrate_properties():
    """Demonstrate GObject properties usage."""
    print("\n" + "=" * 50)
    print("PROPERTIES DEMONSTRATION")
    print("=" * 50)

    # Create object with properties
    obj = My.Object(value=25, name="Property Test")

    print(f"Initial state: {obj.to_string()}")

    # Access properties directly
    print(f"Direct property access - value: {obj.props.value}")
    print(f"Direct property access - name: {obj.props.name}")

    # Modify properties
    obj.props.value = 75
    obj.props.name = "Updated Name"

    print(f"After direct modification: {obj.to_string()}")

    # Using GObject.Object.get/set methods
    obj.set_property("value", 150)
    obj.set_property("name", "GObject Style")

    value = obj.get_property("value")
    name = obj.get_property("name")
    print(f"Using set_property/get_property: value={value}, name='{name}'")


def demonstrate_signals():
    """Demonstrate signal handling."""
    print("\n" + "=" * 50)
    print("SIGNALS DEMONSTRATION")
    print("=" * 50)

    obj = My.Object.new_with_value(10)
    obj.set_name("Signal Tester")

    # Multiple signal handlers
    def handler1(obj, new_value):
        print(f"  Handler 1: Received {new_value}")

    def handler2(obj, new_value):
        print(f"  Handler 2: Value is now {new_value}")

    # Connect multiple handlers
    obj.connect("value-changed", handler1)
    obj.connect("value-changed", handler2)

    print("Testing signal emission...")
    obj.increment()  # Should trigger both handlers
    obj.decrement()  # Should trigger both handlers
    obj.set_value(obj.get_value())  # Same value, should not trigger


def demonstrate_memory_management():
    """Demonstrate reference counting and memory management."""
    print("\n" + "=" * 50)
    print("MEMORY MANAGEMENT DEMONSTRATION")
    print("=" * 50)

    obj = My.Object.new_with_value(99)
    obj.set_name("Memory Test")

    print(f"Initial reference count: {obj.__grefcount__}")

    # Create additional references
    ref1 = obj
    ref2 = obj

    print(f"After creating references: {obj.__grefcount__}")

    # Delete references
    del ref1
    del ref2

    print(f"After deleting references: {obj.__grefcount__}")

    # Object will be automatically cleaned up when it goes out of scope


def demonstrate_type_system():
    """Demonstrate GObject type system integration."""
    print("\n" + "=" * 50)
    print("TYPE SYSTEM DEMONSTRATION")
    print("=" * 50)

    obj = My.Object.new()

    # Type information
    print(f"Object type: {type(obj)}")
    print(f"GType name: {obj.__gtype__.name}")
    print(f"Is GObject: {isinstance(obj, GObject.Object)}")
    print(f"Is MyObject: {isinstance(obj, My.Object)}")

    # List all properties
    print("\nObject properties:")
    for prop in obj.props:
        prop_name = prop.name
        prop_value = getattr(obj.props, prop_name)
        print(f"  {prop_name}: {prop_value}")

    # List all signals
    print("\nObject signals:")
    signal_ids = GObject.signal_list_ids(obj)
    for signal_id in signal_ids:
        signal_name = GObject.signal_name(signal_id)
        print(f"  {signal_name}")


def stress_test():
    """Perform a simple stress test."""
    print("\n" + "=" * 50)
    print("STRESS TEST")
    print("=" * 50)

    objects = []
    num_objects = 1000

    print(f"Creating {num_objects} objects...")
    for i in range(num_objects):
        obj = My.Object.new_with_value(i)
        obj.set_name(f"Object #{i}")
        objects.append(obj)

    print(f"Created {len(objects)} objects")

    print("Testing methods on all objects...")
    total_value = 0
    for obj in objects:
        obj.increment()
        total_value += obj.get_value()

    print(f"Total value across all objects: {total_value}")

    print("Cleaning up...")
    objects.clear()  # This should trigger garbage collection

    print("Stress test completed successfully!")


def main():
    """Main demonstration function."""
    print("🐍 Python GObject Introspection Demo for MyObject")
    print("This demonstrates how to use C GObjects from Python\n")

    try:
        demonstrate_basic_usage()
        demonstrate_properties()
        demonstrate_signals()
        demonstrate_memory_management()
        demonstrate_type_system()
        stress_test()

        print("\n" + "=" * 50)
        print("🎉 All demonstrations completed successfully!")
        print("=" * 50)

    except Exception as e:
        print(f"\n❌ Error during demonstration: {e}")
        print("Make sure the library is built and installed correctly.")
        sys.exit(1)


if __name__ == "__main__":
    main()
