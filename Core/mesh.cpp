#include <vvr/mesh.h>
#include <vvr/drawing.h>
#include <MathGeoLib.h>
#include "tiny_obj_loader.h"
#include <cstdio>
#include <ctime>
#include <cfloat>
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <QFile>
#include <QTextStream>
#include <QtGui> //gl.h

using namespace std;
using namespace vvr;
using namespace math;

void vvr::Triangle::update()
{
    A = v1().y*(v2().z - v3().z) + v2().y*(v3().z - v1().z) + v3().y*(v1().z - v2().z);
    B = v1().z*(v2().x - v3().x) + v2().z*(v3().x - v1().x) + v3().z*(v1().x - v2().x);
    C = v1().x*(v2().y - v3().y) + v2().x*(v3().y - v1().y) + v3().x*(v1().y - v2().y);
    D = -v1().x*(v2().y*v3().z - v3().y*v2().z) - v2().x*(v3().y*v1().z - v1().y*v3().z) - v3().x*(v1().y*v2().z - v2().y*v1().z);
}

const vec& vvr::Triangle::v1() const
{
    return (*vecList)[vi1];
}

const vec& vvr::Triangle::v2() const
{
    return (*vecList)[vi2];
}

const vec& vvr::Triangle::v3() const
{
    return (*vecList)[vi3];
}

const vec vvr::Triangle::getNormal() const
{
    return vec(A, B, C).Normalized();
}

const vec vvr::Triangle::getCenter() const
{
    return (v1() + v2() + v3()) / 3.0;
}

double vvr::Triangle::planeEquation(const vec &r) const
{
    return A*r.x + B*r.y + C*r.z + D;
}

Mesh::Mesh()
{
    mCCW = false;
    mMatrix.SetIdentity();
}

Mesh::Mesh(const string &objFile, const string &texFile, bool ccw)
{
    mCCW = ccw;
    mMatrix.SetIdentity();

    std::string err;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    bool load_ok = tinyobj::LoadObj(shapes, materials, err, objFile.c_str());
    if (!load_ok) throw err;

    vector<float> &positions = shapes[0].mesh.positions;
    vector<unsigned> &indices = shapes[0].mesh.indices;
    vector<float> &normals = shapes[0].mesh.normals;

    //! Store vertices
    for (unsigned i = 0; i < positions.size(); i += 3)
        mVertices.push_back(vec(positions[i], positions[i + 1], positions[i + 2]));

    //! Store faces [triangles]
    for (unsigned i = 0; i < indices.size(); i += 3)
        mTriangles.push_back(Triangle(&mVertices, indices[i], indices[i + 2], indices[i + 1]));

    //! Store normals
    if (!normals.empty()) {
        const size_t n = normals.size();
        for (size_t i = 0; i < n; i += 3)
            mVertexNormals.push_back(vec(normals[i], normals[i + 1], normals[i + 2]));
    }
    else createNormals(); //! Or create them...

    mAABB = aabbFromVertices(mVertices);
}

Mesh::Mesh(const Mesh &src)
    : mVertices(src.mVertices)
    , mTriangles(src.mTriangles)
    , mVertexNormals(src.mVertexNormals)
    , mMatrix(src.mMatrix)
    , mAABB(src.mAABB)
    , mCCW(src.mCCW)
{
    vector<Triangle>::iterator ti;
    for (ti = mTriangles.begin(); ti != mTriangles.end(); ++ti) {
        ti->vecList = &mVertices;
    }
}

void Mesh::operator=(const Mesh &src)
{
    mVertices = src.mVertices;
    mTriangles = src.mTriangles;
    mVertexNormals = src.mVertexNormals;
    mMatrix = src.mMatrix;
    mAABB = src.mAABB;
    mCCW = src.mCCW;

    vector<Triangle>::iterator ti;
    for (ti = mTriangles.begin(); ti != mTriangles.end(); ++ti) {
        ti->vecList = &mVertices;
    }
}

Mesh::~Mesh()
{
}

void Mesh::exportToObj(const string &filename)
{
    QFile file(QString::fromStdString(filename));

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setRealNumberNotation(QTextStream::FixedNotation);
        out.setRealNumberPrecision(6);

        out << "# Exported from VVRFramework" << "\n";
        out << "# Vertices: " << mVertices.size() << "\n";
        out << "# Normals: " << mVertexNormals.size() << "\n";
        out << "# Triangles: " << mTriangles.size() << "\n";

        //! Export vertices

        for (int i = 0; i < mVertices.size(); i++)
        {
            const vec &v = mVertices[i];
            out << "v"
                << " " << v.x
                << " " << v.y
                << " " << v.z
                << "\n";
        }

        //! Export normals

        for (int i = 0; i < mVertexNormals.size(); i++)
        {
            const vec &n = mVertexNormals[i];
            out << "vn"
                << " " << n.x
                << " " << n.y
                << " " << n.z
                << "\n";
        }

        //! Export faces

        out << "s 1" << "\n";
        for (int i = 0; i < mTriangles.size(); i++)
        {
            const Triangle &t = mTriangles[i];
            out << "f"
                << " " << t.vi1 + 1 << "//" << t.vi1 + 1
                << " " << t.vi2 + 1 << "//" << t.vi2 + 1
                << " " << t.vi3 + 1 << "//" << t.vi3 + 1
                << "\n";
        }

        file.close();
    }
}

