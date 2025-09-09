#ifndef MY_OBJECT_H
#define MY_OBJECT_H

#include <glib-object.h>

G_BEGIN_DECLS

#define MY_TYPE_OBJECT (my_object_get_type())
#define MY_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), MY_TYPE_OBJECT, MyObject))
#define MY_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), MY_TYPE_OBJECT, MyObjectClass))
#define MY_IS_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MY_TYPE_OBJECT))
#define MY_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MY_TYPE_OBJECT))
#define MY_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), MY_TYPE_OBJECT, MyObjectClass))

typedef struct _MyObject MyObject;
typedef struct _MyObjectClass MyObjectClass;
typedef struct _MyObjectPrivate MyObjectPrivate;

/**
 * MyObject:
 *
 * An example GObject class that demonstrates proper GObject implementation
 * with GObject Introspection support.
 */
struct _MyObject {
    GObject parent_instance;
    
    /*< private >*/
    MyObjectPrivate *priv;
};

/**
 * MyObjectClass:
 * @parent_class: The parent class
 * @value_changed: Virtual method called when value changes
 *
 * The class structure for MyObject.
 */
struct _MyObjectClass {
    GObjectClass parent_class;
    
    /* Virtual methods */
    void (*value_changed) (MyObject *self, gint new_value);
    
    /*< private >*/
    gpointer padding[12];
};

GType my_object_get_type (void) G_GNUC_CONST;

/* Constructors */
MyObject *my_object_new (void);
MyObject *my_object_new_with_value (gint initial_value);

/* Property getters/setters */
void my_object_set_value (MyObject *self, gint value);
gint my_object_get_value (MyObject *self);

void my_object_set_name (MyObject *self, const gchar *name);
const gchar *my_object_get_name (MyObject *self);

/* Methods */
void my_object_increment (MyObject *self);
void my_object_decrement (MyObject *self);
gchar *my_object_to_string (MyObject *self);

/* Signals */
void my_object_emit_value_changed (MyObject *self, gint new_value);

G_END_DECLS

#endif /* MY_OBJECT_H */