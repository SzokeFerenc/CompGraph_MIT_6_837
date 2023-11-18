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

Matrix4f calculateTransformationMatrix(const CurvePoint& sweepCurvePoint)
{
    Matrix4f transMat;
    Vector4f normal(sweepCurvePoint.N, 0.0f);
    transMat.setCol(0, normal);
    Vector4f binormal(sweepCurvePoint.B, 0.0f);
    transMat.setCol(1, binormal);
    Vector4f tangent(sweepCurvePoint.T, 0.0f);
    transMat.setCol(2, tangent);
    Vector4f vertex(sweepCurvePoint.V, 1.0f);
    transMat.setCol(3, vertex);
    return transMat;
}

Curve transformCurve(const Curve& profile, const Matrix4f& tranMat)
{
    Curve result;
    for (unsigned int i = 0; i < profile.size(); i++)
    {
        CurvePoint curP;
        curP.N = (tranMat * Vector4f(profile[i].N, 0.f)).xyz();
        curP.B = (tranMat * Vector4f(profile[i].B, 0.f)).xyz();
        curP.T = (tranMat * Vector4f(profile[i].T, 0.f)).xyz();
        curP.V = (tranMat * Vector4f(profile[i].V, 1.f)).xyz();
        result.push_back(curP);
    }
    return result;
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;

    sweepSteps = steps;
    profileSteps = profile.size();

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

    //cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;
 
    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    sweepSteps = sweep.size();
    profileSteps = profile.size();

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

    //std::cout << "Surface will have " << sweep.size() << " profiles\n";
    unsigned int steps = sweep.size();
    for (unsigned int i = 0; i < steps; i++)
    {
        Matrix4f transformationMat = calculateTransformationMatrix(sweep[i]);
        Curve actualCurve;
        actualCurve = transformCurve(profile, transformationMat);
        unsigned int maxVertNum = profile.size() * steps;
        curveVertexDataToSurfaceData(surface, actualCurve, maxVertNum); // creates face triangles, connects the end to the beginning
    }

    //cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

    return surface;
}

// function to calculate color depending on the angle of the vertex normal to
// axis Y -> red
// axis X -> green
// axis Z -> blue
GLfloat* colorDependingOnCurvature(const Surface& surf, unsigned int i, unsigned int j)
{
    GLfloat result[4] = { 0.3f, 0.3f, 0.3f, 1.f };
    
    float angleDiff = 0.f;
    float dotProdNorm = 0.f;
    Vector3f base{ 0.f, 0.f, 0.f };

    // dealing with direction up (Y)

    dotProdNorm = Vector3f::dot(surf.VN[surf.VF[i][j]], Vector3f{ 0.f, 1.f, 0.f });
    
    if (dotProdNorm > 1.f) dotProdNorm = 1.f;
    if (dotProdNorm < -1.f) dotProdNorm = -1.f;
    angleDiff = acos(dotProdNorm);
    
    result[0] = abs(angleDiff / M_PI);

    // dealing with direction left-right (X)

    dotProdNorm = Vector3f::dot(surf.VN[surf.VF[i][j]], Vector3f{ 1.f, 0.f, 0.f });
    
    if (dotProdNorm > 1.f) dotProdNorm = 1.f;
    if (dotProdNorm < -1.f) dotProdNorm = -1.f;
    angleDiff = acos(dotProdNorm);
    
    result[1] = abs(angleDiff / M_PI);

    // dealing with direction depth (Z)

    dotProdNorm = Vector3f::dot(surf.VN[surf.VF[i][j]], Vector3f{ 0.f, 0.f, 1.f });
    
    if (dotProdNorm > 1.f) dotProdNorm = 1.f;
    if (dotProdNorm < -1.f) dotProdNorm = -1.f;
    angleDiff = acos(dotProdNorm);
    
    result[2] = abs(angleDiff / M_PI);

    /*for (unsigned int k = 0; k < 3; k++)
    {
        base[k] = 1.f;
        dotProdNorm = Vector3f::dot(surf.VN[surf.VF[i][j]], base);
        if (dotProdNorm > 1.f) dotProdNorm = 1.f;
        if (dotProdNorm < -1.f) dotProdNorm = -1.f;
        angleDiff = acos(dotProdNorm);
        result[k] = abs(angleDiff / M_PI);
    }*/
    
    return result;
}

