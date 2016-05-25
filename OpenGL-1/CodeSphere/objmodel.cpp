#include "objmodel.h"

#include <QFile>
#include <QTextStream>

#include "vertex.h"

#include <QDebug>

OBJModel::OBJModel(QString filename)
{
    qDebug() << "Loading model:" << filename;
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);

        QString line;
        QStringList tokens;

        while(!in.atEnd()) {
            line = in.readLine();
            if (line.startsWith("#")) continue; // skip comments

            tokens = line.split(" ",QString::SkipEmptyParts);

            // Switch depending on first element
            if (tokens[0] == "v")
            {
                parseVertex(tokens);
            }

            if (tokens[0] == "vn" )
            {
                parseNormal(tokens);
            }

            if (tokens[0] == "vt" )
            {
                parseTexture(tokens);
            }

            if (tokens[0] == "f" )
            {
                parseFace(tokens);
            }
        }

        // Use this function to prescale the model to fit inside a unit cube, centered around (0,0,0)
        // You have to implement it yourself if you want to use it!
        // unitize();

        // Create an array version of the model
        unpackIndices();

        // Align all vertex data with the vertex index
        if ( ! norm.isEmpty() ) {
            // Asserts that if texture coords are present
            // normals are also present
            alignData();
        }


        // Release the file resources
        file.close();
    } else {
        qDebug() << "Could not open the file!";
    }
}

// Prescale this model to fit inside a unit cube, centered around (0,0,0)
void OBJModel::unitize()
{
    // TODO: implement this yourself.
    // Use the private variable: vertices_indexed as source coordinates.
    // Scale all axis by the same amount (uniformal scaling) to keep model proportions the same!
    // After you are done, store them into the original variable.
}

void OBJModel::parseVertex(QStringList tokens)
{
    float x,y,z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    vertices_indexed.append(QVector3D(x,y,z));
}

void OBJModel::parseNormal(QStringList tokens)
{
    float x,y,z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    norm.append(QVector3D(x,y,z));
}

void OBJModel::parseTexture(QStringList tokens)
{
    float u,v;
    u = tokens[1].toFloat();
    v = tokens[2].toFloat();
    tex.append(QVector2D(u,v));
}

void OBJModel::parseFace(QStringList tokens)
{
    QStringList elements;

    for( int i = 1; i < tokens.size(); ++i )
    {
        elements = tokens[i].split("/");
        // -1 since .obj count from 1
        indices.append(elements[0].toInt()-1);

        if ( elements.size() > 1 && ! elements[1].isEmpty() )
        {
            texcoord_indices.append(elements[1].toInt()-1);
        }

        if (elements.size() > 2 && ! elements[2].isEmpty() )
        {
            normal_indices.append(elements[2].toInt()-1);
        }
    }
}

// Make sure that the indices from the vertices align with those
// of the normals and the texture coordinates, create extra vertices
// if vertex has multiple normals or texturecoords
void OBJModel::alignData()
{
    QVector<QVector3D> verts = QVector<QVector3D>();
    verts.reserve(vertices_indexed.size());
    QVector<QVector3D> norms = QVector<QVector3D>();
    norms.reserve(vertices_indexed.size());
    QVector<QVector2D> texcs = QVector<QVector2D>();
    texcs.reserve(vertices_indexed.size());
    QVector<Vertex> vs = QVector<Vertex>();

    QVector<unsigned> ind = QVector<unsigned>();
    ind.reserve(indices.size());

    unsigned currentIndex = 0;

    for (int i = 0; i < indices.size(); ++i) {
        QVector3D v = vertices_indexed[indices[i]];
        // TODO: check for normals
        QVector3D n = norm[normal_indices[i]];
        QVector2D t = QVector2D(0,0);
        if ( ! texcoords_indexed.isEmpty()) {
            t = tex[texcoord_indices[i]];
        }
        Vertex k = Vertex(v,n,t);
        if (vs.contains(k)) {
            // Vertex already exists, use that index
            ind.append(vs.indexOf(k));
        } else {
            // Create a new vertex
            verts.append(v);
            norms.append(n);
            texcs.append(t);
            vs.append(k);
            ind.append(currentIndex);
            ++currentIndex;
        }
    }
    // Set the new data
    vertices_indexed = verts;
    normals_indexed = norms;
    texcoords_indexed = texcs;
    indices = ind;
}


void OBJModel::unpackIndices()
{
    for ( int i = 0; i < indices.size(); ++i )
    {
        vertices.append(vertices_indexed[indices[i]]);

        if ( ! norm.isEmpty() )
        {
            normals.append(norm[normal_indices[i]]);
        }

        if ( ! tex.isEmpty() )
        {
            texcoords.append(tex[texcoord_indices[i]]);
        }
    }
}
