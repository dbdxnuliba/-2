#include <math.h>
#include "viewerwidget.h"
#include "dataupdate.h"
#include <qdebug.h>

ViewerWidget::ViewerWidget(QWidget *parent, osgViewer::ViewerBase::ThreadingModel threadingModel) : QWidget(parent)
{
    setThreadingModel(threadingModel);

    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);

    if(true){
        rob = new robot;
        QWidget* widget = addViewWidget(createGraphicsWindow(0,0,100,100), rob->createShapes());

        QGridLayout* grid = new QGridLayout;
        grid->addWidget( widget, 0, 0 );
        setLayout( grid );
    }


    //3D canvas refresh time in ms
    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
    _timer.start( 10 );

}

ViewerWidget::~ViewerWidget(){}

QWidget* ViewerWidget::addViewWidget( osgQt::GraphicsWindowQt* gw, osg::Group* scene )
{
    //osgViewer::View* view = new osgViewer::View; created in .h
    addView( view );

    //set the camera of the view
    osg::Camera* camera = view->getCamera();
    camera->setGraphicsContext( gw );

    const osg::GraphicsContext::Traits* traits = gw->getTraits();

    camera->setClearColor( osg::Vec4(0.12109375, 0.1328125, 0.125, 1.0) ); // color/256
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );



    view->setSceneData( scene );
    view->addEventHandler( new osgViewer::StatsHandler );
    //view->setCameraManipulator( new osgGA::TrackballManipulator );
    view->setCameraManipulator(tc);

    osg::Vec3d eye;
    osg::Vec3d center;
    osg::Vec3d up;
    tc->getHomePosition(eye, center, up);
    eye.z()=eye.z()+1000;
    center.z()=center.z() + 600;
    tc->setHomePosition(eye, center, up);
    view->setCameraManipulator(tc);

    return gw->getGLWidget();
}

osgQt::GraphicsWindowQt* ViewerWidget::createGraphicsWindow( int x, int y, int w, int h, const std::string& name, bool windowDecoration )
{
    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = name;
    traits->windowDecoration = windowDecoration;
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = 8;//ds->getMultiSamples();
    traits->samples =8;  //ds->getNumMultiSamples();
    return new osgQt::GraphicsWindowQt(traits.get());
}

void ViewerWidget::paintEvent(QPaintEvent *event)
{
    //paint the robot on canvas
    frame();

    //move 3D robot using buttons
    if(!bZoomHome)
    {
        view->home();
        tc->setDistance(2700); //original size of the robot
        bZoomHome=true;
        event; //no use
    }
    else if(bZoomIn)
        tc->setDistance(view->getFusionDistanceValue()-50);

    else if(bZoomOut)
        tc->setDistance(view->getFusionDistanceValue()+50);

    osg::Matrix t = tc->getMatrix();

    if(bMoveLeft)
        t.setTrans(t.getTrans().x() + 40, t.getTrans().y(), t.getTrans().z());
    else if(bMoveRight)
        t.setTrans(t.getTrans().x() - 40, t.getTrans().y(), t.getTrans().z());
    else if(bMoveTop)
        t.setTrans(t.getTrans().x(), t.getTrans().y(), t.getTrans().z()-40);
    else if(bMoveBottom)
        t.setTrans(t.getTrans().x(), t.getTrans().y(), t.getTrans().z()+40);
    tc->setByMatrix(t);


}

void ViewerWidget::ZoomHome(){bZoomHome=false;}
void ViewerWidget::ZoomIn(bool trig){bZoomIn=trig;}
void ViewerWidget::ZoomOut(bool trig){bZoomOut=trig;}
void ViewerWidget::MoveLeft(bool trig){bMoveLeft=trig;}
void ViewerWidget::MoveRight(bool trig){bMoveRight=trig;}
void ViewerWidget::MoveTop(bool trig){bMoveTop=trig;}
void ViewerWidget::MoveBottom(bool trig){bMoveBottom=trig;}
