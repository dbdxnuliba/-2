#include "robot.h"
#include "dataupdate.h"
#include "qdebug.h"
#include <QFile>
extern bool robotModel5Kg;
extern QString fileSavePath;
extern AdsMessage_Read  adsReadStruct;

robot::robot(){
    /*
    ///collision configuration contains default setup for memory, collision setup.
    g_colConfig = new btDefaultCollisionConfiguration();
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    g_dispatcher = new btCollisionDispatcher(g_colConfig);
    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    g_broadInterface = new btDbvtBroadphase();
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    g_solver = new btSequentialImpulseConstraintSolver;
    g_world = new btDiscreteDynamicsWorld(g_dispatcher, g_broadInterface, g_solver, g_colConfig);
    g_world->setGravity(btVector3(0,-9.81f,0));
    */
}

void robot::update(double angle[])
{
    osg::Matrix zRot1;
    zRot1.makeRotate(angle[0], 0.0,0.0,1.0);
    joint1->setMatrix(zRot1*joint1BaseMatrix);
    //bullet rotation
    //btTransform upTransform = g_joints[0]->getWorldTransform();
    //upTransform.setRotation(btQuaternion(0.0,0.0,1.0,angle[0]));
    //g_joints[0]->setWorldTransform(upTransform);


    osg::Matrix zRot2;
    zRot2.makeRotate(angle[1], 0.0,0.0,1.0);
    joint2->setMatrix(zRot2*joint2BaseMatrix);
    //bullet rotation
    //upTransform = g_joints[1]->getWorldTransform();
    //upTransform.setRotation(btQuaternion(0.0,0.0,1.0,angle[1]));
    //g_joints[1]->setWorldTransform(upTransform);

    osg::Matrix zRot3;
    zRot3.makeRotate(angle[2], 0.0,0.0,1.0);
    joint3->setMatrix(zRot3*joint3BaseMatrix);
    //bullet rotation
    //upTransform = g_joints[2]->getWorldTransform();
    //upTransform.setRotation(btQuaternion(0.0,0.0,1.0,angle[2]));
    //g_joints[2]->setWorldTransform(upTransform);

    osg::Matrix zRot4;
    zRot4.makeRotate(angle[3], 0.0,0.0,1.0);
    joint4->setMatrix(zRot4*joint4BaseMatrix);
    //bullet rotation
    //upTransform = g_joints[3]->getWorldTransform();
    //upTransform.setRotation(btQuaternion(0.0,0.0,1.0,angle[3]));
    //g_joints[3]->setWorldTransform(upTransform);

    osg::Matrix zRot5;
    zRot5.makeRotate(angle[4], 0.0,0.0,1.0);
    joint5->setMatrix(zRot5*joint5BaseMatrix);
    //bullet rotation
    //upTransform = g_joints[4]->getWorldTransform();
    //upTransform.setRotation(btQuaternion(0.0,0.0,1.0,angle[4]));
    //g_joints[4]->setWorldTransform(upTransform);

    osg::Matrix zRot6;
    if(robotModel5Kg) zRot6.makeRotate(-angle[5], 0.0,0.0,1.0);
    else zRot6.makeRotate(angle[5], 0.0,0.0,1.0);
    joint6->setMatrix(zRot6*joint6BaseMatrix);
    //bullet rotation
    //upTransform = g_joints[5]->getWorldTransform();
    //upTransform.setRotation(btQuaternion(0.0,0.0,1.0,angle[5]));
    //g_joints[5]->setWorldTransform(upTransform);

    osg::Matrix zRot7;
    zRot7.makeRotate(angle[6], 0.0,0.0,1.0);
    joint7->setMatrix(zRot7*joint7BaseMatrix);
    //bullet rotation
    //upTransform = g_joints[6]->getWorldTransform();
    //upTransform.setRotation(btQuaternion(0.0,0.0,1.0,angle[6]));
    //g_joints[6]->setWorldTransform(upTransform);

    //make models
    osg::Matrix mat[8];
    mat[0] = jointBase->getWorldMatrices().at(0);
    mat[1] = joint1->getWorldMatrices().at(0);
    mat[2] = joint2->getWorldMatrices().at(0);
    mat[3] = joint3->getWorldMatrices().at(0);
    mat[4] = joint4->getWorldMatrices().at(0);
    mat[5] = joint5->getWorldMatrices().at(0);
    mat[6] = joint6->getWorldMatrices().at(0);
    mat[7] = joint7->getWorldMatrices().at(0);

    model0.x = mat[0].operator()(3, 0);
    model0.y = mat[0].operator()(3, 1);
    model0.z = mat[0].operator()(3, 2);
    model0.r = 100;
    model1.x = mat[1].operator()(3, 0);
    model1.y = mat[1].operator()(3, 1);
    model1.z = mat[1].operator()(3, 2);
    model1.r = 100;
    model2.x = mat[2].operator()(3, 0);
    model2.y = mat[2].operator()(3, 1);
    model2.z = mat[2].operator()(3, 2);
    model2.r = 100;
    model3.x = mat[3].operator()(3, 0);
    model3.y = mat[3].operator()(3, 1);
    model3.z = mat[3].operator()(3, 2);
    model3.r = 100;
    model4.x = mat[4].operator()(3, 0);
    model4.y = mat[4].operator()(3, 1);
    model4.z = mat[4].operator()(3, 2);
    model4.r = 100;
    model5.x = mat[5].operator()(3, 0);
    model5.y = mat[5].operator()(3, 1);
    model5.z = mat[5].operator()(3, 2);
    model5.r = 100;
    model6.x = mat[6].operator()(3, 0);
    model6.y = mat[6].operator()(3, 1);
    model6.z = mat[6].operator()(3, 2);
    model6.r = 100;
    model7.x = mat[7].operator()(3, 0);
    model7.y = mat[7].operator()(3, 1);
    model7.z = mat[7].operator()(3, 2);
    model7.r = 30;

    //tool position save
    if(useTool){
        osg::Matrix matTool = toolBaseSave->getWorldMatrices().at(0);
        model8.x = matTool.operator()(3, 0);
        model8.y = matTool.operator()(3, 1);
        model8.z = matTool.operator()(3, 2);
    }else{
        model8.x =  model7.x;
        model8.y =  model7.y;
        model8.z =  model7.z;
    }

/*
    //bullet transform
    for(int i=0; i<7; i++)
    {
        btTransform upTrans;
        upTrans.setOrigin(btVector3(mat[i].operator ()(3,0),mat[i].operator ()(3,1),mat[i].operator ()(3,2)));
        osg::Quat matRot=mat[i].getRotate();
        upTrans.setRotation(btQuaternion(matRot.x(),matRot.y(),matRot.z(),matRot.w()));
        g_joints[i]->setWorldTransform(upTrans);
    }

    //check self collision
    //if(joint7->getWorldMatrices().at(0)!=oldMat)
    if(adsReadStruct.stateMachine == 3)
    {
        g_world->stepSimulation(0.1);
        if(g_world->getDispatcher()->getNumManifolds()){
            qDebug() << "************" << g_world->getDispatcher()->getNumManifolds();
            selfCollision = true;
        }else{
            selfCollision = false;
        }
        g_world->clearForces();
    }

    oldMat=joint7->getWorldMatrices().at(0);
*/
    //check self collision
    if(checkCollision(model0, model7) || checkCollision(model1, model7) || checkCollision(model2, model7) || \
       checkCollision(model3, model7) || checkCollision(model4, model7) || checkCollision(model5, model7) || \
       checkCollision(model6, model7) || checkCollision(model0, model6) || checkCollision(model1, model6)){
        selfCollision = true;
    }else{
        selfCollision = false;
    }

    //check virtual wall collision
    if(virtualWallSetted) touchVirtualWall = checkWallCollision();
    else touchVirtualWall = false;
}

