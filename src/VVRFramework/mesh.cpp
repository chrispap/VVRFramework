#include "mesh.h"
#include "geom.h"
#include "tiny_obj_loader.h"

#include <cstdio>
#include <ctime>
#include <cfloat>
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <QtOpenGL>

using namespace vvr;
using namespace tinyobj;

Mesh::Mesh(const string &objDir, const string &objFile, const string &texFile)
{
    vector<shape_t> shapes;
    string err = LoadObj(shapes, objFile.c_str(), objDir.c_str());
    if (!err.empty()) throw err;

    vector<float> &positions = shapes[0].mesh.vec_pos;
    vector<unsigned> &indices = shapes[0].mesh.vec_indices;
    vector<float> &normals = shapes[0].mesh.vec_norm;

    for (unsigned i=0; i<positions.size() ; i+=3)
        mVertices.push_back(Vec3d(positions[i], positions[i+1], positions[i+2]));

    for (unsigned i=0; i<indices.size() ; i+=3)
        mTriangles.push_back(Triangle(&mVertices, indices[i], indices[i+1], indices[i+2]));

    if (!shapes[0].mesh.vec_tex.empty() && !texFile.empty())
        mTexCoords = shapes[0].mesh.vec_tex;

    if (!normals.empty()) {
        for (unsigned i=0; i<indices.size() ; i+=3)
            mVertexNormals.push_back(Vec3d(normals[i], normals[i+1], normals[i+2]));
    }
    else {
        createNormals();
    }

    mAABB = Box(mVertices);
}

Mesh::Mesh(const Mesh &copyfrom):
    mVertices (copyfrom.mVertices),
    mTriangles (copyfrom.mTriangles),
    mVertexNormals (copyfrom.mVertexNormals),
    mTexCoords (copyfrom.mTexCoords),
    mRot (copyfrom.mRot),
    mPos (copyfrom.mPos)
{
    vector<Triangle>::iterator ti;
    for (ti=mTriangles.begin(); ti!= mTriangles.end(); ++ti)
        ti->vecList = &mVertices;
}

Mesh::~Mesh()
{

}

void Mesh::createNormals()
{
    // List of lists of the triangles that are connected to each vertex
    vector<set<int> > mVertexTriangles;

    mVertexTriangles.clear();
    mVertexTriangles.resize(mVertices.size());

    int i=0;
    vector<Triangle>::iterator ti;
    for (ti=mTriangles.begin(); ti!= mTriangles.end(); ++ti) {
        mVertexTriangles[ti->vi1].insert(i);
        mVertexTriangles[ti->vi2].insert(i);
        mVertexTriangles[ti->vi3].insert(i++);
    }

    mVertexNormals.clear();
    mVertexNormals.resize(mVertices.size());
    Vec3d normSum;
    for (unsigned vi=0; vi< mVertices.size(); ++vi) {
        normSum = Vec3d(0);
        set<int>::const_iterator _ti;
        for (_ti=mVertexTriangles[vi].begin(); _ti!=mVertexTriangles[vi].end(); ++_ti)
            normSum.add(mTriangles[*_ti].getNormal());
        mVertexNormals[vi] = normSum.scale(-1).normalize();
    }
}

void Mesh::updateTriangleData()
{
    vector<Triangle>::iterator ti;
    for (ti=mTriangles.begin(); ti!= mTriangles.end(); ++ti)
        ti->update();
}

void Mesh::setBigSize(float size)
{
    float s = size / mAABB.getMaxSize();

    vector<Vec3d>::iterator vi;
    for (vi = mVertices.begin(); vi != mVertices.end(); ++vi)
        vi->scale(s);

    mAABB.scale(s);

    updateTriangleData();
}

void Mesh::cornerAlign()
{
    Vec3d offs(-mAABB.min.x, -mAABB.min.y, -mAABB.min.z);
    move(offs);
}

void Mesh::move(const Vec3d &p)
{
    std::vector<Vec3d>::iterator vi;
    for (vi=mVertices.begin(); vi!= mVertices.end(); ++vi)
        vi->add(p);

    mAABB.add(p);

    updateTriangleData();
}

void Mesh::rotate(const Vec3d &p)
{

}

/* Drawing */
void Mesh::drawTriangles(ColRGB col, bool wire)
{
    bool normExist = !mVertexNormals.empty();
    bool texExists = !mTexCoords.empty();
    vector<Triangle>::const_iterator ti;

    if (texExists) {
        glEnable(GL_TEXTURE_2D);
        glColor4ub(0xFF, 0xFF, 0xFF, 0x00);
    }
    else {
        glDisable(GL_TEXTURE_2D);
        glColor3ubv(col.data);
    }

    glPolygonMode(GL_FRONT_AND_BACK, wire? GL_LINE: GL_FILL);
    glLineWidth(1);

    glBegin(GL_TRIANGLES);
    for(ti=mTriangles.begin(); ti!=mTriangles.end(); ++ti) {
        if (normExist) glNormal3dv(mVertexNormals[ti->vi1].data);
        if (texExists) glTexCoord2f(mTexCoords[2*ti->vi1], mTexCoords[2*ti->vi1+1]);
        glVertex3dv(ti->v1().data);

        if (normExist) glNormal3dv(mVertexNormals[ti->vi2].data);
        if (texExists) glTexCoord2f(mTexCoords[2*ti->vi2], mTexCoords[2*ti->vi2+1]);
        glVertex3dv(ti->v2().data);

        if (normExist) glNormal3dv(mVertexNormals[ti->vi3].data);
        if (texExists) glTexCoord2f(mTexCoords[2*ti->vi3], mTexCoords[2*ti->vi3+1]);
        glVertex3dv(ti->v3().data);
    }
    glEnd();
}

void Mesh::drawNormals(ColRGB col)
{
    Vec3d n;
    glBegin(GL_LINES);
    vector<Triangle>::const_iterator ti;

    for(ti=mTriangles.begin(); ti!=mTriangles.end(); ++ti) {
        n = ti->getCenter();
        glColor3ubv(ColRGB(0x00,0,0).data);
        glVertex3dv(n.data);
        n.add(ti->getNormal());
        glColor3ubv(ColRGB(0xFF,0,0).data);
        glVertex3dv(n.data);
    }

    glEnd();
    return;
}

void Mesh::draw(ColRGB col, Style x)
{
    glPushMatrix();

    glTranslatef(mPos.x, mPos.y, mPos.z);
    
    glRotatef(-90,			1, 0, 0);
    glRotatef(-mRot.x,		0, 0, 1);
    glRotatef(-mRot.y,		1, 0, 0);
    glRotatef( mRot.z,		0, 1, 0);
    glRotatef(-90,			0, 0, 1);

    if (x & SOLID) drawTriangles(col, false);
    if (x & WIRE) drawTriangles(col, true);
    if (x & NORMALS) drawNormals(col);
    if (x & BOUND) mAABB.draw(col);

    glPopMatrix();
}
