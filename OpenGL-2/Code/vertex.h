#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include <QVector2D>

/**
 * Class used by OBJMOdel to filter unique vertices
 * @brief The Vertex class
 */
class Vertex {
public:
    Vertex() : c(), n(), t(){}
    Vertex(QVector3D coords, QVector3D normal, QVector3D texc): c(coords), n(normal), t(texc){}
    QVector3D c,n;
    QVector2D t;
    bool operator==(const Vertex &other) const {
        if (other.c != c)
                return false;
        if (other.n != n)
            return false;
        if (other.t != t)
            return false;
        return true;
    }
};

#endif // VERTEX_H

