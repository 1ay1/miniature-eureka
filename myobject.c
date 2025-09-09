#include "myobject.h"
#include <string.h>

/**
 * SECTION:myobject
 * @short_description: An example GObject implementation
 * @title: MyObject
 * @stability: Stable
 * @include: myobject.h
 *
 * MyObject is an example implementation of a GObject that demonstrates
 * proper GObject patterns including properties, signals, and methods.
 * This object is designed to work with GObject Introspection.
 */

/* Private structure */
struct _MyObjectPrivate {
    gint value;
    gchar *name;
};

/* Property enumeration */
enum {
    PROP_0,
    PROP_VALUE,
    PROP_NAME,
    N_PROPERTIES
};

/* Signal enumeration */
enum {
    VALUE_CHANGED,
    LAST_SIGNAL
};

static GParamSpec *properties[N_PROPERTIES] = { NULL };
static guint signals[LAST_SIGNAL] = { 0 };

/* GObject boilerplate */
G_DEFINE_TYPE_WITH_PRIVATE (MyObject, my_object, G_TYPE_OBJECT)

/* Forward declarations */
static void my_object_dispose (GObject *object);
static void my_object_finalize (GObject *object);
static void my_object_get_property (GObject *object,
                                    guint prop_id,
                                    GValue *value,
                                    GParamSpec *pspec);
static void my_object_set_property (GObject *object,
                                    guint prop_id,
                                    const GValue *value,
                                    GParamSpec *pspec);

