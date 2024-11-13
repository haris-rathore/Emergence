#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <gtk/gtk.h>
#include <string>
#include <mutex>
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
    inline static int _width = 800;
    inline static int _height = 800;
    inline static std::queue<render_data> Queue;
    inline static std::thread update_thread;
    inline static std::mutex q_tex;

    int argc;
    char** argv;
    Application(int argc, char** argv);
    int run();
    static void activate(GtkApplication* app, gpointer user_data);
    static void resize(GtkGLArea* self, gint width, gint height, gpointer user_data);
    static void zoom_handler(GtkGestureZoom* self, gdouble scale, gpointer user_data);
    static gboolean scroll(GtkEventControllerScroll* self, gdouble dx, gdouble dy, gpointer user_data);
    ~Application();
};

#endif