#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

/**
 * @brief The OBJLoader class
 *
 * Loads all data from a Wavefront .obj file
 * Currently only supports traingle meshes with
 * one normal/texture coordinate per vertex
 *
 */
class OBJModel
{
public:

    OBJModel(QString filename);

    // Used for glDrawArrays()
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<QVector2D> texcoords;

    // Used for glDrawElements()
    QVector<QVector3D> vertices_indexed;
    QVector<QVector3D> normals_indexed;
    QVector<QVector2D> texcoords_indexed;
    QVector<unsigned> indices;

private:

    void parseVertex(QStringList tokens);
    void parseNormal(QStringList tokens);
    void parseTexture(QStringList tokens);
    void parseFace(QStringList tokens);
    void alignData();
    void unpackIndices();
    void unitize();

    QVector<QVector3D> norm;
    QVector<QVector2D> tex;
    QVector<unsigned> normal_indices;
    QVector<unsigned> texcoord_indices;

};

#endif // OBJLOADER_H
