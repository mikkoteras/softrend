
#include "window.h"

window::window(scene *s, int w, int h, QWidget *parent) :
    QWidget(parent),

    width(w), height(h),
    container(new QLabel),
    render_buffer(new QImage(width, height, QImage::Format_RGB32)),
    pixmap_buffer{new QPixmap(width, height), new QPixmap(width, height)},
    the_scene(s),
    current_buffer_index(0),
    timer(new QTimer(this)),
    closed(false),
    frame(width, height) {

    connect(timer, SIGNAL(timeout()), this, SLOT(timer_event()));
    timer->start(0);
	the_scene->start();
}

window::~window() {
}

void window::timer_event() {
    update();
}

void window::closeEvent(QCloseEvent *) {
    closed = true;
}

void window::update() {
    if (closed)
        return;

    mark.update_starting();

    frame.clear();
    mark.clear_finished();
    
    the_scene->render(frame);
    mark.render_finished();
    
    current_buffer_index = 1 - current_buffer_index;
    frame.render_frame_on(render_buffer);
    pixmap_buffer[current_buffer_index]->convertFromImage(*render_buffer);
    mark.copy_finished();
    
    container->setPixmap(*pixmap_buffer[current_buffer_index]);
    container->show();
}
