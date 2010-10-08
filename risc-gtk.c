#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <complex.h>
#include <sys/time.h>
#include <math.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkgl.h>

#include "game.h"
#include "physics.h"
#include "ship.h"
#include "bullet.h"
#include "scenario.h"
#include "team.h"
#include "particle.h"
#include "glutil.h"
#include "renderer.h"

#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#ifdef GDK_WINDOWING_QUARTZ
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

static GTimer *timer = NULL;
static gint frames = 0;
static GdkWindow *main_window;

static gboolean is_sync = TRUE;

static const int FPS = 32;
static const double tick_length = 1.0/32.0;
const double zoom_force = 0.1;

int screen_width = 640;
int screen_height = 480;
complex double view_pos = 0.0;
double view_scale = 16.0;
int paused = 0;
int single_step = 0;
int render_all_debug_lines = 0;
struct ship *picked = NULL;
int simple_graphics = 0;

static complex double W(complex double o)
{
	return view_pos + (o - (screen_width/2) - (I * screen_height/2))/view_scale;
}

static struct ship *pick(vec2 p)
{
	GList *es;
	for (es = g_list_first(all_ships); es; es = g_list_next(es)) {
		struct ship *s = es->data;
		if (distance(s->physics->p, p) < s->physics->r) {
			return s;
		}
	}
	return NULL;
}

static void zoom(double f)
{
	int x, y;
	gdk_window_get_pointer(main_window, &x, &y, NULL);
	view_pos = (1-zoom_force)*view_pos + zoom_force * W(C(x,y));
	view_scale *= f;
}

static gboolean
draw (GtkWidget      *widget,
      GdkEventExpose *event,
      gpointer        data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

  /*** OpenGL BEGIN ***/
  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return FALSE;

	render_gl13();

	gdk_gl_drawable_swap_buffers (gldrawable);

  gdk_gl_drawable_gl_end (gldrawable);
  /*** OpenGL END ***/

  frames++;

  {
    gdouble seconds = g_timer_elapsed (timer, NULL);
    if (seconds >= 5.0) {
      gdouble fps = frames / seconds;
      g_print ("%d frames in %6.3f seconds = %6.3f FPS\n", frames, seconds, fps);
      g_timer_reset (timer);
      frames = 0;
    }
  }

  return TRUE;
}

/* new window size or exposure */
static gboolean
reshape (GtkWidget         *widget,
	 GdkEventConfigure *event,
	 gpointer           data)
{
  GtkAllocation allocation;
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

  gtk_widget_get_allocation (widget, &allocation);

  /*** OpenGL BEGIN ***/
  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return FALSE;

	screen_width = allocation.width;
	screen_height = allocation.height;
  glViewport (0, 0, allocation.width, allocation.height);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
	glOrtho(0.0f, allocation.width, allocation.height, 0.0f, -1.0f, 1.0f);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  gdk_gl_drawable_gl_end (gldrawable);
  /*** OpenGL END ***/

  return TRUE;
}

static void
init(GtkWidget *widget,
     gpointer   data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

  /*** OpenGL BEGIN ***/
  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return;

	glEnable( GL_TEXTURE_2D );
	glClearColor( 0.0f, 0.0f, 0.03f, 0.0f );
	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glLineWidth(1.2);

  gdk_gl_drawable_gl_end (gldrawable);
  /*** OpenGL END ***/

  /* create timer */
  if (timer == NULL)
    timer = g_timer_new ();

  g_timer_start (timer);
}

static gboolean
idle (GtkWidget *widget)
{
  GtkAllocation allocation;
  GdkWindow *window;

  window = gtk_widget_get_window (widget);
  gtk_widget_get_allocation (widget, &allocation);

  /* Invalidate the whole window. */
  gdk_window_invalidate_rect (window, &allocation, FALSE);

  /* Update synchronously (fast). */
  if (is_sync)
    gdk_window_process_updates (window, FALSE);

	if (!paused) {
		game_purge();
		game_tick(tick_length);
		if (!simple_graphics) {
			particle_tick();
		}

		struct team *winner;
		if ((winner = game_check_victory())) {
			printf("Team '%s' is victorious in %0.2f seconds\n", winner->name, ticks*tick_length);
			paused = 1;
		}

		ticks += 1;
	}

	if (single_step) {
		paused = 1;
		single_step = 0;
	}

	usleep(16666);

  return TRUE;
}

static guint idle_id = 0;

static void
idle_add (GtkWidget *widget)
{
  if (idle_id == 0)
    {
      idle_id = g_idle_add_full (GDK_PRIORITY_REDRAW,
                                 (GSourceFunc) idle,
                                 widget,
                                 NULL);
    }
}

static void
idle_remove (GtkWidget *widget)
{
  if (idle_id != 0)
    {
      g_source_remove (idle_id);
      idle_id = 0;
    }
}

static gboolean
map (GtkWidget   *widget,
     GdkEventAny *event,
     gpointer     data)
{
  idle_add (widget);

  return TRUE;
}

static gboolean
unmap (GtkWidget   *widget,
       GdkEventAny *event,
       gpointer     data)
{
  idle_remove (widget);

  return TRUE;
}

