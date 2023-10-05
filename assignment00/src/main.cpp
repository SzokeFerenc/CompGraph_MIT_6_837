#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "vecmath.h"
#include "Camera.h"

using namespace std;

// Globals

double pi = 3.14159265359;

// This is the list of points (3D vectors)
vector<Vector3f> vecv;
float* vertexcoords = nullptr;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;
float* vertexnormals = nullptr;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned int>> vecf;
unsigned int* indices = nullptr;

// You will need more global variables to implement color and position changes

unsigned int vert_count = 0;
unsigned int ind_count = 0;
unsigned int norm_count = 0;

// Keyboard C counter global variable
unsigned int cCount = 0;

// Rotation on
bool rotOn = false;
double globRotDeg = 2.0;

// Global Light Position
GLfloat LightPos[] = { 1.0f, 1.0f };



// Global rotation matrix
GLfloat RotMat[16] =  { 1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 1.0 };



// Global camera matrix
// Position the camera at [0,0,5], looking at [0,0,0],
// with [0,1,0] as the up direction.

GLdouble CamMat[9] = { 0.0, 0.0, 5.0,
                      0.0, 0.0, 0.0,
                      0.0, 1.0, 0.0 };

Matrix3f camDataMatrix(0.0, 0.0, 5.0,
                       0.0, 0.0, 0.0,
                       0.0, 1.0, 0.0);

//Global camera class

Camera mainCamera = Camera(camDataMatrix);

// Global camera field of view
double fov = 50.0;

bool mousePosOn = false;
int mouseStartX = 0;
int mouseStartY = 0;
int mouseCurrentX = 0;
int mouseCurrentY = 0;

bool leftMousePosOn = false;
int leftMouseStartX = 0;
int leftMouseStartY = 0;
int leftMouseCurrentX = 0;
int leftMouseCurrentY = 0;

// Here are some global colors you might use - feel free to add more
GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                             {0.9, 0.5, 0.5, 1.0},
                             {0.5, 0.9, 0.3, 1.0},
                             {0.3, 0.8, 0.9, 1.0} };

GLfloat currentColor[4] = { 0.5, 0.5, 0.9, 1.0 };
GLfloat lastColor[4] = { 0.5, 0.5, 0.9, 1.0 };

// Rotation matrix calculator (rotation over axis Z)
void RotZMatCalc(double deg, GLfloat* rM)
{
    
    double rad = deg * pi / 180.0;
    rM[0] = (abs(cos(rad)) < 0.0000001) ? 0.0 : cos(rad);
    rM[1] = (abs(-1.0 * sin(rad)) < 0.0000001) ? 0.0 : -1.0 * sin(rad);
    rM[4] = (abs(sin(rad)) < 0.0000001) ? 0.0 : sin(rad);
    rM[5] = (abs(cos(rad)) < 0.0000001) ? 0.0 : cos(rad);
    for (int i = 0; i < 16; i++)
    {
        std::cout << rM[i] << std::endl;
    }
}

// Rotation matrix calculator (rotation over axis Y)
void RotYMatCalc(double deg, GLfloat* rM)
{
    double rad = deg * pi / 180.0;
    rM[0] = (abs(cos(rad)) < 0.0000001) ? 0.0 : cos(rad);
    rM[2] = (abs(sin(rad)) < 0.0000001) ? 0.0 :  sin(rad);
    rM[8] = (abs(-1.0 * sin(rad)) < 0.0000001) ? 0.0 : -1.0 * sin(rad);
    rM[10] = (abs(cos(rad)) < 0.0000001) ? 0.0 : cos(rad);
    /*for (int i = 0; i < 16; i++)
    {
        std::cout << rM[i] << std::endl;
    }*/
}

