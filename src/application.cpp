#include "application.hpp"

Application::Application(int _argc, char** _argv): argc(_argc), argv(_argv) {
    if(argc == 2){
        // std::cout << argv[0] << '\n' << argv[1] << '\n' << argv[2] << '\n';
        // std::stoi(argv[1]);
        // Updater::ROWS = std::stoi(argv[1]);
        // Updater::COLUMNS = std::stoi(argv[2]);
        Renderer::GRID_SIZE = std::stoi(argv[1]);
        argc = 1;
    }
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
}

int Application::run(){
    return g_application_run (G_APPLICATION (app), argc, argv);
}

void Application::activate(GtkApplication* app, gpointer user_data){
    GtkWidget *window, *box, *button;

    window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 800);
    gtk_window_set_title(GTK_WINDOW(window), "learngl");

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, FALSE);
    gtk_widget_set_margin_start (box, 12);
    gtk_widget_set_margin_end (box, 12);
    gtk_widget_set_margin_top (box, 12);
    gtk_widget_set_margin_bottom (box, 12);
    gtk_box_set_spacing (GTK_BOX (box), 6);
    gtk_window_set_child (GTK_WINDOW (window), box);


    gl_area = gtk_gl_area_new();
    gtk_widget_set_hexpand(gl_area, TRUE);
    gtk_widget_set_vexpand(gl_area, TRUE);
    gtk_widget_set_valign(gl_area, GTK_ALIGN_FILL);
    gtk_widget_set_halign(gl_area, GTK_ALIGN_FILL);
    gtk_widget_set_size_request(gl_area, 800, 800);
    gtk_box_append(GTK_BOX(box), gl_area);
    

    g_signal_connect(gl_area, "realize", G_CALLBACK(Renderer::realize), NULL);
    g_signal_connect(gl_area, "unrealize", G_CALLBACK(Renderer::unrealize), NULL);
    g_signal_connect(gl_area, "render", G_CALLBACK(Renderer::render), NULL);
    g_signal_connect(gl_area, "resize", G_CALLBACK(resize), NULL);

    GtkEventController *event_controller;
    event_controller = gtk_event_controller_key_new();

    g_signal_connect_object (event_controller, "key-pressed", G_CALLBACK(key_pressed), window, G_CONNECT_SWAPPED);
    gtk_widget_add_controller (GTK_WIDGET (window), event_controller);

    GtkEventController * scroll_controller;
    scroll_controller = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES);

    g_signal_connect_object(scroll_controller, "scroll", G_CALLBACK(scroll), window, G_CONNECT_SWAPPED);
    gtk_widget_add_controller(window, scroll_controller);

    GtkGesture* zoom_controller = gtk_gesture_zoom_new();
    g_signal_connect_object(zoom_controller, "scale-changed", G_CALLBACK(zoom_handler), window, G_CONNECT_SWAPPED);
    gtk_widget_add_controller(window, GTK_EVENT_CONTROLLER(zoom_controller));

    gtk_window_present(GTK_WINDOW(window));
}

void Application::resize(GtkGLArea* self, gint width, gint height, gpointer user_data){
    // std::cout << "width: " << width << "\nheight: " << height << "\n";
    Renderer::xratio = static_cast<float>(Updater::COLUMNS) / (4 * width);
    Renderer::yratio = static_cast<float>(Updater::ROWS) / (4 * height);
}

void Application::zoom_handler(GtkGestureZoom* self, gdouble scale, gpointer user_data){
    // std::cout << "scale: " << scale << "\n";
    Renderer::zoom += (scale - 1) * 0.05;
    if(Renderer::zoom < 0.01){
        Renderer::zoom = 0.01;
    }
    else if(Renderer::zoom > 2.5){
        Renderer::zoom = 2.5;
    }
    // std::cout << "zoom: " << zoom << "\n";
}

gboolean Application::key_pressed(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data){
    if(keyval == GDK_KEY_Left){
        Renderer::xoffset += 0.2;
    }
    else if(keyval == GDK_KEY_Right){
        Renderer::xoffset -= 0.2;
    }
    else if(keyval == GDK_KEY_Up){
        Renderer::yoffset -= 0.2;
    }
    else if(keyval == GDK_KEY_Down){
        Renderer::yoffset += 0.2;
    }
    gtk_gl_area_queue_render(GTK_GL_AREA(gl_area));
    return TRUE;
}

gboolean Application::scroll(GtkEventControllerScroll* self, gdouble dx, gdouble dy, gpointer user_data){
    Renderer::xoffset -= dx;
    Renderer::yoffset += dy;
    gtk_gl_area_queue_render(GTK_GL_AREA(gl_area));
    return TRUE;
}

Application::~Application(){
    Renderer::running = false;
    update_thread.join();
    g_object_unref(app);
}