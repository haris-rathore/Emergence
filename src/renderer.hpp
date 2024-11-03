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
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "shader.hpp"
#include "application.hpp"
#include "updater.hpp"

class Renderer{
    public:
    inline static bool running;
    inline static Shader* shader;
    inline static float zoom = 1.0f;
    inline static int GRID_SIZE = 80;
    inline static glm::mat4 projection = glm::ortho(-400.0f / Renderer::zoom, 400.0f / Renderer::zoom, -400.0f / Renderer::zoom, 400.0f / Renderer::zoom, -1.0f, 10.0f);
    inline static std::chrono::nanoseconds update_speed = std::chrono::nanoseconds(100'000'000);
    inline static std::chrono::steady_clock::time_point last_frame;
    inline static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    inline static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    inline static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    static gboolean render(GtkGLArea *area, GdkGLContext *context);
    static void realize(GtkWidget *widget);
    static void unrealize(GtkWidget *widget);
};

#endif