// Rotation matrix calculator (rotation over axis X)
void RotXMatCalc(double deg, GLfloat* rM)
{
    double rad = deg * pi / 180.0;
    rM[5] = (abs(cos(rad)) < 0.0000001) ? 0.0 : cos(rad);
    rM[6] = (abs(-1.0 * sin(rad)) < 0.0000001) ? 0.0 : -1.0 * sin(rad);
    rM[9] = (abs(sin(rad)) < 0.0000001) ? 0.0 : sin(rad);
    rM[10] = (abs(cos(rad)) < 0.0000001) ? 0.0 : cos(rad);
    for (int i = 0; i < 16; i++)
    {
        std::cout << rM[i] << std::endl;
    }
}

// Transformation -> multiplying with transformation matrix
void Rotation(GLfloat* tM, vector<Vector3f>& vecv, vector<Vector3f>& vecn)
{
    for (auto& i : vecv)
    {
        float initx = i.x();
        float inity = i.y();
        float initz = i.z();
        //std::cout << "Before: " << i.x() << "\t" << i.y() << "\t" << i.z() << std::endl;
        i.x() = tM[0] * initx + tM[1] * inity + tM[2] * initz;
        i.y() = tM[4] * initx + tM[5] * inity + tM[6] * initz;
        i.z() = tM[8] * initx + tM[9] * inity + tM[10] * initz;
        //std::cout << "After: " << i.x() << "\t" << i.y() << "\t" << i.z() << std::endl;
    }
    for (auto& i : vecn)
    {
        float initx = i.x();
        float inity = i.y();
        float initz = i.z();
        i.x() = tM[0] * initx + tM[1] * inity + tM[2] * initz;
        i.y() = tM[4] * initx + tM[5] * inity + tM[6] * initz;
        i.z() = tM[8] * initx + tM[9] * inity + tM[10] * initz;
    }
}


// Calculate rotation radians of camera




// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }


void update(int value)
{
    if (rotOn)
    {
        //std::cout << "update started\n";
        /*globRotDeg += 0.01;
        if (globRotDeg > 360.0)
        {
            globRotDeg -= globRotDeg;
        }*/
        RotYMatCalc(globRotDeg, RotMat);
        Rotation(RotMat, vecv, vecn);
        glutPostRedisplay();
        
    }
}

void calcColor(int value)
{
    //std::cout << "\ncurrentColor[0]: " << currentColor[0] << " currentColor[1]: " << currentColor[1] << " currentColor[2]: " << currentColor[2] << std::endl;
    //std::cout << "diffColors[cCount][0]: " << diffColors[cCount][0] << " diffColors[cCount][1]: " << diffColors[cCount][1] << " diffColors[cCount][2]: " << diffColors[cCount][2] << std::endl;

    if (abs(currentColor[0] - diffColors[cCount][0]) > 0.001f/*currentColor[0] != diffColors[cCount][0]*/ /*|| currentColor[1] != diffColors[cCount][1] || currentColor[2] != diffColors[cCount][2]*/)
    {
        GLfloat diffR = diffColors[cCount][0] - lastColor[0];
       /* GLfloat diffG = diffColors[cCount][1] - lastColor[1];
        GLfloat diffB = diffColors[cCount][2] - lastColor[2];*/
        currentColor[0] += (diffR / 10.0f);
        /*currentColor[1] += (diffG / 10.0);
        currentColor[2] += (diffB / 10.0);*/
    }
    if (abs(currentColor[1] - diffColors[cCount][1]) > 0.001f)
    {
        GLfloat diffG = diffColors[cCount][1] - lastColor[1];
        currentColor[1] += (diffG / 10.0f);
    }
    if (abs(currentColor[2] - diffColors[cCount][2]) > 0.001f)
    {
        GLfloat diffB = diffColors[cCount][2] - lastColor[2];
        currentColor[2] += (diffB / 10.0f);
    }
    if (abs(currentColor[0] - diffColors[cCount][0]) < 0.001f && abs(currentColor[1] - diffColors[cCount][1]) < 0.001f && abs(currentColor[2] - diffColors[cCount][2]) < 0.001f)
    {
        //std::cout << "\nhohohohohoho\n";
        lastColor[0] = currentColor[0];
        lastColor[1] = currentColor[1];
        lastColor[2] = currentColor[2];
    }
    
    //std::cout << "lastColor[0]: " << lastColor[0] << " lastColor[1]: " << lastColor[1] << " lastColor[2]: " << lastColor[2] << std::endl;
    glutPostRedisplay();
}


// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
        cCount = ++cCount % 4;
		cout << /*"Unhandled key press " << key << "." <<*/ " ccount: " << cCount << endl;
        break;
    case 'r':
        // add code to rotate the element
        rotOn = !rotOn;
        rotOn == true ? std::cout << "rotation ON\n" : std::cout << "rotation OFF\n";
        break;
    case 'w':
        globRotDeg += 2.0;
        std::cout << "current globRotDeg: " << globRotDeg << std::endl;
        break;
    case 's':
        globRotDeg -= 2.0;
        std::cout << "current globRotDeg: " << globRotDeg << std::endl;

        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}



// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
        LightPos[1] += 0.5f;
		cout << "Unhandled key press: up arrow." << endl;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
        LightPos[1] -= 0.5f;
		cout << "Unhandled key press: down arrow." << endl;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
        LightPos[0] -= 0.5f;
		cout << "Unhandled key press: left arrow." << endl;
        
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
        LightPos[0] += 0.5f;
		cout << "Unhandled key press: right arrow." << endl;
		break;
    }
    cout << "LightPos: " << LightPos[0] << " " << LightPos[1] << endl;

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

void getPressedMouseStartPosition(int button, int state, int x, int y)
{
    if (button == GLUT_MIDDLE_BUTTON && mousePosOn != true)
    {
        mouseStartX = x;
        mouseStartY = y;
    }
    else if (button == GLUT_LEFT_BUTTON && leftMousePosOn != true)
    {
        leftMouseStartX = x;
        leftMouseStartY = y;
    }
}

void getPressedMidMouseActualPosition(int x, int y)
{
    if (mousePosOn)
    {
        std::cout << "Middle mouse button movement in axis x: " << x - mouseStartX << " movement in axis y: " << y - mouseStartY << std::endl;
        mouseCurrentX = x;
        mouseCurrentY = y;
        /*CamMat[0] -= ((mouseCurrentX - mouseStartX) * 0.0001);
        CamMat[3] -= ((mouseCurrentX - mouseStartX) * 0.0001);
        CamMat[1] += ((mouseCurrentY - mouseStartY) * 0.0001);
        CamMat[4] += ((mouseCurrentY - mouseStartY) * 0.0001);*/
        mainCamera.PrimitiveMove((mouseCurrentX - mouseStartX) * 0.0001, (mouseCurrentY - mouseStartY) * 0.0001);
    }

    glutPostRedisplay();
}

void getPressedLeftMouseActualPosition(int x, int y)
{
    if (leftMousePosOn)
    {
        std::cout << "Left mouse button movement in axis x: " << x - leftMouseStartX << " movement in axis y: " << y - leftMouseStartY << std::endl;
        leftMouseCurrentX = x;
        leftMouseCurrentY = y;
        int diffx = x - leftMouseStartX;
        int diffy = y - leftMouseStartY;
        std::cout << "diffx: " << diffx << " diffy: " << diffy << std::endl;
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
        int biggerDimension = max(windowWidth, windowHeight);
        //std::cout << /*fov*/ mainCamera.GetFOV() << " deg of fOv is " << biggerDimension << " pixels." << std::endl;
        //std::cout << "Rotation over axis y: " << /*fov*/ mainCamera.GetFOV() / biggerDimension * diffx << " degrees over axis x: " << /*fov*/ mainCamera.GetFOV() / biggerDimension * diffy * -1.0 << " degrees." << std::endl;
        double rotOverXrad = mainCamera.GetFOV() / biggerDimension * diffy * -1.0 / 180.0 * pi;

        double quantifier = rotOverXrad / (2.0 * pi);
        
        //std::cout << "rotoverxrad: " << rotOverXrad << std::endl;

        if ((pi / 2.0) - rotOverXrad <= 0.0001)
        {
            rotOverXrad = 0.0;
        }
        else if ((pi / -2.0) - rotOverXrad >= -0.0001)
        {
            rotOverXrad = 0.0;
        }
        double rotOverYrad = mainCamera.GetFOV() / biggerDimension * diffx / 180.0 * pi;
        //std::cout << "Rotation over axis y: " << rotOverYrad << " radians over axis x: " << rotOverXrad << " radians." << std::endl;

        // Mi van ha itt kérjük le a kamera és cél poyíciókat és paraméterként adjuk a methodhoz?
        // így akkor mindig a kiinduló állapothoz képest van elforgatva az elem és nem az aktuális pozícióhoz képest.

        /*Vector4f camStart = mainCamera.GetCamPosVec();
        Vector4f targetStart = mainCamera.GetTargetPosVec();

        mainCamera.applyCamRotation(rotOverXrad, rotOverYrad, 0.0, camStart, targetStart);*/
        mainCamera.applyCamRotation(rotOverXrad, rotOverYrad, 0.0);
        //mainCamera.PrimitiveRotation(rotOverXrad, rotOverYrad, 0.0);
        /*CamMat[0] -= ((mouseCurrentX - mouseStartX) * 0.0001);
        CamMat[3] -= ((mouseCurrentX - mouseStartX) * 0.0001);
        CamMat[1] += ((mouseCurrentY - mouseStartY) * 0.0001);
        CamMat[4] += ((mouseCurrentY - mouseStartY) * 0.0001);*/

    }

    glutPostRedisplay();
}


