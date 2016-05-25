#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QWindow>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>


/* A backport from the modern Qt 5.5 OpenGLWindow class
 * Inherit from this class to implement your application
 * You do NOT need to change any code from this class in the first weeeks
 */
class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWindow(QWindow *parent = 0);
    ~OpenGLWindow();

    virtual void render();
    virtual void initialize();
    QOpenGLFunctions_3_3_Core *m_funcs;

public slots:
    /**
     * @brief renderLater schedules a repaint in Qt
     */
    void renderLater();
    /**
     * @brief renderNow, repaints now
     */
    void renderNow();

protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void exposeEvent(QExposeEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_update_pending;
    QOpenGLContext *m_context;
};

#endif // OPENGLWINDOW_H
