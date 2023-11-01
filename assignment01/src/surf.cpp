#include "surf.h"
#include "extra.h"
using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

Curve rotateCurveAroundYAxis(const Curve& curve, const Matrix3f& rotMat, const Matrix3f& rotNormMat)
{
    Curve result;
    for (unsigned int i = 0; i < curve.size(); i++)
    {
        CurvePoint curP;
        curP.V = rotMat * curve[i].V;
        curP.B = rotMat * curve[i].B;
        curP.T = rotMat * curve[i].T;
        curP.N = rotNormMat * curve[i].N;
        result.push_back(curP);
    }
    return result;
}

void curveVertexDataToSurfaceData(Surface& surf, const Curve& curve, const unsigned int maxVertNum)
{
    unsigned int currSurfVecSize = surf.VV.size();
    for (unsigned int i = 0; i < curve.size(); i++)
    {
        surf.VV.push_back(curve[i].V);
        Vector3f surfNormal = curve[i].N;
        surfNormal.negate();
        surf.VN.push_back(surfNormal);
        Tup3u vertexTriangle1;
        Tup3u vertexTriangle2;
        if (i < curve.size()-1 && (currSurfVecSize + i + curve.size()) < maxVertNum)
        {
            vertexTriangle1[0] = currSurfVecSize + i;
            vertexTriangle1[1] = currSurfVecSize + i + 1;
            vertexTriangle1[2] = currSurfVecSize + i + curve.size();
            vertexTriangle2[0] = currSurfVecSize + i + 1;
            vertexTriangle2[1] = currSurfVecSize + i + curve.size() + 1;
            vertexTriangle2[2] = currSurfVecSize + i + curve.size();
        }
        else if (i < curve.size() - 1 && (currSurfVecSize + i + curve.size()) >= maxVertNum)
        {
            vertexTriangle1[0] = currSurfVecSize + i;
            vertexTriangle1[1] = currSurfVecSize + i + 1;
            vertexTriangle1[2] = currSurfVecSize + i + curve.size()- maxVertNum;
            vertexTriangle2[0] = currSurfVecSize + i + 1;
            vertexTriangle2[1] = currSurfVecSize + i + curve.size() + 1 - maxVertNum;
            vertexTriangle2[2] = currSurfVecSize + i + curve.size() - maxVertNum;
        }
        surf.VF.push_back(vertexTriangle1);
        surf.VF.push_back(vertexTriangle2);
    }
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.
    float rotationAngleUnit = float(2 * M_PI / steps);
    
    std::cout << "Rotation: " << steps << " times " << rotationAngleUnit << " radians.\n";
    Matrix3f rotVertMat;
    Matrix3f rotNormMat;

    for (unsigned int i = 0; i < steps; i++)
    {
        float rotationAngle = 0.f;
        rotationAngle += float(i * rotationAngleUnit);
        rotVertMat = rotVertMat.rotateY(rotationAngle);
        rotNormMat = rotVertMat.inverse().transposed();
        Curve actualCurve;
        actualCurve = rotateCurveAroundYAxis(profile, rotVertMat, rotNormMat);
        unsigned int maxVertNum = profile.size() * steps;
        curveVertexDataToSurfaceData(surface, actualCurve, maxVertNum);
    }

    cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;
 
    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

    cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
