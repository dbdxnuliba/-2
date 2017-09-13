#ifndef ROBOT
#define ROBOT

#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Texture2D>
#include <osgViewer/Viewer>
#include <osg/PolygonMode>
#include <osgDB/ReadFile>
#include <osg/Math>
#include <osg/Matrixd>
#include <osg/MatrixTransform>
#include <osg/Vec3d>
#include <osg/io_utils>
#include <osgText/Text>
#include <osg/BlendFunc>
#include <iostream>
#include <osg/PositionAttitudeTransform>
#include "qdebug.h"
#include "dataupdate.h"
/*
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "LoadMeshFromSTL.h"
*/
extern bool selfCollision;
extern bool virtualWallSetted;
extern bool touchVirtualWall;

typedef struct model{
    float x = 0;
    float y = 0;
    float z = 0;
    float r = 0;
}model;

typedef struct wall{
    double len = 0;
    double rot = 0;
    bool enable = false;
}wall;

class robot
{
public:
    robot();
    ~robot();

    void update(double angle[7]);
    osg::Group* createShapes();
    osg::MatrixTransform *createAxis(osg::Transform* previousJoint, float x, float y, float z, float rx, float ry, float rz);
    void createBaseAxis();
    osg::MatrixTransform *createWall(float length, float rotation, int kind);
    void buildWalls(bool visible, double length, double rotation, int kind);
    void createSphere(osg::Transform* previousJoint);
    void createTool(osg::Transform* previousJoint, float x, float y, float z);
    void setTool(bool visible, float x = 0, float y = 0, float z = 0, float rx = 0, float ry = 0, float rz = 0);
    bool checkCollision(model A, model B);
    bool checkWallCollision();
    void createNode(float x, float y, float z, char *txt);  //create program node
    void clearNodes();                                      //clean exist nodes

private:
    osg::MatrixTransform* buildJoint1(osg::MatrixTransform* previousJoint);
    osg::MatrixTransform* buildJoint2(osg::MatrixTransform* previousJoint);
    osg::MatrixTransform* buildJoint3(osg::MatrixTransform* previousJoint);
    osg::MatrixTransform* buildJoint4(osg::MatrixTransform* previousJoint);
    osg::MatrixTransform* buildJoint5(osg::MatrixTransform* previousJoint);
    osg::MatrixTransform* buildJoint6(osg::MatrixTransform* previousJoint);
    osg::MatrixTransform* buildJoint7(osg::MatrixTransform* previousJoint);
    osg::MatrixTransform* buildEndEffector();
    osg::TessellationHints* hints = new osg::TessellationHints;

    std::vector<osg::Geode *> programNodes;
    osg::Group* group;
    osg::Geode *toolNode;
    osg::MatrixTransform* horizonWall1;     //horizon wall 1
    osg::MatrixTransform* horizonWall2;     //horizon wall 2
    osg::MatrixTransform* verticalWall1;    //vertical wall 1
    osg::MatrixTransform* verticalWall2;    //vertical wall 2
    osg::MatrixTransform* verticalWall3;    //vertical wall 3
    osg::MatrixTransform* verticalWall4;    //vertical wall 4
    osg::MatrixTransform* toolBaseSave;
    osg::MatrixTransform* jointBase = new osg::MatrixTransform();
    osg::MatrixTransform* joint1;
    osg::MatrixTransform* joint2;
    osg::MatrixTransform* joint3;
    osg::MatrixTransform* joint4;
    osg::MatrixTransform* joint5;
    osg::MatrixTransform* joint6;
    osg::MatrixTransform* joint7;

    model model0;
    model model1;
    model model2;
    model model3;
    model model4;
    model model5;
    model model6;
    model model7;
    model model8;

    bool useTool = false;

    wall horizonWall1pos;
    wall horizonWall2pos;
    wall verticalWall1pos;
    wall verticalWall2pos;
    wall verticalWall3pos;
    wall verticalWall4pos;


    osg::Matrix joint1BaseMatrix;
    osg::Matrix joint2BaseMatrix;
    osg::Matrix joint3BaseMatrix;
    osg::Matrix joint4BaseMatrix;
    osg::Matrix joint5BaseMatrix;
    osg::Matrix joint6BaseMatrix;
    osg::Matrix joint7BaseMatrix;

/*
    //bullet objects
    btDefaultCollisionConfiguration* g_colConfig;
    btCollisionDispatcher* g_dispatcher;
    btBroadphaseInterface* g_broadInterface;
    btSequentialImpulseConstraintSolver* g_solver;
    btDynamicsWorld* g_world;

    std::vector<btCollisionObject*> g_joints;
    btCollisionObject* g_jointRead(std::string fname, osg::Matrix matj, int num);
    osg::Matrix oldMat;
    */
};

#endif // ROBOT
