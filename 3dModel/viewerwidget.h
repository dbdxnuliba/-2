#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QTimer>
#include <QApplication>
#include <QGridLayout>

#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>
#include <iostream>
#include "robot.h"

/*
 * This class is binded to the move3DWidget widget in ui design.
 * It creats a window embedded with the robot 3D show(created by robot.h)
*/

class ViewerWidget: public QWidget, public osgViewer::CompositeViewer
{
public:
    ViewerWidget(QWidget *parent, osgViewer::ViewerBase::ThreadingModel threadingModel=osgViewer::CompositeViewer::SingleThreaded);
    ~ViewerWidget();

    QWidget* addViewWidget( osgQt::GraphicsWindowQt* gw, osg::Group* scene );
    osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false );

    virtual void paintEvent( QPaintEvent* event );

    robot* rob;
    void ZoomHome();
    void ZoomIn(bool);
    void ZoomOut(bool);
    void MoveLeft(bool);
    void MoveRight(bool);
    void MoveTop(bool);
    void MoveBottom(bool);

    osgViewer::View* view = new osgViewer::View;
    osgGA::TrackballManipulator* tc = new osgGA::TrackballManipulator;

protected:
    bool bZoomHome=false;
    bool bZoomIn=false;
    bool bZoomOut=false;
    bool bMoveLeft=false;
    bool bMoveRight=false;
    bool bMoveTop=false;
    bool bMoveBottom=false;

    QTimer _timer;

};

#endif // VIEWERWIDGET_H
