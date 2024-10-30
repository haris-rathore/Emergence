#include "renderer.hpp"

Renderer::Renderer(){
    // shader = new Shader();
}

// void Renderer::update(){
//     while(running){
//         if(Queue.size() < 10){
//             nextGeneration(grid);
//             render_data to_push;
//             to_push.vertices = make_vertices(grid);
//             to_push.indices = make_indices(vertices);
//             Queue.push(to_push);
//         }
//     }
// }

void Renderer::realize(GtkWidget *widget){
    running = true;
    xoffset = -1.0f;
    yoffset = -1.0f;
    xratio = 1.0f;
    yratio = 1.0f;
    zoom = 1.0f;
    update_speed = std::chrono::nanoseconds(100'000'000);

    GdkGLContext *context;
    gtk_gl_area_make_current (GTK_GL_AREA (widget));
    if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
        return;
    context = gtk_gl_area_get_context (GTK_GL_AREA (widget));
    int major, minor;
    gtk_gl_area_get_required_version (GTK_GL_AREA(widget), &major, &minor);
    std::cout << "major: " << major << "minor: " << minor << '\n'; 

    srand(time(0));
    for(int y = 0; y < GRID_SIZE; y++){
        for(int x = 0; x < GRID_SIZE; x++){
            if(rand() % 2){
                Updater::grid.insert(Cell(x, y));
            }
        }
    }

    render_data to_push;
    to_push.vertices = Updater::make_vertices(Updater::grid);
    to_push.indices = Updater::make_indices(Updater::vertices);
    Application::Queue.push(to_push);
    Application::update_thread = std::thread(Updater::update);
    Updater::vertices = Application::Queue.front().vertices;
    Updater::indices = Application::Queue.front().indices;
    last_frame = std::chrono::steady_clock::now();
   
    GLuint VBO;
    glGenBuffers(1, &VBO);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Updater::vertices.size() * sizeof(int), Updater::vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Updater::indices.size() * sizeof(int), Updater::indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(GLint), (void*) 0);
    glEnableVertexAttribArray(0);
    shader = new Shader();
    shader->use();
}

void Renderer::unrealize(GtkWidget *widget){
    gtk_gl_area_make_current (GTK_GL_AREA (widget));

    if (gtk_gl_area_get_error (GTK_GL_AREA (widget)) != NULL)
        return;

}

gboolean Renderer::render(GtkGLArea *area, GdkGLContext *context){
    if (gtk_gl_area_get_error (area) != NULL){
        std::cout << "failed\n";
        return FALSE;
    }
    GdkFrameClock *frame_clock = gtk_widget_get_frame_clock(GTK_WIDGET(area));
    g_signal_connect_swapped(frame_clock, "update", G_CALLBACK(gtk_gl_area_queue_render), area);
    gdk_frame_clock_begin_updating(frame_clock);

    // std::cout << "fps: " << gdk_frame_clock_get_fps(frame_clock) << '\n';

    // shader->setFloat("scalar", 2.0f/COLUMNS);
    shader->setFloat("xoffset", xoffset);
    shader->setFloat("yoffset", yoffset);
    shader->setFloat("xratio", xratio);
    shader->setFloat("yratio", yratio);
    shader->setFloat("zoom", zoom);

    auto new_frame = std::chrono::steady_clock::now();

    auto duration = new_frame - last_frame;
    if(duration > update_speed && !Application::Queue.empty()){
        std::cout << "compute rate: " << 1'000'000'000.0 / duration.count() << '\n';
        Updater::vertices = Application::Queue.front().vertices;
        Updater::indices = Application::Queue.front().indices;
        Application::Queue.pop();
        last_frame = new_frame;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glDrawElements(GL_TRIANGLES, Updater::indices.size(), GL_UNSIGNED_INT, 0);
    glBufferData(GL_ARRAY_BUFFER, Updater::vertices.size() * sizeof(int), Updater::vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 2 * sizeof(GLint), (void*) 0);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Updater::indices.size() * sizeof(int), Updater::indices.data(), GL_STATIC_DRAW);

    // std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return TRUE;
}
