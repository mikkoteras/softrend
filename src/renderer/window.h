#ifndef WINDOW_H
#define WINDOW_H

#include "framebuffer.h"
#include "benchmark.h"
#include "scene.h"
#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QTimer>

class window : public QWidget {
    Q_OBJECT

public:
    window(scene *s, int w, int h, QWidget *parent = nullptr);
    virtual ~window();

public slots:
    void timer_event();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void update();

private:
    int width, height;
    QLabel *container;
    QImage *render_buffer;
    QPixmap *pixmap_buffer[2];
    scene *the_scene;
    int current_buffer_index;
    QTimer *timer;
    bool closed;
    framebuffer frame;
    benchmark mark;
    
};

#endif