void Mesh::createNormals()
{
    // List of lists of the triangles that are connected to each vertex
    vector<set<int> > vtris;
    vtris.resize(mVertices.size());

    int i = 0;
    vector<Triangle>::iterator ti;
    for (ti = mTriangles.begin(); ti != mTriangles.end(); ++ti) {
        vtris[ti->vi1].insert(i);
        vtris[ti->vi2].insert(i);
        vtris[ti->vi3].insert(i++);
    }

    mVertexNormals.clear();
    mVertexNormals.resize(mVertices.size());
    vec normSum;
    for (unsigned vi = 0; vi < mVertices.size(); ++vi) {
        normSum = vec::zero;
        set<int>::const_iterator _ti;
        for (_ti = vtris[vi].begin(); _ti != vtris[vi].end(); ++_ti)
            normSum += (mTriangles[*_ti].getNormal());
        double s = (mCCW ? -1.0 : 1.0) / normSum.Length();
        mVertexNormals[vi] = normSum.Mul(s);
    }
}

void Mesh::updateTriangleData()
{
    vector<Triangle>::iterator ti;
    for (ti = mTriangles.begin(); ti != mTriangles.end(); ++ti)
        ti->update();
}

void Mesh::update(const bool recomputeAABB)
{
    updateTriangleData();
    createNormals();
    if (recomputeAABB) mAABB = aabbFromVertices(mVertices);
}

float Mesh::getMaxSize() const
{
    const vec sz = mAABB.Size();
    return Max(sz.x, sz.y, sz.z);
}

void Mesh::setBigSize(float size)
{
    float s = size / getMaxSize();

    vector<vec>::iterator vi;
    for (vi = mVertices.begin(); vi != mVertices.end(); ++vi)
        *vi *= s;

    mAABB.Scale(vec::zero, s);

    updateTriangleData();
}

math::AABB Mesh::getAABB() const
{
    auto aabb(mAABB); 
    aabb.TransformAsAABB(mMatrix);
    return aabb;
}

void Mesh::cornerAlign()
{
    vec offs = mAABB.minPoint * -1;

    move(offs);
}

void Mesh::centerAlign()
{
    vec offs = (mAABB.maxPoint + mAABB.minPoint) / -2.0;
    move(offs);
}

void Mesh::move(const vec &p)
{
    for (auto &v : mVertices) v += p;
    mAABB.Translate(p);
    updateTriangleData();
}

void Mesh::transform(const math::float3x4 &t)
{
    for (auto &v : mVertices) v = t.TransformPos(v);
    mAABB.TransformAsAABB(t);
    update(false);
}

void Mesh::setTransform(const math::float3x4 &t)
{
    mMatrix = t;
}

void Mesh::drawTriangles(Colour col, bool wire)
{
    bool normExist = !mVertexNormals.empty();
    vector<Triangle>::const_iterator ti;

    glDisable(GL_TEXTURE_2D);
    glColor3ubv(col.data);
    glPolygonMode(GL_FRONT_AND_BACK, wire ? GL_LINE : GL_FILL);
    glLineWidth(1);

    glBegin(GL_TRIANGLES);
    for (ti = mTriangles.begin(); ti != mTriangles.end(); ++ti) 
    {
        if (normExist) glNormal3fv(mVertexNormals[ti->vi1].ptr());
        glVertex3fv(ti->v1().ptr());
        if (normExist) glNormal3fv(mVertexNormals[ti->vi2].ptr());
        glVertex3fv(ti->v2().ptr());
        if (normExist) glNormal3fv(mVertexNormals[ti->vi3].ptr());
        glVertex3fv(ti->v3().ptr());
    }
    glEnd();
}

void Mesh::drawNormals(Colour col)
{
    const float disp_length = getMaxSize() / 50;

    glBegin(GL_LINES);

    for (int i = 0; i < mVertices.size(); i++)
    {
        vec n = mVertices[i];
        glColor3ubv(Colour(0x00, 0, 0).data);
        glVertex3fv(n.ptr());
        vec norm = mVertexNormals[i];
        norm.ScaleToLength(disp_length);
        n += norm;
        glColor3ubv(Colour(0xFF, 0, 0).data);
        glVertex3fv(n.ptr());
    }

    glEnd();
    return;
}

void Mesh::drawAxes()
{
    glBegin(GL_LINES);
    //[X]
    glColor3ub(0xFF, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(getMaxSize(), 0, 0);
    //[Y]
    glColor3f(0, 0xFF, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, getMaxSize(), 0);
    //[Z]
    glColor3f(0, 0, 0xFF);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, getMaxSize());

    glEnd();
}

void Mesh::draw(Colour col, Style x)
{
    glPushMatrix();

    float4x4 M(mMatrix);
    M.Transpose();
    glMultMatrixf(M.ptr());

    if (x & SOLID) drawTriangles(col, false);
    if (x & WIRE) drawTriangles(col, true);
    if (x & NORMALS) drawNormals(col);
    if (x & BOUND) 
    {
        Aabb3D aabb(mAABB.MinX(), mAABB.MinY(), mAABB.MinZ(), mAABB.MaxX(), mAABB.MaxY(), mAABB.MaxZ(), col);
        aabb.setTransparency(0.88f);
        aabb.draw();
    }
    if (x & AXES) drawAxes();

    glPopMatrix();
}
