#include <gtk/gtk.h>
#include <gdk-pixbuf-2.0/gdk-pixbuf/gdk-pixbuf.h>

#define IMG_SCALE 10
#define PIX_WIDTH 32

GdkPixbuf *g_buf;

enum InputMode {
	NONE,
	ERASE,
	DRAW
};

enum InputMode g_input_mode;


static GdkPixbuf* scalePixbuf(GdkPixbuf *buf)
{
	guint w, h;

	w = gdk_pixbuf_get_width(buf);
	h = gdk_pixbuf_get_height(buf);

	return gdk_pixbuf_scale_simple(buf, w*IMG_SCALE, h*IMG_SCALE, GDK_INTERP_HYPER);
}

static void update_image(guint x, guint y, GtkImage *image, guchar value, guint radius)
{
	static guint brushes[4][7] = {
		{0},
		{1, 0, 1},
		{1, 0, 0, 0, 1},
		{3, 1, 1, 0, 1, 1, 3}
	};

	guchar *pixels = gdk_pixbuf_get_pixels(g_buf);

	guint pos_x = x / IMG_SCALE;
	guint pos_y = y / IMG_SCALE;

	guint start_x = pos_x >= radius ? pos_x - radius : pos_x;
	guint end_x = pos_x + radius < PIX_WIDTH ? pos_x + radius + 1 : PIX_WIDTH;

	guint start_y = pos_y >= radius ? pos_y - radius : pos_y;
	guint end_y = pos_y + radius < PIX_WIDTH ? pos_y + radius + 1 : PIX_WIDTH;
	
	for (guint y = start_y; y < end_y; ++y)
	{
		// applying brush shape
		guint dy = brushes[radius][y - start_y];

		for (guint x = start_x + dy; x < end_x - dy; ++x)
		{
			// calculate pixel index
			guint idx = 3 * (y * PIX_WIDTH + x);
			memset(pixels+idx, value, 3);
		}
	}
	
	GdkPixbuf *old = gtk_image_get_pixbuf(image);
	g_object_unref(G_OBJECT(old));

	gtk_image_set_from_pixbuf(image, scalePixbuf(g_buf));
}

gboolean on_mouse_move(GtkWidget* widget, GdkEventMotion *event, gpointer user_data) 
{
	GtkBox *box = (GtkBox*) gtk_container_get_children((GtkContainer*) widget)->data;
	GtkImage *image = (GtkImage*) gtk_container_get_children((GtkContainer*) box)->data;
	
	if (g_input_mode == DRAW)
	{
		update_image(event->x, event->y, image, 255, 1);
	}
	else if (g_input_mode == ERASE)
	{
		update_image(event->x, event->y, image, 0, 2);
	}

    return TRUE;
}

gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	GtkBox *box = (GtkBox*) gtk_container_get_children((GtkContainer*) widget)->data;
	GtkImage *image = (GtkImage*) gtk_container_get_children((GtkContainer*) box)->data;
	
	if (event->type == GDK_BUTTON_PRESS)
	{
		if (event->button == GDK_BUTTON_PRIMARY)
		{
			g_input_mode = DRAW;
			update_image(event->x, event->y, image, 255, 1);
			return TRUE;
		}

		if (event->button == GDK_BUTTON_SECONDARY)
		{
			g_input_mode = ERASE;
			update_image(event->x, event->y, image, 0, 2);
			return TRUE;
		}
	}
	return TRUE;
}


gboolean on_button_release(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	if (event->type == GDK_BUTTON_RELEASE)
	{
		if (event->button == GDK_BUTTON_PRIMARY   && g_input_mode == DRAW
		 || event->button == GDK_BUTTON_SECONDARY && g_input_mode == ERASE)
		{
			g_input_mode = NONE;
		}
	}
	return TRUE;
}


static GtkWidget* makeImage(guint w, guint h)
{
	// allocate global buf
	g_buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, w, h);
	gdk_pixbuf_fill(g_buf, 0x00000000);

	// setting initial input mode
	g_input_mode = NONE;

	GdkPixbuf *scaled = scalePixbuf(g_buf);
	GtkImage *image = (GtkImage*) gtk_image_new_from_pixbuf(scaled);	
	
	gtk_widget_set_hexpand((GtkWidget*)image, TRUE);
	gtk_widget_set_vexpand((GtkWidget*)image, TRUE);

	// wrap grid inside container
	GtkContainer *wrapper = (GtkContainer*) gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_box_pack_start((GtkBox*)wrapper, (GtkWidget*)image, TRUE, TRUE, 0);

	return (GtkWidget*) wrapper;
}


static void activate(GtkApplication* app, gpointer user_data)
{
	// create window
	GtkWidget *window;
	
	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Window");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	

    g_signal_connect(G_OBJECT(window), "motion-notify-event", G_CALLBACK(on_mouse_move), NULL);
    g_signal_connect(G_OBJECT(window), "button-press-event", G_CALLBACK(on_button_press), NULL);
    g_signal_connect(G_OBJECT(window), "button-release-event", G_CALLBACK(on_button_release), NULL);
	gtk_widget_add_events(window, GDK_POINTER_MOTION_MASK);
	
	// attach grid
	gtk_container_add((GtkContainer*) window, makeImage(PIX_WIDTH, PIX_WIDTH));
	
	gtk_widget_show_all(window);
}


int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
	g_object_unref(G_OBJECT(g_buf));

    return status;
}