osg::Group* robot::createShapes()
{
    hints->setDetailRatio(0.5f);
    group = new osg::Group();

    //read the serial port number from init file
    QFile readModel(fileSavePath + "\\config.init");
    if (readModel.open(QIODevice::ReadOnly | QIODevice::Text)){
        while (!readModel.atEnd()){
            QString initCom = readModel.readLine();
            QStringList keywordPieces = initCom.split(':');
            if(keywordPieces.value(0).trimmed() == "MODEL" && keywordPieces.value(1).trimmed() == "3"){
                robotModel5Kg = false;
            }
        }
    }
    readModel.close();

    group->addChild(jointBase);
    joint1 = buildJoint1(jointBase);
    joint2 = buildJoint2(joint1);
    joint3 = buildJoint3(joint2);
    joint4 = buildJoint4(joint3);
    joint5 = buildJoint5(joint4);
    joint6 = buildJoint6(joint5);
    joint7 = buildJoint7(joint6);
    joint7->addChild(buildEndEffector());

    createBaseAxis();
    //createSphere(jointBase);
    //createSphere(joint1);
    //createSphere(joint2);
    //createSphere(joint3);
    //createSphere(joint4);
    //createSphere(joint5);
    //createSphere(joint6);
    //createSphere(joint7);
    //createTool(joint7);


    joint1BaseMatrix=joint1->getMatrix();
    joint2BaseMatrix=joint2->getMatrix();
    joint3BaseMatrix=joint3->getMatrix();
    joint4BaseMatrix=joint4->getMatrix();
    joint5BaseMatrix=joint5->getMatrix();
    joint6BaseMatrix=joint6->getMatrix();
    joint7BaseMatrix=joint7->getMatrix();


    osg::HeightField* grid = new osg::HeightField;
    grid->allocate(7, 7);
    grid->setXInterval(500.0f);
    grid->setYInterval(500.0f);

    for (unsigned int r = 0; r < 7; ++r)
    {
        for (unsigned int c = 0; c < 7; ++c)
        {
            grid->setHeight(c, r, 0);
        }
    }
    osg::Geode* geode = new osg::Geode();
    geode->addDrawable(new osg::ShapeDrawable(grid));

    osg::PolygonMode* polymode = new osg::PolygonMode;
    polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

    osg::StateSet* stateset = new osg::StateSet;
    stateset->setAttributeAndModes(polymode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
    geode->setStateSet(stateset);

    osg::MatrixTransform* transform_ground = new osg::MatrixTransform();
    osg::Matrix xyTrans = osg::Matrix::translate(-1500.0f,-1500.0f,0.0f);
    //osg::Matrix xRot = osg::Matrix::rotate(osg::PI, 1.0, 0.0, 0.0);
    transform_ground->setMatrix(xyTrans);
    transform_ground->addChild(geode);

    osg::ref_ptr<osg::Material> mat = new osg::Material;
    mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0, 0.0, 0.5));
    osg::StateSet* stateOfCollidedPlane = geode->getOrCreateStateSet();
    stateOfCollidedPlane->setAttribute(mat.get(), osg::StateAttribute::ON);

    group->addChild(transform_ground);


    /***************************bullet***************************/
    /*
    osg::Matrix matj = jointBase->getMatrix();
    g_jointRead(osgMeshes0, matj, -1);
    osg::MatrixTransform* m_joints[7] = {joint1,joint2,joint3,joint4,joint5,joint6,joint7};
    std::string osgMeshesNames[7] = {osgMeshes1,osgMeshes2,osgMeshes3,osgMeshes4,osgMeshes5,osgMeshes6,osgMeshes7};
    g_joints.clear();

    for(int i=0;i<7;i++)
    {
        matj = matj * m_joints[i]->getMatrix();
        g_joints.push_back(g_jointRead(osgMeshesNames[i],matj, i));
    }

    oldMat=matj;
    */
    /**********************************************************/

    return group;
}