/* Class initialization */
static void
my_object_class_init (MyObjectClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    
    object_class->dispose = my_object_dispose;
    object_class->finalize = my_object_finalize;
    object_class->get_property = my_object_get_property;
    object_class->set_property = my_object_set_property;
    
    /* Install properties */
    properties[PROP_VALUE] = 
        g_param_spec_int ("value",
                         "Value",
                         "The integer value stored in the object",
                         G_MININT,
                         G_MAXINT,
                         0,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
    
    properties[PROP_NAME] = 
        g_param_spec_string ("name",
                            "Name",
                            "The name of the object",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);
    
    g_object_class_install_properties (object_class, N_PROPERTIES, properties);
    
    /* Install signals */
    /**
     * MyObject::value-changed:
     * @self: the #MyObject instance
     * @new_value: the new value
     *
     * Emitted when the value property changes.
     */
    signals[VALUE_CHANGED] = 
        g_signal_new ("value-changed",
                     G_TYPE_FROM_CLASS (klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET (MyObjectClass, value_changed),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__INT,
                     G_TYPE_NONE,
                     1,
                     G_TYPE_INT);
}

/* Instance initialization */
static void
my_object_init (MyObject *self)
{
    self->priv = my_object_get_instance_private (self);
    self->priv->value = 0;
    self->priv->name = NULL;
}

/* Dispose method - release references to other objects */
static void
my_object_dispose (GObject *object)
{
    MyObject *self = MY_OBJECT (object);
    
    /* Chain up to parent class */
    G_OBJECT_CLASS (my_object_parent_class)->dispose (object);
}

/* Finalize method - free allocated memory */
static void
my_object_finalize (GObject *object)
{
    MyObject *self = MY_OBJECT (object);
    
    g_free (self->priv->name);
    
    /* Chain up to parent class */
    G_OBJECT_CLASS (my_object_parent_class)->finalize (object);
}

/* Property getter */
static void
my_object_get_property (GObject *object,
                        guint prop_id,
                        GValue *value,
                        GParamSpec *pspec)
{
    MyObject *self = MY_OBJECT (object);
    
    switch (prop_id) {
        case PROP_VALUE:
            g_value_set_int (value, self->priv->value);
            break;
        case PROP_NAME:
            g_value_set_string (value, self->priv->name);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

/* Property setter */
static void
my_object_set_property (GObject *object,
                        guint prop_id,
                        const GValue *value,
                        GParamSpec *pspec)
{
    MyObject *self = MY_OBJECT (object);
    
    switch (prop_id) {
        case PROP_VALUE:
            my_object_set_value (self, g_value_get_int (value));
            break;
        case PROP_NAME:
            my_object_set_name (self, g_value_get_string (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

/* Public API implementation */

/**
 * my_object_new:
 *
 * Creates a new #MyObject instance.
 *
 * Returns: (transfer full): a new #MyObject
 */
MyObject *
my_object_new (void)
{
    return g_object_new (MY_TYPE_OBJECT, NULL);
}

/**
 * my_object_new_with_value:
 * @initial_value: the initial value to set
 *
 * Creates a new #MyObject instance with the specified initial value.
 *
 * Returns: (transfer full): a new #MyObject
 */
MyObject *
my_object_new_with_value (gint initial_value)
{
    return g_object_new (MY_TYPE_OBJECT, "value", initial_value, NULL);
}

/**
 * my_object_set_value:
 * @self: a #MyObject
 * @value: the new value to set
 *
 * Sets the value property of the object.
 */
void
my_object_set_value (MyObject *self, gint value)
{
    g_return_if_fail (MY_IS_OBJECT (self));
    
    if (self->priv->value != value) {
        gint old_value = self->priv->value;
        self->priv->value = value;
        
        /* Notify property change */
        g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_VALUE]);
        
        /* Emit signal */
        g_signal_emit (self, signals[VALUE_CHANGED], 0, value);
    }
}

/**
 * my_object_get_value:
 * @self: a #MyObject
 *
 * Gets the current value of the object.
 *
 * Returns: the current value
 */
gint
my_object_get_value (MyObject *self)
{
    g_return_val_if_fail (MY_IS_OBJECT (self), 0);
    
    return self->priv->value;
}

/**
 * my_object_set_name:
 * @self: a #MyObject
 * @name: (nullable): the new name to set
 *
 * Sets the name property of the object.
 */
void
my_object_set_name (MyObject *self, const gchar *name)
{
    g_return_if_fail (MY_IS_OBJECT (self));
    
    if (g_strcmp0 (self->priv->name, name) != 0) {
        g_free (self->priv->name);
        self->priv->name = g_strdup (name);
        
        /* Notify property change */
        g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_NAME]);
    }
}

/**
 * my_object_get_name:
 * @self: a #MyObject
 *
 * Gets the current name of the object.
 *
 * Returns: (nullable): the current name or %NULL
 */
const gchar *
my_object_get_name (MyObject *self)
{
    g_return_val_if_fail (MY_IS_OBJECT (self), NULL);
    
    return self->priv->name;
}

/**
 * my_object_increment:
 * @self: a #MyObject
 *
 * Increments the value by 1.
 */
void
my_object_increment (MyObject *self)
{
    g_return_if_fail (MY_IS_OBJECT (self));
    
    my_object_set_value (self, self->priv->value + 1);
}

/**
 * my_object_decrement:
 * @self: a #MyObject
 *
 * Decrements the value by 1.
 */
void
my_object_decrement (MyObject *self)
{
    g_return_if_fail (MY_IS_OBJECT (self));
    
    my_object_set_value (self, self->priv->value - 1);
}

/**
 * my_object_to_string:
 * @self: a #MyObject
 *
 * Creates a string representation of the object.
 *
 * Returns: (transfer full): a newly allocated string representation
 */
gchar *
my_object_to_string (MyObject *self)
{
    g_return_val_if_fail (MY_IS_OBJECT (self), NULL);
    
    if (self->priv->name) {
        return g_strdup_printf ("MyObject(name='%s', value=%d)",
                               self->priv->name,
                               self->priv->value);
    } else {
        return g_strdup_printf ("MyObject(value=%d)", self->priv->value);
    }
}

/**
 * my_object_emit_value_changed:
 * @self: a #MyObject
 * @new_value: the new value
 *
 * Explicitly emits the value-changed signal.
 * This is typically called internally when the value changes.
 */
void
my_object_emit_value_changed (MyObject *self, gint new_value)
{
    g_return_if_fail (MY_IS_OBJECT (self));
    
    g_signal_emit (self, signals[VALUE_CHANGED], 0, new_value);
}