#!/usr/bin/env gjs

/**
 * JavaScript example demonstrating MyObject usage with GObject Introspection.
 *
 * This script shows how to use a custom GObject from JavaScript through
 * GObject Introspection bindings using GJS (GNOME JavaScript).
 *
 * To run this example:
 * 1. Make sure the library is built: make debug typelib
 * 2. Set environment variables:
 *    export LD_LIBRARY_PATH=$PWD/build/lib:$LD_LIBRARY_PATH
 *    export GI_TYPELIB_PATH=$PWD/build/typelib:$GI_TYPELIB_PATH
 * 3. Run with: gjs example.js
 */

imports.gi.versions.GObject = "2.0";
imports.gi.versions.GLib = "2.0";

const { GObject, GLib } = imports.gi;

// Import our custom library
let My;
try {
  My = imports.gi.My;
} catch (e) {
  print("❌ Error importing My module:", e.message);
  print("Make sure to:");
  print("1. Build the project: make debug typelib");
  print("2. Set LD_LIBRARY_PATH to include build/lib");
  print("3. Set GI_TYPELIB_PATH to include build/typelib");
  imports.system.exit(1);
}

/**
 * Signal handler for value-changed signal
 */
function onValueChanged(obj, newValue) {
  let name = obj.get_name() || "Unnamed";
  print(`🔔 Signal: '${name}' value changed to ${newValue}`);
}

/**
 * Demonstrate basic object creation and usage
 */
function demonstrateBasicUsage() {
  print("=".repeat(50));
  print("BASIC USAGE DEMONSTRATION");
  print("=".repeat(50));

  // Create objects using different constructors
  let obj1 = new My.Object();
  let obj2 = My.Object.new_with_value(42);

  print(`obj1 initial value: ${obj1.get_value()}`);
  print(`obj2 initial value: ${obj2.get_value()}`);

  // Set names
  obj1.set_name("JavaScript Object 1");
  obj2.set_name("JavaScript Object 2");

  // Connect to signals
  obj1.connect("value-changed", onValueChanged);
  obj2.connect("value-changed", onValueChanged);

  // Modify values to trigger signals
  print("\nModifying values...");
  obj1.set_value(100);
  obj2.increment();
  obj2.increment();

  // Display string representations
  print(`\nObject 1: ${obj1.to_string()}`);
  print(`Object 2: ${obj2.to_string()}`);
}

/**
 * Demonstrate GObject properties usage
 */
function demonstrateProperties() {
  print("\n" + "=".repeat(50));
  print("PROPERTIES DEMONSTRATION");
  print("=".repeat(50));

  // Create object with properties
  let obj = new My.Object({
    value: 25,
    name: "Property Test",
  });

  print(`Initial state: ${obj.to_string()}`);

  // Access properties directly
  print(`Direct property access - value: ${obj.value}`);
  print(`Direct property access - name: ${obj.name}`);

  // Modify properties
  obj.value = 75;
  obj.name = "Updated Name";

  print(`After direct modification: ${obj.to_string()}`);

  // Using GObject property methods
  obj.set_property("value", 150);
  obj.set_property("name", "GObject Style");

  let value = obj.value;
  let name = obj.name;
  print(`Using set_property/get_property: value=${value}, name='${name}'`);
}

/**
 * Demonstrate signal handling
 */
function demonstrateSignals() {
  print("\n" + "=".repeat(50));
  print("SIGNALS DEMONSTRATION");
  print("=".repeat(50));

  let obj = My.Object.new_with_value(10);
  obj.set_name("Signal Tester");

  // Multiple signal handlers
  function handler1(obj, newValue) {
    print(`  Handler 1: Received ${newValue}`);
  }

  function handler2(obj, newValue) {
    print(`  Handler 2: Value is now ${newValue}`);
  }

  // Connect multiple handlers
  obj.connect("value-changed", handler1);
  obj.connect("value-changed", handler2);

  print("Testing signal emission...");
  obj.increment(); // Should trigger both handlers
  obj.decrement(); // Should trigger both handlers
  obj.set_value(obj.get_value()); // Same value, should not trigger
}

/**
 * Demonstrate memory management and reference counting
 */
