#include "mainwindow.h"

#include <QGuiApplication>
#include <QScreen>

#include <QDebug>

#define WIDTH 400
#define HEIGHT 400

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    // Configure OpenGL to use the 3.3 core version
    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3,3);
    // Amount of bits used for the Z-buffer (24 usually)
    format.setDepthBufferSize(24);
    // Set super sampling
    //format.setSamples(16);

    MainWindow window;
    window.setFormat(format);

    // Resize the window and place it on the center of the screen
    QScreen *screen = app.primaryScreen();
    QRect size(screen->geometry());
    window.setGeometry(size.width()/2-(WIDTH/2),size.height()/2-(HEIGHT/2),WIDTH,HEIGHT);

    window.show();

    return app.exec();
}