void mouseFunc(int button, int state, int x, int y)
{
    std::cout << button << " " << state << " " << x << " " << y << std::endl;
    switch (button)
    {

    case GLUT_LEFT_BUTTON:
        getPressedMouseStartPosition(button, state, x, y);
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            
            leftMousePosOn = true;
        }
        else
            leftMousePosOn = false;
        leftMousePosOn == true ? std::cout << "Left mouse button down, mouse position ON\n" : std::cout << "Left mouse button up, mouse position OFF\n";
        glutMotionFunc(getPressedLeftMouseActualPosition);
        break;
    case GLUT_MIDDLE_BUTTON:
        getPressedMouseStartPosition(button, state, x, y);
        if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
        {
            mousePosOn = true;
        }
        else
            mousePosOn = false;
        glutMotionFunc(getPressedMidMouseActualPosition);
        
        
        //mousePosOn == true ? std::cout << "mouse position ON\n" : std::cout << "mouse position OFF\n";

        mousePosOn == true ? std::cout << "Middle mouse button down, mouse position ON\n" : std::cout << "middle mouse button up, mouse position OFF\n";
        break;
    
    case 3:
        /*CamMat[2] -= 0.5;
        CamMat[5] -= 0.5;*/
        //mainCamera.PrimitiveZoom(-0.5);
        mainCamera.PrimitiveZoom(-0.05);
        break;
    case 4:
        /*CamMat[2] += 0.5;
        CamMat[5] += 0.5;*/
        //mainCamera.PrimitiveZoom(0.5);
        mainCamera.PrimitiveZoom(0.05);
        break;
    
    }
    
    glutPostRedisplay();
}

