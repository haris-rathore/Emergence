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

// static gboolean key_handler(GtkWidget* widget, GdkEvent* event, gpointer user_data){
//     switch(gdk_key_event_get_keyval(event)){
//         case GDK_KEY_A: case GDK_KEY_a: xoffset -= 0.5; break;
//         case GDK_KEY_F: case GDK_KEY_f: xoffset += 0.5; break;
//         case GDK_KEY_S: case GDK_KEY_s: yoffset += 0.5; break;
//         case GDK_KEY_D: case GDK_KEY_d: yoffset -= 0.5; break;
//         default: break;
//     }
//     return TRUE;
// }

// gboolean key_pressed (GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data){
//     std::cout << gtk_event_controller_key_get_group(self);
//     std::cout << "\nkey_pressed was called\n";
//     return TRUE;
// }

void inc_x(GtkButton* self, gpointer user_data){
    xoffset += 0.5;
}

void dec_x(GtkButton* self, gpointer user_data){
    xoffset -= 0.5;
}

void inc_y(GtkButton* self, gpointer user_data){
    yoffset += 0.5;
}

void dec_y(GtkButton* self, gpointer user_data){
    yoffset -= 0.5;
}

static gboolean render(GtkGLArea *area, GdkGLContext *context){
    if (gtk_gl_area_get_error (area) != NULL){
        std::cout << "failed\n";
        return FALSE;
    }
    GdkFrameClock *frame_clock = gtk_widget_get_frame_clock(GTK_WIDGET(area));
    g_signal_connect_swapped(frame_clock, "update", G_CALLBACK(gtk_gl_area_queue_render), gl_area);
    gdk_frame_clock_begin_updating(frame_clock);

    shader->setFloat("scalar", 2.0f/COLUMNS);
    shader->setFloat("xoffset", xoffset);
    shader->setFloat("yoffset", yoffset);

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

void activate(GtkApplication* app, gpointer user_data){
    GtkWidget *window, *box, *button;

    window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 800);
    gtk_window_set_title(GTK_WINDOW(window), "learngl");
    // g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, FALSE);
    // gtk_widget_set_margin_start (box, 12);
    // gtk_widget_set_margin_end (box, 12);
    // gtk_widget_set_margin_top (box, 12);
    // gtk_widget_set_margin_bottom (box, 12);
    // gtk_box_set_spacing (GTK_BOX (box), 6);
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

    button = gtk_button_new_with_label("+x");
    gtk_box_append(GTK_BOX(box), button);
    g_signal_connect(button, "clicked", G_CALLBACK(inc_x), NULL);

    button = gtk_button_new_with_label("-x");
    gtk_box_append(GTK_BOX(box), button);
    g_signal_connect(button, "clicked", G_CALLBACK(dec_x), NULL);

    button = gtk_button_new_with_label("+y");
    gtk_box_append(GTK_BOX(box), button);
    g_signal_connect(button, "clicked", G_CALLBACK(inc_y), NULL);

    button = gtk_button_new_with_label("-y");
    gtk_box_append(GTK_BOX(box), button);
    g_signal_connect(button, "clicked", G_CALLBACK(dec_y), NULL);

    // GtkEventController* event_controller = gtk_event_controller_key_new();
    // gtk_event_controller_key_get_type()

    // g_signal_connect(event_controller, "key-press-event", G_CALLBACK(key_pressed), NULL);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char** argv){
    GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}