osg::MatrixTransform* robot::createWall(float length, float rotation, int kind)
{
    osg::MatrixTransform* wallBase = new osg::MatrixTransform();
    group->addChild(wallBase);

    osg::Box* wallSize;
    //kind 1 2: horizon wall, kind 3 4 5: vertical wall
    if(kind <= 2) wallSize = new osg::Box(osg::Vec3(0,0,length), 1500, 1500, 1);
    if(kind >= 3) wallSize = new osg::Box(osg::Vec3(0,-length,1287/2), 1500, 1, 1500);

    osg::ShapeDrawable *centerShape = new osg::ShapeDrawable(wallSize, hints);
    //centerShape->setColor(osg::Vec4(0.53725, 0.59412, 0.74902, 0.3));

    //make transparent
    osg::StateSet* state2 = centerShape->getOrCreateStateSet(); //Creating material
    osg::ref_ptr<osg::Material> mat2 = new osg::Material;
    mat2->setAlpha(osg::Material::FRONT_AND_BACK, 0.3); //Making alpha channel
    state2->setAttributeAndModes( mat2.get() ,
        osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    osg::BlendFunc* bf = new                        //Blending
        osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,
        osg::BlendFunc::ONE_MINUS_DST_COLOR );
    state2->setAttributeAndModes(bf);


    osg::Geode *Geodecenter = new osg::Geode;
    Geodecenter->addDrawable(centerShape);
    wallBase->addChild(Geodecenter);


    //rotation
    osg::Matrix zTrans = osg::Matrix::translate(0, 0, 0);
    osg::Matrix xRot = osg::Matrix::rotate(0, 1.0, 0.0, 0.0);//
    osg::Matrix yRot = osg::Matrix::rotate(0.0, 0.0, 1.0, 0.0);
    osg::Matrix zRot = osg::Matrix::rotate(osg::PI*rotation/180, 0.0, 0.0, 1.0);
    wallBase->setMatrix(xRot*yRot*zRot*zTrans);

    return wallBase;
}

void robot::buildWalls(bool visible, double length, double rotation, int kind)
{
    if(visible){
        switch (kind) {
            case 1:
                horizonWall1 = createWall(length, rotation, kind);
                horizonWall1pos.len = length;
                horizonWall1pos.enable=true;
                break;
            case 2:
                horizonWall2 = createWall(length, rotation, kind);
                horizonWall2pos.len = length;
                horizonWall2pos.enable=true;
                break;
            case 3:
                verticalWall1 = createWall(length, rotation, kind);
                verticalWall1pos.len = length;
                verticalWall1pos.rot = rotation;
                verticalWall1pos.enable=true;
                break;
            case 4:
                verticalWall2 = createWall(length, rotation, kind);
                verticalWall2pos.len = length;
                verticalWall2pos.rot = rotation;
                verticalWall2pos.enable=true;
                break;
            case 5:
                verticalWall3 = createWall(length, rotation, kind);
                verticalWall3pos.len = length;
                verticalWall3pos.rot = rotation;
                verticalWall3pos.enable=true;
                break;
            case 6:
                verticalWall4 = createWall(length, rotation, kind);
                verticalWall4pos.len = length;
                verticalWall4pos.rot = rotation;
                verticalWall4pos.enable=true;
                break;
            default:
                break;
        }

    }else{
        if(horizonWall1 != NULL) group->removeChild(horizonWall1);
        if(horizonWall2 != NULL) group->removeChild(horizonWall2);
        if(verticalWall1 != NULL) group->removeChild(verticalWall1);
        if(verticalWall2 != NULL) group->removeChild(verticalWall2);
        if(verticalWall3 != NULL) group->removeChild(verticalWall3);
        if(verticalWall4 != NULL) group->removeChild(verticalWall4);

        horizonWall1pos.enable=false;
        horizonWall2pos.enable=false;
        verticalWall1pos.enable=false;
        verticalWall2pos.enable=false;
        verticalWall3pos.enable=false;
        verticalWall4pos.enable=false;
    }
}


void robot::createSphere(osg::Transform *previousJoint)
{
    osg::ShapeDrawable *centerShape = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0, 0, 0), 50), hints);
    centerShape->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    osg::Geode *Geodecenter = new osg::Geode;
    Geodecenter->addDrawable(centerShape);
    previousJoint->addChild(Geodecenter);
}

void robot::createTool(osg::Transform *previousJoint, float x, float y, float z)
{
    osg::ShapeDrawable *centerShape = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(x, y, z), 10), hints);
    //osg::ShapeDrawable *centerShape = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(x, y, z), 30, 200), hints);
    centerShape->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    osg::Geode *Geodecenter = new osg::Geode;
    Geodecenter->addDrawable(centerShape);
    //Geodecenter->setNodeMask(0x000000); //invisible
    previousJoint->addChild(Geodecenter);
    //save Geodecenter
    toolNode = Geodecenter;

}

void robot::setTool(bool visible, float x, float y, float z, float rx, float ry, float rz)
{
    if(visible){
        if(joint7 != NULL){
            toolBaseSave = createAxis(joint7,x,y,z,0,0,0);
            useTool = true;
        }
        //joint7->getChild(1)->asGeode()->setNodeMask(4294967295);    //visible
    }
    else{
        if(toolBaseSave != NULL){
            useTool = false;
            group->removeChild(jointBase);

            jointBase = new osg::MatrixTransform();
            group->addChild(jointBase);
            joint1 = buildJoint1(jointBase);
            joint2 = buildJoint2(joint1);
            joint3 = buildJoint3(joint2);
            joint4 = buildJoint4(joint3);
            joint5 = buildJoint5(joint4);
            joint6 = buildJoint6(joint5);
            joint7 = buildJoint7(joint6);
            joint7->addChild(buildEndEffector());
            createBaseAxis();

        }
    }
}