static gboolean
visible (GtkWidget          *widget,
	 GdkEventVisibility *event,
	 gpointer            data)
{
  if (event->state == GDK_VISIBILITY_FULLY_OBSCURED)
    idle_remove (widget);
  else
    idle_add (widget);

  return TRUE;
}

static gboolean
key (GtkWidget   *widget,
     GdkEventKey *event,
     gpointer     data)
{
  GtkAllocation allocation;

  switch (event->keyval) {
		case GDK_z:
			zoom(1.1);
			break;
		case GDK_x:
			zoom(1.0/1.1);
			break;
		case GDK_space:
			paused = !paused;
			break;
		case GDK_Return:
			paused = 0;
			single_step = 1;
			break;
		case GDK_y:
			render_all_debug_lines = !render_all_debug_lines;
			break;
		case GDK_p:
			screenshot("screenshot.tga");
			break;
    case GDK_Escape:
      gtk_main_quit ();
      break;
    default:
      return FALSE;
    }

  gtk_widget_get_allocation (widget, &allocation);
  gdk_window_invalidate_rect (gtk_widget_get_window (widget), &allocation, FALSE);

  return TRUE;
}

static gboolean
button_press_event( GtkWidget *widget, GdkEventButton *event )
{
	printf("button %d\n", event->button);
	int x,y;

	switch (event->button) {
		case 1:
			gdk_window_get_pointer(main_window, &x, &y, NULL);
			picked = pick(W(C(x, y)));
			break;
		case 4:
			zoom(1.1);
			break;
		case 5:
			zoom(1.0/1.1);
			break;
		default:
			break;
	}

  return TRUE;
}

int
main (int   argc,
      char *argv[])
{
  GdkGLConfig *glconfig;
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *drawing_area;
  GtkWidget *button;
  int i;

	setenv("__GL_FSAA_MODE", "12", 0);

  /*
   * Init GTK.
   */

  gtk_init (&argc, &argv);

  /*
   * Init GtkGLExt.
   */

  gtk_gl_init (&argc, &argv);

  /*
   * Command line options.
   */

  for (i = 0; i < argc; i++)
    {
      if (strcmp (argv[i], "--async") == 0)
        is_sync = FALSE;
    }

  /*
   * Configure OpenGL-capable visual.
   */

  /* Try double-buffered visual */
  glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB    |
					GDK_GL_MODE_DEPTH  |
					GDK_GL_MODE_DOUBLE);
  if (glconfig == NULL)
    {
      g_print ("*** Cannot find the double-buffered visual.\n");
      g_print ("*** Trying single-buffered visual.\n");

      /* Try single-buffered visual */
      glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB   |
					    GDK_GL_MODE_DEPTH);
      if (glconfig == NULL)
	{
	  g_print ("*** No appropriate OpenGL-capable visual found.\n");
	  exit (1);
	}
    }

  /*
   * Top-level window.
   */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	main_window = gtk_widget_get_window (window);
  gtk_window_set_title (GTK_WINDOW (window), "RISC");

  /* Get automatically redrawn if any of their children changed allocation. */
  gtk_container_set_reallocate_redraws (GTK_CONTAINER (window), TRUE);

  g_signal_connect (G_OBJECT (window), "delete_event",
		    G_CALLBACK (gtk_main_quit), NULL);

  /*
   * VBox.
   */

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (vbox);

  /*
   * Drawing area for drawing OpenGL scene.
   */

  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, 300, 300);

  /* Set OpenGL-capability to the widget. */
  gtk_widget_set_gl_capability (drawing_area,
				glconfig,
				NULL,
				TRUE,
				GDK_GL_RGBA_TYPE);

  gtk_widget_add_events (drawing_area, GDK_VISIBILITY_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK);

  g_signal_connect_after (G_OBJECT (drawing_area), "realize",
                          G_CALLBACK (init), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "configure_event",
		    G_CALLBACK (reshape), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "expose_event",
		    G_CALLBACK (draw), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "map_event",
		    G_CALLBACK (map), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "unmap_event",
		    G_CALLBACK (unmap), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "visibility_notify_event",
		    G_CALLBACK (visible), NULL);

  g_signal_connect_swapped(G_OBJECT (window), "key_press_event", G_CALLBACK (key), drawing_area);
	g_signal_connect_swapped(G_OBJECT (drawing_area), "button_press_event", G_CALLBACK(button_press_event), NULL);

  gtk_box_pack_start (GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);

  gtk_widget_show (drawing_area);

  /*
   * Simple quit button.
   */

  button = gtk_button_new_with_label ("Quit");

  g_signal_connect (G_OBJECT (button), "clicked",
		    G_CALLBACK (gtk_main_quit), NULL);

  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  gtk_widget_show (button);

  /*
   * Show window.
   */

  gtk_widget_show (window);

	int seed = getpid() ^ time(NULL);

	int num_teams;
	char *scenario;
	char **teams;

	if (argc == 1) {
		scenario = NULL;
		num_teams = 0;
		teams = NULL;
	} else {
		scenario = argv[1];
		num_teams = argc - 2;
		teams = argv+2;
	}

	if (game_init(seed, scenario, num_teams, teams)) {
		fprintf(stderr, "initialization failed\n");
		return 1;
	}


  /*
   * Main loop.
   */
//idle_add (drawing_area);
  gtk_main ();

  return 0;
}