void glutSolidOBJ()
{
    // original draw algorithm

    /*glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < vecf.size(); i++)
    {
        glNormal3d(vecn[vecf[i][2] - 1][0], vecn[vecf[i][2] - 1][1], vecn[vecf[i][2] - 1][2]);
        glVertex3d(vecv[vecf[i][0] - 1][0], vecv[vecf[i][0] - 1][1], vecv[vecf[i][0] - 1][2]);
        glNormal3d(vecn[vecf[i][5] - 1][0], vecn[vecf[i][5] - 1][1], vecn[vecf[i][5] - 1][2]);
        glVertex3d(vecv[vecf[i][3] - 1][0], vecv[vecf[i][3] - 1][1], vecv[vecf[i][3] - 1][2]);
        glNormal3d(vecn[vecf[i][8] - 1][0], vecn[vecf[i][8] - 1][1], vecn[vecf[i][8] - 1][2]);
        glVertex3d(vecv[vecf[i][6] - 1][0], vecv[vecf[i][6] - 1][1], vecv[vecf[i][6] - 1][2]);
    }
    glEnd();*/

    // using display list

    /*GLuint index = glGenLists(1);

    glNewList(index, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (unsigned int i = 0; i < vecf.size(); i++)
        {
            glNormal3d(vecn[vecf[i][2] - 1][0], vecn[vecf[i][2] - 1][1], vecn[vecf[i][2] - 1][2]);
            glVertex3d(vecv[vecf[i][0] - 1][0], vecv[vecf[i][0] - 1][1], vecv[vecf[i][0] - 1][2]);
            glNormal3d(vecn[vecf[i][5] - 1][0], vecn[vecf[i][5] - 1][1], vecn[vecf[i][5] - 1][2]);
            glVertex3d(vecv[vecf[i][3] - 1][0], vecv[vecf[i][3] - 1][1], vecv[vecf[i][3] - 1][2]);
            glNormal3d(vecn[vecf[i][8] - 1][0], vecn[vecf[i][8] - 1][1], vecn[vecf[i][8] - 1][2]);
            glVertex3d(vecv[vecf[i][6] - 1][0], vecv[vecf[i][6] - 1][1], vecv[vecf[i][6] - 1][2]);
        }
        glEnd();
    glEndList();

    glCallList(index);
    
    glDeleteLists(index, 1);*/

    // using display list with elementsdraw

    vertexnormals = new float[norm_count*3];

    for (unsigned int i = 0; i < vecf.size(); i++)
    {
        vertexnormals[(vecf[i][0] - 1) * 3] = vecn[vecf[i][2] - 1][0];
        vertexnormals[(vecf[i][0] - 1) * 3 + 1] = vecn[vecf[i][2] - 1][1];
        vertexnormals[(vecf[i][0] - 1) * 3 + 2] = vecn[vecf[i][2] - 1][2];
        vertexnormals[(vecf[i][3] - 1) * 3] = vecn[vecf[i][5] - 1][0];
        vertexnormals[(vecf[i][3] - 1) * 3 + 1] = vecn[vecf[i][5] - 1][1];
        vertexnormals[(vecf[i][3] - 1) * 3 + 2] = vecn[vecf[i][5] - 1][2];
        vertexnormals[(vecf[i][6] - 1) * 3] = vecn[vecf[i][8] - 1][0];
        vertexnormals[(vecf[i][6] - 1) * 3 + 1] = vecn[vecf[i][8] - 1][1];
        vertexnormals[(vecf[i][6] - 1) * 3 + 2] = vecn[vecf[i][8] - 1][2];
    }

    vertexcoords = new float[vert_count*3];

    for (int i = 0; i < vert_count; i++)
    {
        vertexcoords[i * 3] = vecv[i].x();
        vertexcoords[i * 3 + 1] = vecv[i].y();
        vertexcoords[i * 3 + 2] = vecv[i].z();
    }

    indices = new unsigned int[ind_count];

    for (int i = 0; i < vecf.size(); i++)
    {
        indices[i * 3] = vecf[i][0] - 1;
        indices[i * 3 + 1] = vecf[i][3] - 1;
        indices[i * 3 + 2] = vecf[i][6] - 1;
    }

    GLuint index = glGenLists(1);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertexcoords);
    glNormalPointer(GL_FLOAT, 0, vertexnormals);

    glNewList(index, GL_COMPILE);

    glDrawElements(GL_TRIANGLES, ind_count, GL_UNSIGNED_INT, indices);

    glEndList();

    glCallList(index);

    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glDeleteLists(index, 1);

}


