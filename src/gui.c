#include "gui.h"

//
// global data
//

static GdkPixbuf    *g_buf;
static GtkListStore *g_list_store;

enum InputMode 
{
	NONE,
	ERASE,
	DRAW
} 
static g_input_mode;

//
// Forward declarations:
// 

gboolean requireUpdate(float *data);

static GtkWidget* makeGUI();

static void allocatePixbuf(guint w, guint h);

static void allocateListStore();

static void populateList(GtkListStore *list_store);

static GdkPixbuf* scalePixbuf(GdkPixbuf *buf);

static void update_image(guint x, guint y, GtkImage *image, guchar value, guint radius);

static gboolean on_mouse_move(GtkWidget* widget, GdkEventMotion *event, gpointer user_data);

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

static gboolean on_button_release(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

//
// Definitions :
//

gboolean requireUpdate(float *data)
{
	GtkTreePath *path = gtk_tree_path_new_from_string("0");
	GtkTreeIter iter;
	
	if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(g_list_store), &iter, path))
	{
		g_log(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "can't access g_list_store");
		return FALSE;
	}
	
	for (int i = 0; i < 10; ++i)
	{
		gint d = (gint) round(100 * data[i]);
		gtk_list_store_set(GTK_LIST_STORE(g_list_store), &iter, 1, d, -1);	
	 	gtk_tree_model_iter_next(GTK_TREE_MODEL(g_list_store), &iter);	
	}
	
	return TRUE;
}



static GtkWidget* makeGUI()
{
    // global g_buf	
	allocatePixbuf(PIX_WIDTH, PIX_WIDTH);
	
	// global g_list_store
	allocateListStore();

	// prepare image
	GdkPixbuf *scaled = scalePixbuf(g_buf);
	GtkWidget *image = gtk_image_new_from_pixbuf(scaled);	
	gtk_widget_set_hexpand((GtkWidget*)image, TRUE);
	gtk_widget_set_vexpand((GtkWidget*)image, TRUE);

	// box +-> wrapper -> ebox -> image
	//     |
	//     +-> tree_view 

	GtkContainer *box = (GtkContainer*) gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkContainer *wrapper = (GtkContainer*) gtk_fixed_new();

	// prepare ebox (add image and event handlers)
	GtkWidget *ebox = gtk_event_box_new(); 
	gtk_container_add((GtkContainer*) ebox, (GtkWidget*) image);

    g_signal_connect(G_OBJECT(ebox), "motion-notify-event", G_CALLBACK(on_mouse_move), NULL);
    g_signal_connect(G_OBJECT(ebox), "button-press-event", G_CALLBACK(on_button_press), NULL);
    g_signal_connect(G_OBJECT(ebox), "button-release-event", G_CALLBACK(on_button_release), NULL);

	gtk_fixed_put((GtkFixed*) wrapper, ebox, 0, 0);
	
	// output section
	GtkTreeView *tree_view = (GtkTreeView*) gtk_tree_view_new_with_model(GTK_TREE_MODEL(g_list_store));
	
	GtkCellRenderer *text_rend = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *text_col = gtk_tree_view_column_new_with_attributes("Digit", 
		(GtkCellRenderer*) text_rend, 
		"text", 0, // wtf ?
		NULL); // null termination
	gtk_tree_view_append_column(tree_view, text_col);

	GtkCellRenderer *prog_rend = gtk_cell_renderer_progress_new();
	GtkTreeViewColumn *prog_col = gtk_tree_view_column_new_with_attributes("Result", 
		prog_rend,
		"value", 1,
		NULL); 

	gtk_tree_view_append_column(tree_view, prog_col);

	gtk_container_add(box, (GtkWidget*) wrapper);
	gtk_container_add(box, (GtkWidget*) tree_view);
	
	static float data[] = {.23f, .45f, .35f, .2f, .11f, .55f, .71f, .46f, .21f, .01f}; 
	g_timeout_add(600, (GSourceFunc)requireUpdate, &data); 

	return (GtkWidget*) box;
}


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

	gint start_x = x / IMG_SCALE - radius;
	gint start_y = y / IMG_SCALE - radius;
	gint end_y = start_y + (2 * radius + 1);
	
	for (gint y = start_y; y < end_y; ++y)
	{
		// applying brush shape
		gint dy = brushes[radius][y - start_y];

		// calculate pixel index
		gint x = start_x + dy;
		gint span = 2*(radius-dy) + 1;
		
		// if x less then zero, write only rest of the pixels
		if (x < 0)
		{
			span += x;
			x = 0;
		}
		
		if (y >= 0 && y < PIX_WIDTH && x < PIX_WIDTH)
		{
			gint idx = PIX_STRIDE * (y * PIX_WIDTH + x);
			
			if (x + span >= PIX_WIDTH)
			{
				span = -x + PIX_WIDTH;
			}

			memset(pixels+idx, value, span*PIX_STRIDE);
		}
	}
	
	GdkPixbuf *old = gtk_image_get_pixbuf(image);
	g_object_unref(G_OBJECT(old));

	gtk_image_set_from_pixbuf(image, scalePixbuf(g_buf));
}

static gboolean on_mouse_move(GtkWidget* widget, GdkEventMotion *event, gpointer user_data) 
{
	GtkBox *box = (GtkBox*) widget; 
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

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	GtkBox *box = (GtkBox*) widget; 
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

static gboolean on_button_release(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
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

static void allocatePixbuf(guint w, guint h)
{
	// allocate global buf
	g_buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, w, h);
	gdk_pixbuf_fill(g_buf, 0x00000000);
	
	// setting initial input mode
	g_input_mode = NONE;
}

static void allocateListStore()
{
	g_list_store = (GtkListStore*)gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	populateList(g_list_store);
}


static void populateList(GtkListStore *list_store)
{
	GtkTreeIter iter;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store), &iter);	
	
	// populating list with values:
	static const char* names[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
	
	for (int i = 0; i < 10; i++) 
	{
		gtk_list_store_append((GtkListStore*) list_store, &iter);
		gtk_list_store_set((GtkListStore*) list_store, 
			&iter, 
			0, names[i], 
			1, i*10, 
			-1);
	 }		
}


static void activate(GtkApplication* app, gpointer user_data)
{
	// create window
	GtkWidget *window;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Neuron Engine");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	
	// setup gui
	gtk_container_add((GtkContainer*) window, makeGUI());
	
	gtk_widget_show_all(window);
}


int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("obps.neuron.engine", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
   
	// clean up
	g_object_unref(app);
	g_object_unref(G_OBJECT(g_buf));
	g_object_unref(G_OBJECT(g_list_store));

    return status;
}
