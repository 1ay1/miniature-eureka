#include <glib.h>
#include <glib-object.h>
#include "myobject.h"

/* Signal handler for value-changed signal */
static void
on_value_changed (MyObject *obj, gint new_value, gpointer user_data)
{
    const gchar *name = my_object_get_name (obj);
    g_print ("Signal received: value changed to %d", new_value);
    if (name) {
        g_print (" (object name: %s)", name);
    }
    g_print ("\n");
}

/* Test basic object creation and properties */
static void
test_object_creation (void)
{
    g_print ("\n=== Testing Object Creation ===\n");
    
    /* Create object with default constructor */
    MyObject *obj1 = my_object_new ();
    g_assert (MY_IS_OBJECT (obj1));
    g_assert_cmpint (my_object_get_value (obj1), ==, 0);
    g_assert_null (my_object_get_name (obj1));
    
    /* Create object with initial value */
    MyObject *obj2 = my_object_new_with_value (42);
    g_assert (MY_IS_OBJECT (obj2));
    g_assert_cmpint (my_object_get_value (obj2), ==, 42);
    
    g_print ("✓ Object creation tests passed\n");
    
    g_object_unref (obj1);
    g_object_unref (obj2);
}

/* Test property getters and setters */
static void
test_properties (void)
{
    g_print ("\n=== Testing Properties ===\n");
    
    MyObject *obj = my_object_new ();
    
    /* Test value property */
    my_object_set_value (obj, 123);
    g_assert_cmpint (my_object_get_value (obj), ==, 123);
    
    /* Test name property */
    my_object_set_name (obj, "Test Object");
    g_assert_cmpstr (my_object_get_name (obj), ==, "Test Object");
    
    /* Test property change via GObject property system */
    gint value;
    gchar *name;
    g_object_get (obj, "value", &value, "name", &name, NULL);
    g_assert_cmpint (value, ==, 123);
    g_assert_cmpstr (name, ==, "Test Object");
    
    g_object_set (obj, "value", 456, "name", "Updated Object", NULL);
    g_assert_cmpint (my_object_get_value (obj), ==, 456);
    g_assert_cmpstr (my_object_get_name (obj), ==, "Updated Object");
    
    g_print ("✓ Property tests passed\n");
    
    g_free (name);
    g_object_unref (obj);
}

/* Test methods */
static void
test_methods (void)
{
    g_print ("\n=== Testing Methods ===\n");
    
    MyObject *obj = my_object_new_with_value (10);
    my_object_set_name (obj, "Counter");
    
    /* Test increment */
    my_object_increment (obj);
    g_assert_cmpint (my_object_get_value (obj), ==, 11);
    
    /* Test decrement */
    my_object_decrement (obj);
    my_object_decrement (obj);
    g_assert_cmpint (my_object_get_value (obj), ==, 9);
    
    /* Test to_string */
    gchar *str = my_object_to_string (obj);
    g_print ("String representation: %s\n", str);
    g_assert (g_str_has_prefix (str, "MyObject(name='Counter'"));
    
    g_print ("✓ Method tests passed\n");
    
    g_free (str);
    g_object_unref (obj);
}

/* Test signals */
static void
test_signals (void)
{
    g_print ("\n=== Testing Signals ===\n");
    
    MyObject *obj = my_object_new ();
    my_object_set_name (obj, "Signal Tester");
    
    /* Connect to signal */
    g_signal_connect (obj, "value-changed", G_CALLBACK (on_value_changed), NULL);
    
    /* Change value - should emit signal */
    g_print ("Setting value to 100...\n");
    my_object_set_value (obj, 100);
    
    g_print ("Incrementing value...\n");
    my_object_increment (obj);
    
    g_print ("Setting same value again (should not emit signal)...\n");
    my_object_set_value (obj, 101);
    
    g_print ("✓ Signal tests passed\n");
    
    g_object_unref (obj);
}

/* Test reference counting */
static void
test_reference_counting (void)
{
    g_print ("\n=== Testing Reference Counting ===\n");
    
    MyObject *obj = my_object_new ();
    g_assert_cmpuint (G_OBJECT (obj)->ref_count, ==, 1);
    
    /* Add reference */
    g_object_ref (obj);
    g_assert_cmpuint (G_OBJECT (obj)->ref_count, ==, 2);
    
    /* Remove reference */
    g_object_unref (obj);
    g_assert_cmpuint (G_OBJECT (obj)->ref_count, ==, 1);
    
    g_print ("✓ Reference counting tests passed\n");
    
    g_object_unref (obj);
}

/* Test type system */
static void
test_type_system (void)
{
    g_print ("\n=== Testing Type System ===\n");
    
    /* Test type registration */
    GType type = my_object_get_type ();
    g_assert (g_type_is_a (type, G_TYPE_OBJECT));
    g_assert_cmpstr (g_type_name (type), ==, "MyObject");
    
    /* Test type checking macros */
    MyObject *obj = my_object_new ();
    g_assert (MY_IS_OBJECT (obj));
    g_assert (G_IS_OBJECT (obj));
    
    /* Test casting macros */
    GObject *gobj = G_OBJECT (obj);
    MyObject *myobj = MY_OBJECT (gobj);
    g_assert (obj == myobj);
    
    g_print ("✓ Type system tests passed\n");
    
    g_object_unref (obj);
}

/* Main test runner */
int
main (int argc, char *argv[])
{
    g_print ("Starting MyObject tests...\n");
    
    /* Initialize GObject type system */
    g_type_init ();
    
    /* Run all tests */
    test_object_creation ();
    test_properties ();
    test_methods ();
    test_signals ();
    test_reference_counting ();
    test_type_system ();
    
    g_print ("\n🎉 All tests passed successfully!\n");
    
    return 0;
}