bool robot::checkCollision(model A, model B)
{
    return (A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y)+(A.z-B.z)*(A.z-B.z) <= (A.r+B.r)*(A.r+B.r);
}

double calculateCos(double l, double theta, double x, double y){
    double t = x*cos(theta*3.14/180) + y*sin(theta*3.14/180);
    return (l-t)/sqrt(l*l + x*x + y*y -2*l*t);
}

bool robot::checkWallCollision()
{
    if(horizonWall1pos.enable){
        if(horizonWall1pos.len >= 400){
            //robot should be lower than this wall
            if(model4.z >= horizonWall1pos.len - 100) return true;
            if(model6.z >= horizonWall1pos.len - 100) return true;
            if(model8.z >= horizonWall1pos.len) return true;
        }

        if(horizonWall1pos.len <= 220){
            //robot should be higher than this wall
            if(model4.z <= horizonWall1pos.len + 100) return true;
            if(model6.z <= horizonWall1pos.len + 100) return true;
            if(model8.z <= horizonWall1pos.len) return true;
        }
    }

    if(horizonWall2pos.enable){
        if(horizonWall2pos.len >= 400){
            //robot should be lower than this wall
            if(model4.z >= horizonWall2pos.len - 100) return true;
            if(model6.z >= horizonWall2pos.len - 100) return true;
            if(model8.z >= horizonWall2pos.len) return true;
        }

        if(horizonWall2pos.len <= 0){
            //robot should be higher than this wall
            if(model4.z <= horizonWall2pos.len + 100) return true;
            if(model6.z <= horizonWall2pos.len + 100) return true;
            if(model8.z <= horizonWall2pos.len) return true;
        }
    }

    if(verticalWall1pos.enable){
        //虚拟墙的坐标系与model的坐标系不同
        if(calculateCos(verticalWall1pos.len-100, verticalWall1pos.rot, -model4.y, model4.x) < 0) return true;
        if(calculateCos(verticalWall1pos.len-100, verticalWall1pos.rot, -model6.y, model6.x) < 0) return true;
        if(calculateCos(verticalWall1pos.len, verticalWall1pos.rot, -model8.y, model8.x) < 0) return true;
    }

    if(verticalWall2pos.enable){
        //虚拟墙的坐标系与model的坐标系不同
        if(calculateCos(verticalWall2pos.len-100, verticalWall2pos.rot, -model4.y, model4.x) < 0) return true;
        if(calculateCos(verticalWall2pos.len-100, verticalWall2pos.rot, -model6.y, model6.x) < 0) return true;
        if(calculateCos(verticalWall2pos.len, verticalWall2pos.rot, -model8.y, model8.x) < 0) return true;
    }

    if(verticalWall3pos.enable){
        //虚拟墙的坐标系与model的坐标系不同
        if(calculateCos(verticalWall3pos.len-100, verticalWall3pos.rot, -model4.y, model4.x) < 0) return true;
        if(calculateCos(verticalWall3pos.len-100, verticalWall3pos.rot, -model6.y, model6.x) < 0) return true;
        if(calculateCos(verticalWall3pos.len, verticalWall3pos.rot, -model8.y, model8.x) < 0) return true;
    }

    if(verticalWall4pos.enable){
        //虚拟墙的坐标系与model的坐标系不同
        if(calculateCos(verticalWall4pos.len-100, verticalWall4pos.rot, -model4.y, model4.x) < 0) return true;
        if(calculateCos(verticalWall4pos.len-100, verticalWall4pos.rot, -model6.y, model6.x) < 0) return true;
        if(calculateCos(verticalWall4pos.len, verticalWall4pos.rot, -model8.y, model8.x) < 0) return true;
    }

    return false;
}


void robot::createNode(float x, float y, float z, char* txt)
{
    osg::ShapeDrawable *centerShape = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(x, y, z), 10), hints);
    centerShape->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    osg::Geode *Geodecenter = new osg::Geode;
    Geodecenter->addDrawable(centerShape);

    osg::Vec3 pos(x-50, y, z+20);
    osgText::Text *text = new osgText::Text;
    text->setText(txt);
    text->setPosition(pos);
    text->setFontResolution(4294967295,4294967295);
    text->setCharacterSize(30);
    osg::Quat rotation;
    rotation.makeRotate(osg::PI/2, 1, 0, 0);
    text->setRotation(rotation);
    text->setDrawMode(osgText::TextBase::DrawModeMask::FILLEDBOUNDINGBOX | osgText::TextBase::DrawModeMask::TEXT);
    text->setBoundingBoxColor(osg::Vec4(0.0f, 0.0f, 0.2f, 0.25f));
    text->setBoundingBoxMargin(2.0f);
    Geodecenter->addDrawable(text);

    group->addChild(Geodecenter);
    programNodes.push_back(Geodecenter);
}

void robot::clearNodes()
{
    for(int i = 0; i < programNodes.size(); i++)
        group->removeChild(programNodes.at(i));//remove path node
    programNodes.clear();
}