// This function is responsible for displaying the object.
void drawScene(void)
{
    //int i;
    if (rotOn)
    {
        glutTimerFunc(25, update, 0);
    }

    if (mousePosOn)
    {
        //glutTimerFunc(25, getCurrentPressedMousePos, 0);
    }

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity
    

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    /*gluLookAt(CamMat[0], CamMat[1], CamMat[2],
              CamMat[3], CamMat[4], CamMat[5],
              CamMat[6], CamMat[7], CamMat[8]);*/

    double* lookAt = mainCamera.CrLookAtArr();

    gluLookAt(lookAt[0], lookAt[1], lookAt[2],
              lookAt[3], lookAt[4], lookAt[5],
              lookAt[6], lookAt[7], lookAt[8]);

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    /*GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };

    GLfloat currentColor[4] = { 0.5, 0.5, 0.9, 1.0 };*/
    


    if (currentColor[0] != diffColors[cCount][0] || currentColor[1] != diffColors[cCount][1] || currentColor[2] != diffColors[cCount][2])
    {
        glutTimerFunc(25, calcColor, 0);
    }

	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, /*diffColors[cCount]*/ currentColor);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
	GLfloat Lt0pos[] = {1.0f, 1.0f, 5.0f, 1.0f};
    Lt0pos[0] = LightPos[0];
    Lt0pos[1] = LightPos[1];

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	//glutSolidTeapot(1.0);
    
    glutSolidOBJ();
    


    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    //gluPerspective(fov, 1.0, 1.0, 100.0);
    gluPerspective(mainCamera.GetFOV(), mainCamera.GetAspectRatio(), mainCamera.GetNear(), mainCamera.GetFar());
}

void loadInput()
{
    char buffer[100];

    std::string s;
    while (cin.getline(buffer, 100))
    {
        
        stringstream ss(buffer);
        Vector3f v;
        vector<unsigned int> vu = {0,0,0,0,0,0,0,0,0};
        ss >> s;
        if (s == "v")
        {
            ss >> v[0] >> v[1] >> v[2];
            vecv.push_back(v);
            vert_count += 1;
            //std::cout << v[0] << " " << v[1] << " " << v[2] << std::endl;
        }
        else if (s == "vn")
        {
            ss >> v[0] >> v[1] >> v[2];
            vecn.push_back(v);
            norm_count += 1;
            //std::cout << v[0] << " " << v[1] << " " << v[2] << std::endl;
        }
        else if (s == "f")
        {
            char fill;
            ss >> vu[0] >> fill >> vu[1] >> fill >> vu[2];
            ss >> vu[3] >> fill >> vu[4] >> fill >> vu[5];
            ss >> vu[6] >> fill >> vu[7] >> fill >> vu[8];
            vecf.push_back(vu);
            ind_count += 3;
            //std::cout << vu[0] << " " << fill << " " << vu[8] << std::endl;
            //std::cout << vecf[0][8] << std::endl;
        }

    }
	// load the OBJ file here
}


// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    std::cout << "Use the application by typing:\n"
        "assignment00 < filepath/some.obj\n\n";
    

    loadInput();
    
    /*std::cout << "Model has " << vecv.size() << " vertices." << std::endl;
    std::cout << "Model has " << vecn.size() << " normals." << std::endl;
    std::cout << "Model has " << vecf.size() << " triangles." << std::endl;
    std::cout << "Model has " << vecf.size() * 3 << " indices." << std::endl;

    std::cout << "Model has " << vert_count << " vertices." << std::endl;
    std::cout << "Model has " << norm_count << " normals." << std::endl;
    std::cout << "Model has " << vecf.size() << " triangles." << std::endl;
    std::cout << "Model has " << ind_count << " indices." << std::endl;*/

    glutInit(&argc, argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Initial parameters for window position and size
    glutInitWindowPosition(60, 60);
    glutInitWindowSize(360, 360);
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    std::cout << glGetString(GL_VERSION) << std::endl;

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys
    glutMouseFunc(mouseFunc); // Handles mouse buttons

    
     // Set up the callback function for resizing windows
    glutReshapeFunc(reshapeFunc);

    // Call this whenever window needs redrawing
    glutDisplayFunc(drawScene);

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

    if (indices != nullptr)
        delete[] indices;
    if (vertexcoords != nullptr)
        delete[] vertexcoords;
    if (vertexnormals != nullptr)
        delete[] vertexnormals;

    return 0;	// This line is never reached.
}
