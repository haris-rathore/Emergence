#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <gtk/gtk.h>
#include <epoxy/gl.h>
#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <thread>
#include <chrono>
#include "shader.hpp"
#include "application.hpp"
#include "updater.hpp"

class Renderer{
    public:
    inline static bool running;
    inline static Shader* shader;
    inline static float xoffset;
    inline static float yoffset;
    inline static float xratio;
    inline static float yratio;
    inline static float zoom;
    inline static int GRID_SIZE = 80;
    inline static std::chrono::nanoseconds update_speed; // half a second
    inline static std::chrono::steady_clock::time_point last_frame;

    Renderer();
    static gboolean render(GtkGLArea *area, GdkGLContext *context);
    static void realize(GtkWidget *widget);
    static void unrealize(GtkWidget *widget);
};

#endif