// function to calculate color depending on the angle of the adjacent vertex normals
GLfloat* colorDependingOnProfileCurvature(const Surface& surf, unsigned int i, unsigned int j)
{
    GLfloat result[4] = { 0.3f, 0.3f, 1.f, 1.f };

    float angleDiff = 0.f;
    float dotProdNorm = 0.f;

    unsigned index = surf.VF[i][j];
    unsigned nextIndex = 0;

    // profile curvature

    if ((index + 1) % profileSteps != 0)
    {
        nextIndex = index + 1;
    }
    else if ((index + 1) % profileSteps == 0)
    {
        nextIndex = index - profileSteps + 1;
    }

    dotProdNorm = Vector3f::dot(surf.VN[index], surf.VN[nextIndex]);

    if (dotProdNorm > 1.f) dotProdNorm = 1.f;
    if (dotProdNorm < -1.f) dotProdNorm = -1.f;
    angleDiff = acos(dotProdNorm);
    //std::cout << "anglediff: " << angleDiff << std::endl;

    result[0] = abs(angleDiff / M_PI);

    // sweep curvature

    if ((index + profileSteps) < surf.VV.size())
    {
        nextIndex = index + profileSteps;
    }
    else
    {
        nextIndex = 0 + index % profileSteps;
    }

    dotProdNorm = Vector3f::dot(surf.VN[index], surf.VN[nextIndex]);

    if (dotProdNorm > 1.f) dotProdNorm = 1.f;
    if (dotProdNorm < -1.f) dotProdNorm = -1.f;
    angleDiff = acos(dotProdNorm);
    //std::cout << "anglediff: " << angleDiff << std::endl;

    result[1] = abs(angleDiff / M_PI);

    return result;
}

void drawSurface(const Surface &surface, bool shaded, int coloring)
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

        // Enable color material to use custom vertex colors
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
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
        GLfloat col1[4]{ 0.3f, 0.3f, 0.3f, 1.f };
        GLfloat col2[4]{ 0.3f, 0.3f, 0.3f, 1.f };
        GLfloat col3[4]{ 0.3f, 0.3f, 0.3f, 1.f };

        if (coloring == 1)
        {
            GLfloat* temp = colorDependingOnCurvature(surface, i, 0);
            col1[0] = temp[0];
            col1[1] = temp[1];
            col1[2] = temp[2];
            temp = colorDependingOnCurvature(surface, i, 1);
            col2[0] = temp[0];
            col2[1] = temp[1];
            col2[2] = temp[2];
            temp = colorDependingOnCurvature(surface, i, 2);
            col3[0] = temp[0];
            col3[1] = temp[1];
            col3[2] = temp[2];
        }
        else if (coloring == 0)
        {
            GLfloat* temp = colorDependingOnProfileCurvature(surface, i, 0);
            col1[0] = temp[0];
            col1[1] = temp[1];
            col1[2] = temp[2];
            temp = colorDependingOnProfileCurvature(surface, i, 1);
            col2[0] = temp[0];
            col2[1] = temp[1];
            col2[2] = temp[2];
            temp = colorDependingOnProfileCurvature(surface, i, 2);
            col3[0] = temp[0];
            col3[1] = temp[1];
            col3[2] = temp[2];
        }
        glColor4f(col1[0], col1[1], col1[2], col1[3]);
        /*glColor4f(1.f, 0.f, 0.f, 0.f);*/
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);

        glColor4f(col2[0], col2[1], col2[2], col2[3]);
        /*glColor4f(0.f, 1.f, 0.f, 0.f);*/
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        
        glColor4f(col3[0], col3[1], col3[2], col3[3]);
        /*glColor4f(0.f, 0.f, 1.f, 0.f);*/
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
        if (i >= profileSteps && i < 2*profileSteps)
        {
            glColor4f(1, 0, 0, 1);
        }
        else if (i % profileSteps == 0)
        {
            glColor4f(0, 0, 1, 1);
        }
        else
            glColor4f(0, 1, 1, 1);
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
