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

Mesh::Mesh()
{
    printf("[Mesh] Creating object in address 0x%p (default ctor) \n", this);
    fflush(0);
}

Mesh::~Mesh()
{
    printf("[Mesh] Deleting object in address 0x%p \n", this);
    fflush(0);
}

Mesh::Mesh(const string &objDir, const string &objFile, const string &texFile, bool ccw)
{
    printf("[Mesh] Creating object in address 0x%p \n", this);
    fflush(0);

    mCCW = ccw;

    std::vector<shape_t> shapes;
    std::vector<material_t> materials;
    string err = LoadObj(shapes, materials, objFile.c_str(), objDir.c_str());
    if (!err.empty()) throw err;

    vector<float> &positions = shapes[0].mesh.positions;
    vector<unsigned> &indices = shapes[0].mesh.indices;
    vector<float> &normals = shapes[0].mesh.normals;

    // Store vertices.
    for (unsigned i=0; i<positions.size() ; i+=3)
        mVertices.push_back(Vec3d(positions[i], positions[i+1], positions[i+2]));
    
    // Store faces (triangles).
    for (unsigned i=0; i<indices.size() ; i+=3)
        mTriangles.push_back(Triangle(&mVertices, indices[i], indices[i+2], indices[i+1]));

    // Store texture coordinates.
    if (!shapes[0].mesh.texcoords.empty() && !texFile.empty())
        mTexCoords = shapes[0].mesh.texcoords;

    // Store normals.
    // Or create them.
    if (!normals.empty()) {
        const int n = normals.size();
        for (unsigned i=0; i<n ; i+=3)
            mVertexNormals.push_back(Vec3d(normals[i], normals[i+1], normals[i+2]));
    }
    else createNormals();

    mAABB = Box(mVertices);
}

Mesh::Mesh(const Mesh *original):
    mCCW(original->mCCW),
    mVertices (original->mVertices),
    mTriangles (original->mTriangles),
    mVertexNormals (original->mVertexNormals),
    mTexCoords (original->mTexCoords),
    mAABB (original->mAABB),
    mRot (original->mRot),
    mPos (original->mPos),
    mTexName(original->mTexName)
{
    printf("[Mesh] Copying object from address 0x%p to 0x%p \n", original, this);
    fflush(0);

    vector<Triangle>::iterator ti;
    for (ti=mTriangles.begin(); ti!= mTriangles.end(); ++ti) {
        ti->vecList = &mVertices;
    }
}

Mesh::Mesh(const Mesh &original):
    mCCW(original.mCCW),
    mVertices (original.mVertices),
    mTriangles (original.mTriangles),
    mVertexNormals (original.mVertexNormals),
    mTexCoords (original.mTexCoords),
    mAABB (original.mAABB),
    mRot (original.mRot),
    mPos (original.mPos),
    mTexName(original.mTexName)
{
    printf("[Mesh] Copying object from address 0x%p to 0x%p \n", &original, this);
    fflush(0);

    vector<Triangle>::iterator ti;
    for (ti=mTriangles.begin(); ti!= mTriangles.end(); ++ti) {
        ti->vecList = &mVertices;
    }
}

void Mesh::operator=(const Mesh *src)
{
    printf("[Mesh] Assigning object from address 0x%p to 0x%p \n", src, this); fflush(0);

    mCCW = src->mCCW;
    mVertices = src->mVertices;
    mTriangles = src->mTriangles;
    mVertexNormals = src->mVertexNormals;
    mTexCoords = src->mTexCoords;
    mAABB = src->mAABB;
    mRot = src->mRot;
    mPos = src->mPos;
    mTexName= src->mTexName;

    vector<Triangle>::iterator ti;
    for (ti=mTriangles.begin(); ti!= mTriangles.end(); ++ti) {
        ti->vecList = &mVertices;
    }
}

void Mesh::operator=(const Mesh &src)
{
    printf("[Mesh] Assigning object from address 0x%p to 0x%p \n", &src, this); fflush(0);

    mCCW = src.mCCW;
    mVertices = src.mVertices;
    mTriangles = src.mTriangles;
    mVertexNormals = src.mVertexNormals;
    mTexCoords = src.mTexCoords;
    mAABB = src.mAABB;
    mRot = src.mRot;
    mPos = src.mPos;
    mTexName= src.mTexName;

    vector<Triangle>::iterator ti;
    for (ti=mTriangles.begin(); ti!= mTriangles.end(); ++ti) {
        ti->vecList = &mVertices;
    }
}

void Mesh::createNormals()
{
    // List of lists of the triangles that are connected to each vertex
    vector<set<int> > mVertexTriangles;
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
        double s = (mCCW ? -1.0 : 1.0) / normSum.length();
        mVertexNormals[vi] = normSum.scale(s);
    }
}

void Mesh::updateTriangleData()
{
    vector<Triangle>::iterator ti;
    for (ti=mTriangles.begin(); ti!= mTriangles.end(); ++ti)
        ti->update();
}

void Mesh::update()
{
    updateTriangleData();
    createNormals();
    mAABB = Box(mVertices);
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
    Vec3d offs(mAABB.min);
    offs.scale(-1.0);
    move(offs);
}

void Mesh::centerAlign()
{
    Vec3d offs(mAABB.max);
    offs.add(mAABB.min);
    offs.scale(-0.5);
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
void Mesh::drawTriangles(Colour col, bool wire)
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

void Mesh::drawNormals(Colour col)
{
    const float len = mAABB.getMaxSize() / 50;
    
    glBegin(GL_LINES);

    for(int i=0; i<mVertices.size(); i++) {
        Vec3d n = mVertices[i];
        glColor3ubv(Colour(0x00,0,0).data);
        glVertex3dv(n.data);
        Vec3d norm = mVertexNormals[i];
        norm.scale(len);
        n.add(norm);
        glColor3ubv(Colour(0xFF,0,0).data);
        glVertex3dv(n.data);
    }

    glEnd();
    return;
}

void Mesh::drawAxes()
{
    glBegin(GL_LINES);
    //[X]
    glColor3ub(0xFF, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(mAABB.getMaxSize(), 0, 0);
    //[Y]
    glColor3f(0, 0xFF, 0);
    glVertex3f(0,0,0);
    glVertex3f(0, mAABB.getMaxSize(), 0);
    //[Z]
    glColor3f(0, 0, 0xFF);
    glVertex3f(0,0,0);
    glVertex3f(0, 0, mAABB.getMaxSize());

    glEnd();
}

void Mesh::draw(Colour col, Style x)
{
    glPushMatrix();

    glTranslatef(mPos.x, mPos.y, mPos.z);
    
    glRotatef( mRot.x,  1, 0, 0);
    glRotatef( mRot.y,  0, 1, 0);
    glRotatef( mRot.z,  0, 0, 1);

    if (x & SOLID) drawTriangles(col, false);
    if (x & WIRE) drawTriangles(col, true);
    if (x & NORMALS) drawNormals(col);
    if (x & BOUND) mAABB.draw(col);
    if (x & AXES) drawAxes();

    glPopMatrix();
}
