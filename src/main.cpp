#include <iostream>
#include <chrono>
#include <thread>
#include <set>
#include <vector>
#include <array>
#include <gtk/gtk.h>
#include <epoxy/gl.h>
#include "shader.hpp"

static GtkWidget* gl_area = NULL;
Shader* shader;
float xoffset = -1;
float yoffset = -1;
float xratio = 1;
float yratio = 1;
float zoom = 1;

struct Cell{
    int x;
    int y;

    Cell(int x, int y){
        this->x = x;
        this->y = y;
    }
};

bool operator<(const Cell& cell1, const Cell& cell2){
        if(cell1.y < cell2.y || (cell1.y == cell2.y && cell1.x < cell2.x)){
            return true;
        }
        else{
            return false;
        }
    }

const int COLUMNS = 80;
const int ROWS = 80;

std::set<Cell> grid;
std::vector<int> vertices;
std::vector<unsigned int> indices;

void insertCell(const Cell& cell, const std::set<Cell>& grid, std::set<Cell>& next_grid){
    int neighbors = 0;
    if(grid.find(Cell(cell.x - 1, cell.y + 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x, cell.y + 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x + 1, cell.y + 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x - 1, cell.y)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x + 1, cell.y)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x - 1, cell.y - 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x, cell.y - 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(Cell(cell.x + 1, cell.y - 1)) != grid.end()){
        neighbors++;
    }
    if(grid.find(cell) != grid.end()){
        if(neighbors == 2 || neighbors == 3){
            next_grid.insert(cell);
        }
    }
    else{
        if(neighbors == 3){
            next_grid.insert(cell);
        }
    }
}

void nextGeneration(std::set<Cell>& grid){
    std::set<Cell> next_grid;
    for(const Cell& cell : grid){
        insertCell(cell, grid, next_grid);
        insertCell(Cell(cell.x - 1, cell.y + 1), grid, next_grid);
        insertCell(Cell(cell.x, cell.y + 1), grid, next_grid);
        insertCell(Cell(cell.x + 1, cell.y + 1), grid, next_grid);
        insertCell(Cell(cell.x - 1, cell.y), grid, next_grid);
        insertCell(Cell(cell.x + 1, cell.y), grid, next_grid);
        insertCell(Cell(cell.x - 1, cell.y - 1), grid, next_grid);
        insertCell(Cell(cell.x, cell.y - 1), grid, next_grid);
        insertCell(Cell(cell.x + 1, cell.y - 1), grid, next_grid);
    }
    grid = next_grid;
}

std::vector<int> make_vertices(std::set<Cell>& grid){
    std::vector<int> vertices;
    for(const Cell& cell : grid){
        vertices.push_back(cell.x);
        vertices.push_back(cell.y);
        vertices.push_back(cell.x + 1);
        vertices.push_back(cell.y);
        vertices.push_back(cell.x);
        vertices.push_back(cell.y + 1);
        vertices.push_back(cell.x + 1);
        vertices.push_back(cell.y + 1);
    }
    return vertices;
}

std::vector<unsigned int> make_indices(std::vector<int>& vertices){
    std::vector<unsigned int> indices;
    for(int i = 0; i < vertices.size(); i+=4){
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
    }
    return indices;
}

void realize (GtkWidget *widget){
    GdkGLContext *context;
    gtk_gl_area_make_current (GTK_GL_AREA (widget));
    if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
        return;
    context = gtk_gl_area_get_context (GTK_GL_AREA (widget));
    int major, minor;
    gtk_gl_area_get_required_version (GTK_GL_AREA(widget), &major, &minor);
    std::cout << "major: " << major << "minor: " << minor << '\n'; 

    srand(time(0));
    const int GRID_SIZE = 80;
    for(int y = 0; y < GRID_SIZE; y++){
        for(int x = 0; x < GRID_SIZE; x++){
            if(rand() % 2){
                grid.insert(Cell(x, y));
            }
        }
    }


    

    vertices = make_vertices(grid);
    indices = make_indices(vertices);
    GLuint VBO;
    glGenBuffers(1, &VBO);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(int), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(GLint), (void*) 0);
    glEnableVertexAttribArray(0);
    shader = new Shader();
    shader->use();
}

static void unrealize (GtkWidget *widget){
    gtk_gl_area_make_current (GTK_GL_AREA (widget));

    if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
        return;

}


gboolean key_pressed(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data){
    if(keyval == GDK_KEY_Left){
        xoffset += 0.2;
    }
    else if(keyval == GDK_KEY_Right){
        xoffset -= 0.2;
    }
    else if(keyval == GDK_KEY_Up){
        yoffset -= 0.2;
    }
    else if(keyval == GDK_KEY_Down){
        yoffset += 0.2;
    }

    gtk_gl_area_queue_render(GTK_GL_AREA(gl_area));
    return TRUE;
}

gboolean scroll(GtkEventControllerScroll* self, gdouble dx, gdouble dy, gpointer user_data){
    xoffset -= dx;
    yoffset += dy;
    gtk_gl_area_queue_render(GTK_GL_AREA(gl_area));
    return TRUE;
}



static gboolean render(GtkGLArea *area, GdkGLContext *context){
    if (gtk_gl_area_get_error (area) != NULL){
        std::cout << "failed\n";
        return FALSE;
    }
    GdkFrameClock *frame_clock = gtk_widget_get_frame_clock(GTK_WIDGET(area));
    g_signal_connect_swapped(frame_clock, "update", G_CALLBACK(gtk_gl_area_queue_render), gl_area);
    gdk_frame_clock_begin_updating(frame_clock);

    // shader->setFloat("scalar", 2.0f/COLUMNS);
    shader->setFloat("xoffset", xoffset);
    shader->setFloat("yoffset", yoffset);
    shader->setFloat("xratio", xratio);
    shader->setFloat("yratio", yratio);
    shader->setFloat("zoom", zoom);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    nextGeneration(grid);
    vertices = make_vertices(grid);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(int), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(GLint), (void*) 0);
    glEnableVertexAttribArray(0);
    indices = make_indices(vertices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    // std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return TRUE;
}

void resize(GtkGLArea* self, gint width, gint height, gpointer user_data){
    // std::cout << "width: " << width << "\nheight: " << height << "\n";
    xratio = static_cast<float>(COLUMNS) / (4 * width);
    yratio = static_cast<float>(ROWS) / (4 * height);
}

void zoom_handler(GtkGestureZoom* self, gdouble scale, gpointer user_data){
    // std::cout << "scale: " << scale << "\n";
    zoom += (scale - 1) * 0.1;
}

void activate(GtkApplication* app, gpointer user_data){
    GtkWidget *window, *box, *button;

    window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 800);
    gtk_window_set_title(GTK_WINDOW(window), "learngl");
    // g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);

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
    

    g_signal_connect(gl_area, "realize", G_CALLBACK(realize), NULL);
    g_signal_connect(gl_area, "unrealize", G_CALLBACK(unrealize), NULL);
    g_signal_connect(gl_area, "render", G_CALLBACK(render), NULL);
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

int main(int argc, char** argv){
    GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}