function demonstrateMemoryManagement() {
  print("\n" + "=".repeat(50));
  print("MEMORY MANAGEMENT DEMONSTRATION");
  print("=".repeat(50));

  let obj = My.Object.new_with_value(99);
  obj.set_name("Memory Test");

  print(`Object created: ${obj.to_string()}`);

  // JavaScript uses garbage collection, so manual reference management
  // is not needed like in C. The GObject will be automatically cleaned up
  // when no more references exist.

  // Create additional references
  let ref1 = obj;
  let ref2 = obj;

  print("Created additional references (ref1, ref2)");

  // Clear references (in real code, these would go out of scope)
  ref1 = null;
  ref2 = null;

  print("Cleared additional references");
  print("Object will be garbage collected when 'obj' goes out of scope");
}

/**
 * Demonstrate type system integration
 */
function demonstrateTypeSystem() {
  print("\n" + "=".repeat(50));
  print("TYPE SYSTEM DEMONSTRATION");
  print("=".repeat(50));

  let obj = new My.Object();

  // Type information
  print(`Object constructor: ${obj.constructor.name}`);
  print(`Object toString: ${obj.toString()}`);
  print(`Is GObject: ${obj instanceof GObject.Object}`);

  // List properties
  print("\nObject properties:");
  print(`  value: ${obj.value}`);
  print(`  name: ${obj.name}`);

  // List signals
  print("\nObject signals:");
  let signals = GObject.signal_list_ids(obj.constructor.$gtype);
  for (let signalId of signals) {
    let signalName = GObject.signal_name(signalId);
    print(`  ${signalName}`);
  }
}

/**
 * Demonstrate array handling and more complex operations
 */
function demonstrateAdvancedFeatures() {
  print("\n" + "=".repeat(50));
  print("ADVANCED FEATURES DEMONSTRATION");
  print("=".repeat(50));

  // Create multiple objects and demonstrate collection operations
  let objects = [];
  let numObjects = 10;

  print(`Creating ${numObjects} objects...`);
  for (let i = 0; i < numObjects; i++) {
    let obj = My.Object.new_with_value(i * 10);
    obj.set_name(`Object #${i}`);
    objects.push(obj);
  }

  print(`Created ${objects.length} objects`);

  // Perform operations on all objects
  print("Incrementing all objects...");
  let totalValue = 0;
  objects.forEach((obj, index) => {
    obj.increment();
    totalValue += obj.get_value();
    print(`  ${obj.get_name()}: ${obj.get_value()}`);
  });

  print(`Total value across all objects: ${totalValue}`);

  // Demonstrate method chaining-like operations
  print("\nDemonstrating method calls:");
  let testObj = new My.Object();
  testObj.set_name("Chained Operations");
  testObj.set_value(0);

  // JavaScript doesn't have method chaining built-in for our object,
  // but we can simulate it
  for (let i = 0; i < 5; i++) {
    testObj.increment();
  }

  print(`After 5 increments: ${testObj.to_string()}`);
}

/**
 * Performance test
 */
function performanceTest() {
  print("\n" + "=".repeat(50));
  print("PERFORMANCE TEST");
  print("=".repeat(50));

  let numOperations = 10000;
  let startTime = Date.now();

  let obj = new My.Object();

  print(`Performing ${numOperations} operations...`);
  for (let i = 0; i < numOperations; i++) {
    obj.set_value(i);
    obj.get_value();
    if (i % 2 === 0) {
      obj.increment();
    } else {
      obj.decrement();
    }
  }

  let endTime = Date.now();
  let duration = endTime - startTime;
  let opsPerSecond = Math.round(numOperations / (duration / 1000));

  print(`Completed ${numOperations} operations in ${duration}ms`);
  print(`Performance: ~${opsPerSecond} operations per second`);
  print(`Final value: ${obj.get_value()}`);
}

/**
 * Main demonstration function
 */
function main() {
  print("🚀 JavaScript GObject Introspection Demo for MyObject");
  print("This demonstrates how to use C GObjects from JavaScript using GJS\n");

  try {
    demonstrateBasicUsage();
    demonstrateProperties();
    demonstrateSignals();
    demonstrateMemoryManagement();
    demonstrateTypeSystem();
    demonstrateAdvancedFeatures();
    performanceTest();

    print("\n" + "=".repeat(50));
    print("🎉 All demonstrations completed successfully!");
    print("=".repeat(50));
  } catch (e) {
    print(`\n❌ Error during demonstration: ${e.message}`);
    print("Stack trace:", e.stack);
    print(
      "Make sure the library is built and the environment is set up correctly.",
    );
    imports.system.exit(1);
  }
}

// Run the demonstration
main();