osg::MatrixTransform* robot::buildJoint1(osg::MatrixTransform* previousJoint)
{
    osg::Node *link;
    if(robotModel5Kg) link = osgDB::readNodeFile(osgMeshes0);
    else link = osgDB::readNodeFile(osgMeshes3Kg0);
    if(link){
        osg::ref_ptr<osg::Material> mat = new osg::Material;
        mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.53725, 0.59412, 0.74902, 1));
        osg::StateSet* stateOfCollidedPlane = link->getOrCreateStateSet();
        stateOfCollidedPlane->setAttribute(mat.get(), osg::StateAttribute::ON);
        previousJoint->addChild(link);  //将模型显示在当前位置
    }

    osg::MatrixTransform *nextTransform = new osg::MatrixTransform();
    previousJoint->addChild(nextTransform);

    if(robotModel5Kg){
        osg::Matrix zTrans = osg::Matrix::translate(0, 0, 172.0);       //下一关节相对于基座标的平移（0，0，172）
        osg::Matrix xRot0 = osg::Matrix::rotate(0.0, 1.0, 0.0, 0.0);    //下一关节相对于x轴的旋转（弧度制）
        osg::Matrix yRot0 = osg::Matrix::rotate(0.0, 0.0, 1.0, 0.0);    //下一关节相对于y轴的旋转（弧度制）
        osg::Matrix zRot0 = osg::Matrix::rotate(0.0, 0.0, 0.0, 1.0);    //下一关节相对于z轴的旋转（弧度制）
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }else{
        osg::Matrix zTrans = osg::Matrix::translate(0, 0, 155.0);       //下一关节相对于基座标的平移（0，0，172）
        osg::Matrix xRot0 = osg::Matrix::rotate(0.0, 1.0, 0.0, 0.0);    //下一关节相对于x轴的旋转（弧度制）
        osg::Matrix yRot0 = osg::Matrix::rotate(0.0, 0.0, 1.0, 0.0);    //下一关节相对于y轴的旋转（弧度制）
        osg::Matrix zRot0 = osg::Matrix::rotate(-osg::PI/2, 0.0, 0.0, 1.0);    //下一关节相对于z轴的旋转（弧度制）
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }

    return nextTransform;
}

