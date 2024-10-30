#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <gtk/gtk.h>
#include <string>
#include "renderer.hpp"
#include "updater.hpp"

struct render_data{
    std::vector<int> vertices;
    std::vector<unsigned int> indices;
};

class Application{
    public:
    GtkApplication* app;
    inline static GtkWidget* gl_area;

    // inline static Shader* shader;
    // inline static float xoffset;
    // inline static float yoffset;
    // inline static float xratio;
    // inline static float yratio;
    // inline static float zoom; 
    inline static std::queue<render_data> Queue;
    inline static std::thread update_thread;
    // inline static std::chrono::nanoseconds update_speed; // half a second
    // inline static int COLUMNS;
    // inline static int ROWS;

    int argc;
    char** argv;
    Application(int argc, char** argv);
    int run();
    static void activate(GtkApplication* app, gpointer user_data);
    static void resize(GtkGLArea* self, gint width, gint height, gpointer user_data);
    static void zoom_handler(GtkGestureZoom* self, gdouble scale, gpointer user_data);
    static gboolean key_pressed(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data);
    static gboolean scroll(GtkEventControllerScroll* self, gdouble dx, gdouble dy, gpointer user_data);
    ~Application();
};

#endif