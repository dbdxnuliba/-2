#-------------------------------------------------
#
# Project created by QtCreator 2015-05-29T16:29:49
#
#-------------------------------------------------

QT       += core gui opengl network serialport xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = robotUI
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    dataupdate.cpp \
    program/runprogram.cpp \
    3dModel/robot.cpp \
    3dModel/viewerwidget.cpp \
    UI/addnodewindow.cpp \
    UI/numberboard.cpp \
    UI/keyboard.cpp \
    ADS/ads.cpp \
    Log/logfile.cpp \
    TCP/tcpserver.cpp \
    TCP/socketthread.cpp \
    threads/movetothread.cpp \
    threads/loadingthread.cpp \
    UI/warningwindow.cpp \
    Robot_Script/7dof.tab.c \
    Robot_Script/absyn.c \
    Robot_Script/datatype.c \
    Robot_Script/enventry.c \
    Robot_Script/errmsg.c \
    Robot_Script/expval.c \
    Robot_Script/interpEnv.c \
    Robot_Script/interpProg.c \
    Robot_Script/lex.yy.c \
    Robot_Script/log.c \
    Robot_Script/siasunRSI.c \
    Robot_Script/stack.c \
    Robot_Script/symbol.c \
    Robot_Script/sysutil.c \
    Robot_Script/table.c \
    Robot_Script/tool.c \
    Robot_Script/7doflib/7dof_math.c \
    Robot_Script/7doflib/7dof_robot.c \
    Robot_Script/7doflib/7dof_socket.c \
    camera/calibration.cpp \
    Robot_Script/7doflib/7dof_camera.c \
    Robot_Script/regetable.c \
    threads/autologin.cpp

HEADERS  += mainwindow.h \
    ADS/TcAdsAPI.h \
    ADS/TcAdsDef.h \
    dataupdate.h \
    program/runprogram.h \
    3dModel/robot.h \
    3dModel/viewerwidget.h \
    UI/addnodewindow.h \
    UI/numberboard.h \
    UI/keyboard.h \
    ADS/ads.h \
    Log/logfile.h \
    TCP/tcpserver.h \
    TCP/socketthread.h \
    threads/movetothread.h \
    threads/loadingthread.h \
    UI/warningwindow.h \
    Robot_Script/7dof.tab.h \
    Robot_Script/7dof_media.h \
    Robot_Script/absyn.h \
    Robot_Script/datatype.h \
    Robot_Script/enventry.h \
    Robot_Script/errmsg.h \
    Robot_Script/expval.h \
    Robot_Script/interpEnv.h \
    Robot_Script/interpProg.h \
    Robot_Script/log.h \
    Robot_Script/siasunRSI.h \
    Robot_Script/stack.h \
    Robot_Script/symbol.h \
    Robot_Script/sysutil.h \
    Robot_Script/table.h \
    Robot_Script/tool.h \
    Robot_Script/7doflib/7dof_math.h \
    Robot_Script/7doflib/7dof_robot.h \
    Robot_Script/7doflib/7dof_socket.h \
    camera/calibration.h \
    Robot_Script/7doflib/7dof_camera.h \
    threads/autologin.h \
    3dModel/GLInstanceGraphicsShape.h \
    3dModel/LoadMeshFromSTL.h

FORMS    += mainwindow.ui \
    addnodewindow.ui \
    numberboard.ui \
    keyboard.ui \
    warningwindow.ui


LIBS += -lC:\SiasunRobot\libs\TcAdsDll\TcAdsDll
INCLUDEPATH += C:\SiasunRobot\libs\TcAdsDll


LIBS += -lC:\SiasunRobot\libs\opengl\Qt5WebKitWidgets
LIBS += -lC:\SiasunRobot\libs\opengl\Qt5WebKit


#64bit osg lib
LIBS        += -lC:\SiasunRobot\OSG\lib\OpenThreads
LIBS        += -lC:\SiasunRobot\OSG\lib\osg
LIBS        += -lC:\SiasunRobot\OSG\lib\osgDB
LIBS        += -lC:\SiasunRobot\OSG\lib\osgUtil
LIBS        += -lC:\SiasunRobot\OSG\lib\osgViewer
LIBS        += -lC:\SiasunRobot\OSG\lib\osgGA
LIBS        += -lC:\SiasunRobot\OSG\lib\osgQt
LIBS        += -lC:\SiasunRobot\OSG\lib\osgWidget
LIBS        += -lC:\SiasunRobot\OSG\lib\osgText
INCLUDEPATH +=   C:\SiasunRobot\OSG\include


LIBS += -lC:\SiasunRobot\opencv\opencv_calib3d249
LIBS += -lC:\SiasunRobot\opencv\opencv_contrib249
LIBS += -lC:\SiasunRobot\opencv\opencv_core249
LIBS += -lC:\SiasunRobot\opencv\opencv_features2d249
LIBS += -lC:\SiasunRobot\opencv\opencv_highgui249
LIBS += -lC:\SiasunRobot\opencv\opencv_imgproc249
INCLUDEPATH += C:\SiasunRobot\opencv\include


#LIBS        += -lC:\SiasunRobot\bullet\bin\Bullet2FileLoader_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\Bullet3Common_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\Bullet3Dynamics_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\Bullet3Geometry_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\Bullet3OpenCL_clew_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\BulletCollision_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\BulletDynamics_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\BulletFileLoader_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\BulletInverseDynamics_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\BulletInverseDynamicsUtils_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\BulletSoftBody_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\BulletWorldImporter_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\BussIK_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\ConvexDecomposition_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\enet_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\gtest_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\gwen_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\HACD_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\LinearMath_vs2010_x64_release
#LIBS        += -lC:\SiasunRobot\bullet\bin\OpenGL_Window_vs2010_x64_release
#INCLUDEPATH +=   C:\SiasunRobot\bullet\src

LIBS += -luser32

#LIBS += -lC:\SiasunRobot\ode\lib\ReleaseDoubleDLL\ode_double
#INCLUDEPATH +=   C:\SiasunRobot\ode\include


RESOURCES += \
    images.qrc \

RC_FILE = ico.rc