osg::MatrixTransform* robot::buildJoint2(osg::MatrixTransform* previousJoint)
{
    osg::MatrixTransform *nextTransform = new osg::MatrixTransform();
    osg::Node *link;
    if(robotModel5Kg) link = osgDB::readNodeFile(osgMeshes1);
    else link = osgDB::readNodeFile(osgMeshes3Kg1);

    if(link){
        osg::ref_ptr<osg::Material> mat = new osg::Material;
        mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.13725, 0.49412, 0.74902, 1));
        osg::StateSet* stateOfCollidedPlane = link->getOrCreateStateSet();
        stateOfCollidedPlane->setAttribute(mat.get(), osg::StateAttribute::ON);
        previousJoint->addChild(link);
    }

    previousJoint->addChild(nextTransform);

    if(robotModel5Kg){
        osg::Matrix zTrans = osg::Matrix::translate(59, 0.0, 138);
        osg::Matrix xRot0 = osg::Matrix::rotate(0, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(osg::PI/2, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(0.0, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }else{
        osg::Matrix zTrans = osg::Matrix::translate(0, 93, 145);
        osg::Matrix xRot0 = osg::Matrix::rotate(osg::PI/2, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(0, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(osg::PI, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }

    return nextTransform;
}

osg::MatrixTransform* robot::buildJoint3(osg::MatrixTransform* previousJoint)
{
    osg::MatrixTransform *nextTransform = new osg::MatrixTransform();
    osg::Node *link;
    if(robotModel5Kg) link = osgDB::readNodeFile(osgMeshes2);
    else link = osgDB::readNodeFile(osgMeshes3Kg2);

    if(link){
        osg::ref_ptr<osg::Material> mat = new osg::Material;
        mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.53725, 0.59412, 0.74902, 1));
        osg::StateSet* stateOfCollidedPlane = link->getOrCreateStateSet();
        stateOfCollidedPlane->setAttribute(mat.get(), osg::StateAttribute::ON);
        previousJoint->addChild(link);
    }

    previousJoint->addChild(nextTransform);

    if(robotModel5Kg){
        osg::Matrix zTrans = osg::Matrix::translate(-269.5, 0.0, -59);
        osg::Matrix xRot0 = osg::Matrix::rotate(0.0, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(-osg::PI/2, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(0.0, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }else{
        osg::Matrix zTrans = osg::Matrix::translate(0, 153, -93);
        osg::Matrix xRot0 = osg::Matrix::rotate(-osg::PI/2, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(osg::PI, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(0, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }

    return nextTransform;
}

osg::MatrixTransform* robot::buildJoint4(osg::MatrixTransform* previousJoint)
{
    osg::MatrixTransform *nextTransform = new osg::MatrixTransform();
    osg::Node *link;
    if(robotModel5Kg) link = osgDB::readNodeFile(osgMeshes3);
    else link = osgDB::readNodeFile(osgMeshes3Kg3);

    if(link){
        osg::ref_ptr<osg::Material> mat = new osg::Material;
        mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.13725, 0.49412, 0.74902, 1));
        osg::StateSet* stateOfCollidedPlane = link->getOrCreateStateSet();
        stateOfCollidedPlane->setAttribute(mat.get(), osg::StateAttribute::ON);
        previousJoint->addChild(link);
    }

    previousJoint->addChild(nextTransform);

    if(robotModel5Kg){
        osg::Matrix zTrans = osg::Matrix::translate(60.5, 0.0, 130.5);
        osg::Matrix xRot0 = osg::Matrix::rotate(0.0, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(osg::PI/2, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(0.0, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }else{
        osg::Matrix zTrans = osg::Matrix::translate(0, 90, 147);
        osg::Matrix xRot0 = osg::Matrix::rotate(-osg::PI/2, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(osg::PI, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(0.0, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }

    return nextTransform;
}

osg::MatrixTransform* robot::buildJoint5(osg::MatrixTransform* previousJoint)
{
    osg::MatrixTransform *nextTransform = new osg::MatrixTransform();
    osg::Node *link;
    if(robotModel5Kg) link = osgDB::readNodeFile(osgMeshes4);
    else link = osgDB::readNodeFile(osgMeshes3Kg4);

    if(link){
        osg::ref_ptr<osg::Material> mat = new osg::Material;
        mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.53725, 0.59412, 0.74902, 1));
        osg::StateSet* stateOfCollidedPlane = link->getOrCreateStateSet();
        stateOfCollidedPlane->setAttribute(mat.get(), osg::StateAttribute::ON);
        previousJoint->addChild(link);
    }

    previousJoint->addChild(nextTransform);

    if(robotModel5Kg){
        osg::Matrix zTrans = osg::Matrix::translate(-272.5, 0.0, -60.5);
        osg::Matrix xRot0 = osg::Matrix::rotate(0.0, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(-osg::PI/2, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(osg::PI, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(zRot0*xRot0*yRot0*zTrans);
    }else{
        osg::Matrix zTrans = osg::Matrix::translate(0, 136, -90);
        osg::Matrix xRot0 = osg::Matrix::rotate(-osg::PI/2, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(0, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(0, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(zRot0*xRot0*yRot0*zTrans);
    }

    return nextTransform;


}

osg::MatrixTransform* robot::buildJoint6(osg::MatrixTransform* previousJoint)
{
    osg::MatrixTransform *nextTransform = new osg::MatrixTransform();
    osg::Node *link;
    if(robotModel5Kg) link = osgDB::readNodeFile(osgMeshes5);
    else link = osgDB::readNodeFile(osgMeshes3Kg5);

    if(link){
        osg::ref_ptr<osg::Material> mat = new osg::Material;
        mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.13725, 0.49412, 0.74902, 1));
        osg::StateSet* stateOfCollidedPlane = link->getOrCreateStateSet();
        stateOfCollidedPlane->setAttribute(mat.get(), osg::StateAttribute::ON);
        previousJoint->addChild(link);
    }

    previousJoint->addChild(nextTransform);

    if(robotModel5Kg){
        osg::Matrix zTrans = osg::Matrix::translate(-89.8, 0, 127.5);
        osg::Matrix xRot0 = osg::Matrix::rotate(0.0, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(osg::PI/2, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(0.0, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }else{
        osg::Matrix zTrans = osg::Matrix::translate(0, -100, 164);
        osg::Matrix xRot0 = osg::Matrix::rotate(-osg::PI/2, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(0, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(osg::PI, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }
    return nextTransform;
}

osg::MatrixTransform* robot::buildJoint7(osg::MatrixTransform* previousJoint)
{
    osg::MatrixTransform *nextTransform = new osg::MatrixTransform();
    osg::Node *link;
    if(robotModel5Kg) link = osgDB::readNodeFile(osgMeshes6);
    else link = osgDB::readNodeFile(osgMeshes3Kg6);

    if(link){
        osg::ref_ptr<osg::Material> mat = new osg::Material;
        mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.53725, 0.59412, 0.74902, 1));
        osg::StateSet* stateOfCollidedPlane = link->getOrCreateStateSet();
        stateOfCollidedPlane->setAttribute(mat.get(), osg::StateAttribute::ON);
        previousJoint->addChild(link);
    }

    previousJoint->addChild(nextTransform);

    if(robotModel5Kg){
        osg::Matrix zTrans = osg::Matrix::translate(-175, 0.0, 89.8);
        osg::Matrix xRot0 = osg::Matrix::rotate(0.0, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(-osg::PI/2, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(0.0, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }else{
        osg::Matrix zTrans = osg::Matrix::translate(0, -95, -140);
        osg::Matrix xRot0 = osg::Matrix::rotate(osg::PI/2, 1.0, 0.0, 0.0);
        osg::Matrix yRot0 = osg::Matrix::rotate(0, 0.0, 1.0, 0.0);
        osg::Matrix zRot0 = osg::Matrix::rotate(0.0, 0.0, 0.0, 1.0);
        nextTransform->setMatrix(xRot0*yRot0*zRot0*zTrans);
    }
    return nextTransform;
}

osg::MatrixTransform* robot::buildEndEffector()
{
    osg::MatrixTransform* EndEffector = new osg::MatrixTransform();

    osg::Node *link;
    if(robotModel5Kg) link = osgDB::readNodeFile(osgMeshes7);
    else link = osgDB::readNodeFile(osgMeshes3Kg7);
    if(link){
        osg::ref_ptr<osg::Material> mat = new osg::Material;
        mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.75, 0.0, 0.0, 1));
        osg::StateSet* stateOfCollidedPlane = link->getOrCreateStateSet();
        stateOfCollidedPlane->setAttribute(mat.get(), osg::StateAttribute::ON);
        EndEffector->addChild(link);
    }

    return EndEffector;
}
/*
btCollisionObject *robot::g_jointRead(std::string fname, osg::Matrix matj, int num)
{

    GLInstanceGraphicsShape* glmesh = LoadMeshFromSTL(fname.c_str());
    btTriangleMesh*  meshInterface = new btTriangleMesh();
qDebug() << "num:" << num << "********numindices" << glmesh->m_numIndices;
    for (int i=0;i<glmesh->m_numIndices/3;i++)
    {
        float* v0 = glmesh->m_vertices->at(glmesh->m_indices->at(i*3)).xyzw;
        float* v1 = glmesh->m_vertices->at(glmesh->m_indices->at(i*3+1)).xyzw;
        float* v2 = glmesh->m_vertices->at(glmesh->m_indices->at(i*3+2)).xyzw;
        meshInterface->addTriangle(btVector3(v0[0],v0[1],v0[2]),
                                    btVector3(v1[0],v1[1],v1[2]),
                                btVector3(v2[0],v2[1],v2[2]));

    }

    btGImpactMeshShape* jointmesh = new btGImpactMeshShape(meshInterface);
    jointmesh->setMargin(-1.f);
    jointmesh->updateBound();

    //register algorithm
    btCollisionDispatcher * dispatcher = static_cast<btCollisionDispatcher *>(g_world ->getDispatcher());
    btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

    btTransform startTransform;
    startTransform.setIdentity();
    btVector3 pos(0.f,0.f,0.f);
    btQuaternion rot(0.f,0.f,0.f,0.f);

    for(int i=0;i<3;i++)
    {
        pos[i] = matj.operator()(3,i);
    }
    startTransform.setOrigin(pos);

    for(int i = 0; i <= 3; i++){
        qDebug()<<matj.operator()(0,i) << "," << matj.operator()(1,i) << "," << matj.operator()(2,i) << "," << matj.operator()(3,i);
    }
    char a = (char)num;
    createNode(matj.operator()(3,0), matj.operator()(3,1),matj.operator()(3,2),&a);
    qDebug()<<"**********";

    rot = btQuaternion(matj.getRotate().x(),matj.getRotate().y(),matj.getRotate().z(),matj.getRotate().w());
    startTransform.setRotation(rot);

    btCollisionObject* jointbody=new btCollisionObject;

    jointbody->setCollisionShape(jointmesh);
    jointbody->setCollisionFlags(jointbody->getCollisionFlags()|btCollisionObject::CF_KINEMATIC_OBJECT);
    jointbody->setActivationState(DISABLE_DEACTIVATION);
    //g_world->addRigidBody(jointbody);
    //if(num == 6)
    g_world->addCollisionObject(jointbody);

    return jointbody;
}
*/


osg::MatrixTransform* robot::createAxis(osg::Transform* previousJoint, float x, float y, float z, float rx, float ry, float rz)
{
    osg::MatrixTransform* toolBase = new osg::MatrixTransform();
    previousJoint->addChild(toolBase);
    osg::Matrix trans = osg::Matrix::translate(x, y, z);
    osg::Matrix yRot = osg::Matrix::rotate(-ry, 1.0, 0.0, 0.0);
    osg::Matrix xRot = osg::Matrix::rotate(rx, 0.0, 1.0, 0.0);
    osg::Matrix zRot = osg::Matrix::rotate(rz, 0.0, 0.0, 1.0);
    toolBase->setMatrix(yRot*xRot*zRot*trans);

    double height = 70;
    double radius = 3;

    //z
    osg::MatrixTransform* zmt = new osg::MatrixTransform();
    toolBase->addChild(zmt);
    osg::ShapeDrawable *zShape = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0,0, height/2), radius, height), hints);
    osg::ShapeDrawable *zCone = new osg::ShapeDrawable(new osg::Cone(osg::Vec3(0, 0, 0), radius * 3, 2.0 * 10), hints);
    osg::MatrixTransform* zmtCone = new osg::MatrixTransform();
    osg::Geode *zgCone = new osg::Geode;
    zmtCone->setMatrix(osg::Matrix::translate(0.0, 0.0, height));
    toolBase->addChild(zmtCone);
    zShape->setColor(osg::Vec4(0, 0.6484375, 1, 1));
    zCone->setColor(osg::Vec4(0, 0.6484375, 1, 1));
    osg::Geode *nodez = new osg::Geode;
    nodez->addDrawable(zShape);
    zgCone->addDrawable(zCone);
    zmtCone->addChild(zgCone);
    zmt->addChild(nodez);

    //y
    osg::MatrixTransform* mt = new osg::MatrixTransform();
    toolBase->addChild(mt);
    osg::Matrix xMatrix = osg::Matrix::rotate(-osg::PI_2, 0.0, 1.0, 0.0);
    mt->setMatrix(xMatrix);

    //y text
    osg::Vec3 pos(0, 0, 230);
    osgText::Text *text = new osgText::Text;
    text->setText("Y");
    text->setPosition(pos);
    text->setFontResolution(4294967295,4294967295);
    text->setCharacterSize(40);
    osg::Quat rotation;
    rotation.makeRotate(-osg::PI/2, 0, 1, 0);
    osg::Quat rotation2;
    rotation2.makeRotate(-osg::PI/2, 0, 0, 1);
    text->setRotation(rotation*rotation2);
    text->setDrawMode(osgText::TextBase::DrawModeMask::FILLEDBOUNDINGBOX | osgText::TextBase::DrawModeMask::TEXT);
    text->setBoundingBoxColor(osg::Vec4(0.0f, 0.0f, 0.2f, 0.25f));
    text->setBoundingBoxMargin(2.0f);

    osg::ShapeDrawable *xShape = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0, 0, height/2), radius, height), hints);
    xShape->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    osg::Geode *nodex = new osg::Geode;
    nodex->addDrawable(xShape);
    //nodex->addDrawable(text);
    mt->addChild(nodex);

    //y arrow
    osg::MatrixTransform* yArrowBase = new osg::MatrixTransform();
    toolBase->addChild(yArrowBase);
    yArrowBase->setMatrix(osg::Matrix::translate(-height, 0, 0));
    osg::MatrixTransform* xmtCone = new osg::MatrixTransform();
    yArrowBase->addChild(xmtCone);
    xmtCone->setMatrix(xMatrix);
    osg::ShapeDrawable *xCone = new osg::ShapeDrawable(new osg::Cone(osg::Vec3(0, 0, 0), radius * 3, 20), hints);
    xCone->setColor(osg::Vec4(0, 1, 0, 1));
    osg::Geode *xgCone = new osg::Geode;
    xgCone->addDrawable(xCone);
    xmtCone->addChild(xgCone);

    //x
    osg::MatrixTransform *yMt = new osg::MatrixTransform();
    toolBase->addChild(yMt);
    osg::Matrix yMatrix = osg::Matrix::rotate(-osg::PI_2, 1.0, 0.0, 0.0);
    yMt->setMatrix(yMatrix);
    osg::ShapeDrawable *yShape = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0, 0, height/2), radius, height), hints);
    yShape->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    osg::Geode *nodey = new osg::Geode;
    nodey->addDrawable(yShape);
    yMt->addChild(nodey);

    //x arrow
    osg::MatrixTransform* xArrowBase = new osg::MatrixTransform();
    toolBase->addChild(xArrowBase);
    xArrowBase->setMatrix(osg::Matrix::translate(0, height, 0));
    osg::MatrixTransform* ymtCone = new osg::MatrixTransform();
    xArrowBase->addChild(ymtCone);
    ymtCone->setMatrix(yMatrix);
    osg::ShapeDrawable *yCone = new osg::ShapeDrawable(new osg::Cone(osg::Vec3(0, 0, 0), radius * 3, 20), hints);
    yCone->setColor(osg::Vec4(1, 0, 0, 1));
    osg::Geode *ygCone = new osg::Geode;
    ygCone->addDrawable(yCone);
    ymtCone->addChild(ygCone);

    return toolBase;
}

void robot::createBaseAxis()
{
    double height = 200;
    double radius = 5;

    //z
    //osg::MatrixTransform* zmt = new osg::MatrixTransform();
    //jointBase->addChild(zmt);
    //osg::ShapeDrawable *zShape = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0, 0, height/2), radius, height), hints);
    //osg::ShapeDrawable *zCone = new osg::ShapeDrawable(new osg::Cone(osg::Vec3(0, 0, 0), radius * 2, 2.0 * 10), hints);
    //osg::MatrixTransform* zmtCone = new osg::MatrixTransform();
    //osg::Geode *zgCone = new osg::Geode;
    //zmtCone->setMatrix(osg::Matrix::translate(0.0, 0.0, height));
    //jointBase->addChild(zmtCone);
    //zShape->setColor(osg::Vec4(1, 1, 1, 1));
    //zCone->setColor(osg::Vec4(1, 1, 1, 1));
    //osg::Geode *nodez = new osg::Geode;
    //nodez->addDrawable(zShape);
    //zgCone->addDrawable(zCone);
    //zmtCone->addChild(zgCone);
    //zmt->addChild(nodez);

    //y
    osg::Matrix xMatrix = osg::Matrix::rotate(osg::PI_2, 0.0, 1.0, 0.0);

    //y text
    osg::Vec3 pos(0, 0, 230);
    osgText::Text *text = new osgText::Text;
    text->setText("Y");
    text->setPosition(pos);
    text->setFontResolution(4294967295,4294967295);
    text->setCharacterSize(60);
    osg::Quat rotation;
    rotation.makeRotate(-osg::PI/2, 0, 1, 0);
    osg::Quat rotation2;
    rotation2.makeRotate(osg::PI/2, 0, 0, 1);
    text->setRotation(rotation*rotation2);
    text->setDrawMode(osgText::TextBase::DrawModeMask::FILLEDBOUNDINGBOX | osgText::TextBase::DrawModeMask::TEXT);
    text->setBoundingBoxColor(osg::Vec4(0.0f, 0.0f, 0.2f, 0.25f));
    text->setBoundingBoxMargin(2.0f);


    //y cylinder
    osg::MatrixTransform* mt = new osg::MatrixTransform();
    jointBase->addChild(mt);
    mt->setMatrix(xMatrix);
    osg::ShapeDrawable *xShape = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0, 0, height/2), radius, height), hints);
    xShape->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    osg::Geode *nodex = new osg::Geode;
    nodex->addDrawable(xShape);
    nodex->addDrawable(text);
    mt->addChild(nodex);

    //y arrow
    osg::MatrixTransform* yArrowBase = new osg::MatrixTransform();
    jointBase->addChild(yArrowBase);
    yArrowBase->setMatrix(osg::Matrix::translate(height, 0, 0));
    osg::MatrixTransform* xmtCone = new osg::MatrixTransform();
    yArrowBase->addChild(xmtCone);
    xmtCone->setMatrix(xMatrix);
    osg::ShapeDrawable *xCone = new osg::ShapeDrawable(new osg::Cone(osg::Vec3(0, 0, 0), radius * 3, 30), hints);
    xCone->setColor(osg::Vec4(0, 1, 0, 1));
    osg::Geode *xgCone = new osg::Geode;
    xgCone->addDrawable(xCone);
    xmtCone->addChild(xgCone);



    //x

    //x text
    osg::Vec3 posx(20, 0, 210);
    osgText::Text *textx = new osgText::Text;
    textx->setText("X");
    textx->setPosition(posx);
    textx->setFontResolution(4294967295,4294967295);
    textx->setCharacterSize(60);
    rotation.makeRotate(osg::PI/2, 0, 0, 0);
    textx->setRotation(rotation);
    textx->setDrawMode(osgText::TextBase::DrawModeMask::FILLEDBOUNDINGBOX | osgText::TextBase::DrawModeMask::TEXT);
    textx->setBoundingBoxColor(osg::Vec4(0.0f, 0.0f, 0.2f, 0.25f));
    textx->setBoundingBoxMargin(2.0f);

    //x cylinder
    osg::MatrixTransform *yMt = new osg::MatrixTransform();
    jointBase->addChild(yMt);
    osg::Matrix yMatrix = osg::Matrix::rotate(osg::PI_2, 1.0, 0.0, 0.0);
    yMt->setMatrix(yMatrix);
    osg::ShapeDrawable *yShape = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0, 0, height/2), radius, height), hints);
    yShape->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    osg::Geode *nodey = new osg::Geode;
    nodey->addDrawable(yShape);
    nodey->addDrawable(textx);
    yMt->addChild(nodey);

    //x arrow
    osg::MatrixTransform* xArrowBase = new osg::MatrixTransform();
    jointBase->addChild(xArrowBase);
    xArrowBase->setMatrix(osg::Matrix::translate(0, -height, 0));
    osg::MatrixTransform* ymtCone = new osg::MatrixTransform();
    xArrowBase->addChild(ymtCone);
    ymtCone->setMatrix(yMatrix);
    osg::ShapeDrawable *yCone = new osg::ShapeDrawable(new osg::Cone(osg::Vec3(0, 0, 0), radius * 3, 30), hints);
    yCone->setColor(osg::Vec4(1, 0, 0, 1));
    osg::Geode *ygCone = new osg::Geode;
    ygCone->addDrawable(yCone);
    ymtCone->addChild(ygCone);
}
