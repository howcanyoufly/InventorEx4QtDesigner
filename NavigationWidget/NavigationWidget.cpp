/*========================================================================
Copyright (c) 2024
Unpublished - All rights reserved

==========================================================================
File description:
Reconstruction QGraphicsView
==========================================================================
Date            Name        Description of Change
2024-03-05      FJH         Init
2024-03-28      FJH         #I98JXE bug fix: from zoom
==========================================================================*/
#include "NavigationWidget.h"
#include "NavigationStyle.h"

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QSurfaceFormat>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <vector>
#include <array>

using namespace SIM::Coin3D::Quarter;
using namespace NavQuarterWidget::Tf;

namespace
{
    void ConvertPerspective2Ortho(const SoPerspectiveCamera* in, SoOrthographicCamera* out)
    {
        out->aspectRatio.setValue(in->aspectRatio.getValue());
        out->focalDistance.setValue(in->focalDistance.getValue());
        out->orientation.setValue(in->orientation.getValue());
        out->position.setValue(in->position.getValue());
        out->viewportMapping.setValue(in->viewportMapping.getValue());

        float focaldist = in->focalDistance.getValue();

        out->height = 2.0f * focaldist * (float)tan(in->heightAngle.getValue() / 2.0);
    }

    SoSeparator* CreateAxis(SoMaterial* material, SoTransform* transform, float size)
    {
        SoSeparator* axis = new SoSeparator();

        if (material != NULL)
        {
            axis->addChild(material);
        }

        if (transform != NULL)
        {
            axis->addChild(transform);
        }

        SoCylinder* cylinder = new SoCylinder();
        cylinder->radius = size * 0.015f;
        cylinder->height = size;
        SoCone* cone = new SoCone();
        cone->bottomRadius = size * 0.015f * 3.0f;
        cone->height = size / 4.0f;

        SoTransform* coneTransform = new SoTransform();
        coneTransform->translation.setValue(0.0f, size / 2.0f, 0.0f);

        SoGroup* arrow = new SoGroup();
        arrow->addChild(cylinder);
        arrow->addChild(coneTransform);
        arrow->addChild(cone);

        if (arrow != NULL)
        {
            axis->addChild(arrow);
        }

        return axis;
    }

    SoSeparator* CreateCsys(float size = 100)
    {
        // X axis
        SoMaterial* xMaterial = new SoMaterial();
        xMaterial->diffuseColor.setValue(1.0f, 0.0f, 0.0f);

        SoTransform* xTransform = new SoTransform();
        xTransform->translation.setValue(size / 2.0f, 0.0f, 0.0f);
        xTransform->rotation.setValue(SbVec3f(0.0f, 0.0f, -1.0f), (float)M_PI_2);

        SoSeparator* xAxis = CreateAxis(xMaterial, xTransform, size);

        SoText2* xAxisText = new SoText2();
        xAxisText->string = "X";
        xAxis->addChild(xAxisText);

        // Y axis
        SoMaterial* yMaterial = new SoMaterial();
        yMaterial->diffuseColor.setValue(0.0f, 1.0f, 0.0f);

        SoTransform* yTransform = new SoTransform();
        yTransform->translation.setValue(0.0f, size / 2.0f, 0.0f);

        SoSeparator* yAxis = CreateAxis(yMaterial, yTransform, size);

        SoText2* yAxisText = new SoText2();
        yAxisText->string = "Y";
        yAxis->addChild(yAxisText);

        // Z axis
        SoMaterial* zMaterial = new SoMaterial();
        zMaterial->diffuseColor.setValue(0.0f, 0.0f, 1.0f);

        SoTransform* zTransform = new SoTransform();
        zTransform->translation.setValue(0.0f, 0.0f, size / 2.0f);
        zTransform->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), (float)M_PI_2);

        SoSeparator* zAxis = CreateAxis(zMaterial, zTransform, size);

        SoText2* zAxisText = new SoText2();
        zAxisText->string = "Z";
        zAxis->addChild(zAxisText);

        // Origin point
        SoSphere* sphere = new SoSphere();
        sphere->radius = size * 0.015f * 2.0f;

        // text font
        SoFont* curFont = new SoFont();
        curFont->name.setValue("Times-Roman");
        curFont->size.setValue(18.0f);

        SoSeparator* axes = new SoSeparator();
        axes->addChild(curFont);
        axes->addChild(sphere);
        axes->addChild(xAxis);
        axes->addChild(yAxis);
        axes->addChild(zAxis);

        SoPickStyle* pickStyle = new SoPickStyle();
        pickStyle->style.setValue(SoPickStyle::SHAPE_ON_TOP);
        pickStyle->setOverride(TRUE);
        SoAnnotation* annotation = new SoAnnotation();
        annotation->addChild(pickStyle);

        SoSeparator* frame = new SoSeparator();
        frame->addChild(annotation);

        return frame;
    }

}

SoCamera* searchForCamera(SoNode* root)
{
    SoSearchAction searchAction;
    searchAction.setInterest(SoSearchAction::FIRST);
    searchAction.setType(SoCamera::getClassTypeId());
    searchAction.apply(root);
    if (searchAction.getPath())
    {
        SoNode* node = searchAction.getPath()->getTail();
        if (node && node->isOfType(SoCamera::getClassTypeId()))
        {
            return (SoCamera*)node;
        }
    }

    return nullptr;
}

//Inner class, cannot be called from outside
class NavigationGLWidget : public QOpenGLWidget
{
public:
    NavigationGLWidget(const QSurfaceFormat& format, QWidget* parent = 0, const QOpenGLWidget* shareWidget = 0, Qt::WindowFlags f = Qt::WindowFlags())
        : QOpenGLWidget(parent, f), m_format(format)
    {
        Q_UNUSED(shareWidget);
        QSurfaceFormat surfaceFormat(format);
        surfaceFormat.setSamples(4);
        surfaceFormat.setDepthBufferSize(24);
        surfaceFormat.setStencilBufferSize(8);
        surfaceFormat.setSwapInterval(1);
        surfaceFormat.setVersion(1, 3);

        setFormat(surfaceFormat);
    }

    ~NavigationGLWidget()
    {
    }

    void InitializeGL()
    {
        QOpenGLContext* context = QOpenGLContext::currentContext();
        if (context)
        {
            connect(context, &QOpenGLContext::aboutToBeDestroyed, this, &NavigationGLWidget::AboutToDestroyGLContext, Qt::DirectConnection);
        }

        connect(this, &NavigationGLWidget::resized, this, &NavigationGLWidget::SlotResized);
    }

    void PaintGL()
    {
        NavQuarterWidget::Tf::NavigationWidget* cqw = qobject_cast<NavQuarterWidget::Tf::NavigationWidget*>(parentWidget());
        if (cqw)
        {
            cqw->Redraw();
        }
    }

    void AboutToDestroyGLContext()
    {
#if QT_VERSION >= 0x050900
        // With Qt 5.9 a signal is emitted while the QuarterWidget is being destroyed.
        // At this state its type is a QWidget, not a QuarterWidget any more.
        NavQuarterWidget::Tf::NavigationWidget* cqw = qobject_cast<NavQuarterWidget::Tf::NavigationWidget*>(parent());
        if (!cqw)
        {
            return;
        }
#endif
        QMetaObject::invokeMethod(parent(), "aboutToDestroyGLContext", Qt::DirectConnection, QGenericReturnArgument());
    }

    void SlotResized()
    {
        update(); // fixes flickering on some systems
    }

    QSurfaceFormat m_format;
};

NavQuarterWidget::Tf::NavigationWidget::NavigationWidget(const QSurfaceFormat& format, QWidget* parent, const QOpenGLWidget* shareWidget, Qt::WindowFlags f)
    :QGraphicsView(parent),
    m_scene(NULL),
    m_currentStateMachine(NULL),
    m_navigationModeFile(NULL)
{
    Q_UNUSED(f);
    Constructor(QSurfaceFormat(), shareWidget);

    Init();

    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
}

NavQuarterWidget::Tf::NavigationWidget::NavigationWidget(QWidget* parent, const QOpenGLWidget* sharewidget, Qt::WindowFlags f)
    :QGraphicsView(parent),
    m_scene(NULL),
    m_currentStateMachine(NULL),
    m_navigationModeFile(NULL)
{
    Q_UNUSED(f);
    this->Constructor(QSurfaceFormat(), sharewidget);

    Init();

    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
}

NavQuarterWidget::Tf::NavigationWidget::~NavigationWidget()
{
    this->setSceneGraph(NULL);
    this->setSoRenderManager(NULL);
    this->setSoEventManager(NULL);
}

void NavQuarterWidget::Tf::NavigationWidget::ResetNavigationModeFile(void)
{
    this->SetNavigationModeFile(QUrl());
}

void NavQuarterWidget::Tf::NavigationWidget::SetNavigationModeFile(const QUrl& url /*= QUrl(QString::fromLatin1(DEFAULT_NAVIGATIONFILE))*/)
{
    QString fileName;

    if ("coin" == url.scheme())
    {
        fileName = url.path();
        if ('/' == fileName[0])
        {
            fileName.remove(0, 1);
        }
        fileName = url.scheme() + ':' + fileName;
    }

    QByteArray tmpFileName = fileName.toLocal8Bit();
    ScXMLStateMachine* stateMachine = NULL;

    if (tmpFileName.startsWith("coin:"))
    {
        stateMachine = ScXML::readFile(tmpFileName.data());
    }

    if (stateMachine && stateMachine->isOfType(SoScXMLStateMachine::getClassTypeId()))
    {
        SoScXMLStateMachine* newStateMachine = static_cast<SoScXMLStateMachine*>(stateMachine);
        if (m_currentStateMachine)
        {
            this->RemoveStateMachine(m_currentStateMachine);
            delete m_currentStateMachine;
        }

        this->AddStateMachine(newStateMachine);
        newStateMachine->initialize();
        m_currentStateMachine = newStateMachine;
    }
    else
    {
        if (stateMachine)
        {
            delete stateMachine;
        }
    }
}

const QUrl& NavQuarterWidget::Tf::NavigationWidget::NavigationModeFile(void) const
{
    return m_navigationModeFile;
}

void NavQuarterWidget::Tf::NavigationWidget::AddStateMachine(SoScXMLStateMachine* statemachine)
{
    SoEventManager* eventManager = this->getSoEventManager();
    eventManager->addSoScXMLStateMachine(statemachine);
    statemachine->setSceneGraphRoot(this->getSoRenderManager()->getSceneGraph());
    statemachine->setActiveCamera(this->getSoRenderManager()->getCamera());
}

void NavQuarterWidget::Tf::NavigationWidget::RemoveStateMachine(SoScXMLStateMachine* statemachine)
{
    SoEventManager* eventManager = this->getSoEventManager();
    statemachine->setSceneGraphRoot(NULL);
    statemachine->setActiveCamera(NULL);
    eventManager->removeSoScXMLStateMachine(statemachine);
}

void NavQuarterWidget::Tf::NavigationWidget::ViewAll()
{
    SoCamera* cam = this->getSoRenderManager()->getCamera();
    if (!cam)
    {
        return;
    }
    FitView();
}

void NavQuarterWidget::Tf::NavigationWidget::paintEvent(QPaintEvent* event)
{
    InitializeGL();

    getSoRenderManager()->activate();
    //glMatrixMode(GL_PROJECTION);

    QOpenGLWidget* w = static_cast<QOpenGLWidget*>(this->viewport());
    if (!w->isValid())
    {
        qWarning() << "No valid GL context found!";
        return;
    }

    assert(w->isValid() && "No valid GL context found!");
    w->makeCurrent();

    auto ra = this->getSoRenderManager()->getGLRenderAction();
    this->getSoRenderManager()->render(ra);

    //Start the standard graphics view processing for all widgets and graphic items. As
    //QGraphicsView initaliizes a QPainter which changes the Opengl context in an unpredictable
    //Manner we need to store the context and recreate it after Qt is done.
    //glPushAttrib(GL_MULTISAMPLE_BIT_EXT);
    QGraphicsView::paintEvent(event);
    //glPopAttrib();
}

void NavQuarterWidget::Tf::NavigationWidget::resizeEvent(QResizeEvent* event)
{
    qreal devPixRatio = 1.0;
    int width = static_cast<int>(devPixRatio * event->size().width());
    int height = static_cast<int>(devPixRatio * event->size().height());

    SbViewportRegion vp(width, height);
    getSoRenderManager()->setViewportRegion(vp);
    getSoEventManager()->setViewportRegion(vp);

    QGraphicsView::resizeEvent(event);
    QPainterPath paintPath;
    QRectF rect = QRectF(0, 0, this->width(), this->height());
    paintPath.addRoundedRect(rect, 10, 10, Qt::AbsoluteSize);
    QPolygon polygon = paintPath.toFillPolygon().toPolygon();
    QRegion region(polygon);
    setMask(region);

    QGraphicsView::resizeEvent(event);
}

void NavQuarterWidget::Tf::NavigationWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (NavigationStyle::VM_PANNING == m_navigation->m_currentmode)
    {
        const SbViewportRegion& vp = getSoRenderManager()->getViewportRegion();
        const SbVec2s size(vp.getViewportSizePixels());
        const SbVec2s pos(event->pos().x(), size[1] - event->pos().y() - 1);
        const SbVec2f posn((float)pos[0] / (float)std::max((int)(size[0] - 1), 1),
                           (float)pos[1] / (float)std::max((int)(size[1] - 1), 1));

        m_navigation->DoPan(this->getSoRenderManager()->getCamera(), posn);
        this->update();
    }
    else if (NavigationStyle::VM_DRAGGING == m_navigation->m_currentmode)
    {
        const SbViewportRegion& vp = getSoRenderManager()->getViewportRegion();
        const SbVec2s size(vp.getViewportSizePixels());
        const SbVec2s pos(event->pos().x(), size[1] - event->pos().y() - 1);
        const SbVec2f posn((float)pos[0] / (float)std::max((int)(size[0] - 1), 1),
                           (float)pos[1] / (float)std::max((int)(size[1] - 1), 1));


        m_navigation->DoSpin(this->getSoRenderManager()->getCamera(), pos, SbTime::getTimeOfDay(), posn);
        this->update();
    }

    m_zoomCenter = event->pos();
}

void NavQuarterWidget::Tf::NavigationWidget::mousePressEvent(QMouseEvent* event)
{
    auto m_event = dynamic_cast<QMouseEvent*>(event);
    if (event->button() == Qt::RightButton &&
        event->modifiers() == Qt::NoModifier &&
        !(event->buttons() & Qt::LeftButton) &&
        !(event->buttons() & Qt::MiddleButton))
    {
        SwitchCoordinateSystem(false);
        FitView();
        SwitchCoordinateSystem(true);
        this->update();
        return;
    }
    else if ((event->buttons() & Qt::MiddleButton) && !(QGuiApplication::mouseButtons() & Qt::LeftButton) && !(QGuiApplication::mouseButtons() & Qt::RightButton))
    {
        const SbViewportRegion& vp = getSoRenderManager()->getViewportRegion();
        const SbVec2s size(vp.getViewportSizePixels());
        const SbVec2s pos(event->pos().x(), size[1] - event->pos().y() - 1);
        const SbVec2f posn((float)pos[0] / (float)std::max((int)(size[0] - 1), 1),
                           (float)pos[1] / (float)std::max((int)(size[1] - 1), 1));
        m_navigation->m_lastmouseposition = posn;

        if (event->modifiers() & Qt::ShiftModifier)
        {
            m_navigation->m_currentmode = NavigationStyle::VM_PANNING;
        }
        else if (!(event->buttons() & ~(Qt::MiddleButton)) &&
                 !(event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier)))
        {
            if (NavigationStyle::VM_DRAGGING != m_navigation->m_currentmode)
            {
                m_navigation->SaveCursorPosition(NULL);
            }
            m_navigation->SetViewingMode(NavigationStyle::VM_DRAGGING);
        }
    }
    else
    {
        if (NavigationStyle::VM_IDLE != m_navigation->m_currentmode)
        {
            m_navigation->m_currentmode = NavigationStyle::VM_IDLE;
        }
    }
}

void NavQuarterWidget::Tf::NavigationWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (QGuiApplication::mouseButtons() & Qt::MiddleButton)
    {
        if (!(QGuiApplication::mouseButtons() & Qt::LeftButton) && !(QGuiApplication::mouseButtons() & Qt::RightButton))
        {
            const SbViewportRegion& vp = getSoRenderManager()->getViewportRegion();
            const SbVec2s size(vp.getViewportSizePixels());
            const SbVec2s pos(event->pos().x(), size[1] - event->pos().y() - 1);
            const SbVec2f posn((float)pos[0] / (float)std::max((int)(size[0] - 1), 1),
                               (float)pos[1] / (float)std::max((int)(size[1] - 1), 1));
            m_navigation->m_lastmouseposition = posn;

            if (event->modifiers() == Qt::NoModifier)
            {
                if (NavigationStyle::VM_DRAGGING != m_navigation->m_currentmode)
                {
                    m_navigation->SaveCursorPosition(NULL);
                }
                m_navigation->SetViewingMode(NavigationStyle::VM_DRAGGING);
            }
            else if (event->modifiers() & Qt::ShiftModifier)
            {
                m_navigation->m_currentmode = NavigationStyle::VM_PANNING;
            }
        }
        else
        {
            if (NavigationStyle::VM_IDLE != m_navigation->m_currentmode)
            {
                m_navigation->m_currentmode = NavigationStyle::VM_IDLE;
            }
        }
    }
    else
    {
        if (NavigationStyle::VM_IDLE != m_navigation->m_currentmode)
        {
            m_navigation->m_currentmode = NavigationStyle::VM_IDLE;
        }
    }
}

void NavQuarterWidget::Tf::NavigationWidget::keyReleaseEvent(QKeyEvent* event)
{
    if ((QGuiApplication::mouseButtons() & Qt::MiddleButton))
    {
        const SbViewportRegion& vp = getSoRenderManager()->getViewportRegion();
        const SbVec2s size(vp.getViewportSizePixels());
        const SbVec2s pos(mapFromGlobal(QCursor::pos()).x(), size[1] - mapFromGlobal(QCursor::pos()).y() - 1);
        const SbVec2f posn((float)pos[0] / (float)std::max((int)(size[0] - 1), 1),
                           (float)pos[1] / (float)std::max((int)(size[1] - 1), 1));
        m_navigation->m_lastmouseposition = posn;

        if (event->modifiers() == Qt::NoModifier)
        {
            if (NavigationStyle::VM_DRAGGING != m_navigation->m_currentmode)
            {
                m_navigation->SaveCursorPosition(NULL);
            }
            m_navigation->SetViewingMode(NavigationStyle::VM_DRAGGING);
        }
        else if (event->modifiers() & Qt::ShiftModifier)
        {
            m_navigation->m_currentmode = NavigationStyle::VM_PANNING;
        }
        else
        {
            if (NavigationStyle::VM_IDLE != m_navigation->m_currentmode)
            {
                m_navigation->m_currentmode = NavigationStyle::VM_IDLE;
            }
        }
    }
    else
    {
        if (NavigationStyle::VM_IDLE != m_navigation->m_currentmode)
        {
            m_navigation->m_currentmode = NavigationStyle::VM_IDLE;
        }
    }
}

void NavQuarterWidget::Tf::NavigationWidget::keyPressEvent(QKeyEvent* event)
{
    if (Qt::Key_Shift != event->key())
    {
        if (NavigationStyle::VM_IDLE != m_navigation->m_currentmode)
        {
            m_navigation->m_currentmode = NavigationStyle::VM_IDLE;
        }
    }
    else if (Qt::Key_Shift == event->key() && !(event->modifiers() & ~Qt::ShiftModifier))
    {
        if ((QGuiApplication::mouseButtons() & Qt::MiddleButton) && !(QGuiApplication::mouseButtons() & ~Qt::MiddleButton))
        {
            const SbViewportRegion& vp = getSoRenderManager()->getViewportRegion();
            const SbVec2s size(vp.getViewportSizePixels());
            const SbVec2s pos(mapFromGlobal(QCursor::pos()).x(), size[1] - mapFromGlobal(QCursor::pos()).y() - 1);
            const SbVec2f posn((float)pos[0] / (float)std::max((int)(size[0] - 1), 1),
                               (float)pos[1] / (float)std::max((int)(size[1] - 1), 1));
            m_navigation->m_lastmouseposition = posn;

            m_navigation->m_currentmode = NavigationStyle::VM_PANNING;
        }
    }
    else
    {
        if (NavigationStyle::VM_IDLE != m_navigation->m_currentmode)
        {
            m_navigation->m_currentmode = NavigationStyle::VM_IDLE;
        }
    }

    QGraphicsView::keyPressEvent(event);
}

void NavQuarterWidget::Tf::NavigationWidget::wheelEvent(QWheelEvent* event)
{
    if (this->m_navigation->m_currentmode == NavigationStyle::VM_IDLE)
    {
        const SbViewportRegion& vp = getSoRenderManager()->getViewportRegion();
        const SbVec2s size(vp.getViewportSizePixels());

        //QPoint to SbVec2s
        SbVec2f pos;
        pos[0] = m_zoomCenter.x();
        pos[1] = m_zoomCenter.y();

        const SbVec2f posn((float)pos[0] / (float)std::max((int)(size[0] - 1), 1),
                           1 - (float)pos[1] / (float)std::max((int)(size[1] - 1), 1));
        //handle mouse wheel zoom
        m_navigation->DoZoom(this->getSoRenderManager()->getCamera(), event->angleDelta().y(), posn);

        this->update();
    }
}

void NavQuarterWidget::Tf::NavigationWidget::Constructor(const QSurfaceFormat& format, const QOpenGLWidget* shareWidget)
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    setScene(scene);
    setViewport(new NavigationGLWidget(format, this, shareWidget));
    setFrameStyle(QFrame::NoFrame);
    setAutoFillBackground(false);
    viewport()->setAutoFillBackground(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_soRenderManager = new SoRenderManager;
    m_soEventManager = new SoEventManager;

    //Add light
    SetLights();

    getSoRenderManager()->setAutoClipping(SoRenderManager::VARIABLE_NEAR_PLANE);
    getSoRenderManager()->setRenderCallback(NavigationWidget::Rendercb, this);
    getSoRenderManager()->setBackgroundColor(SbColor4f(0.0f, 0.0f, 0.0f, 0.0f));
    getSoRenderManager()->activate();

    getSoEventManager()->setNavigationState(SoEventManager::MIXED_NAVIGATION);
    this->setMouseTracking(true);

    this->setFocusPolicy(Qt::StrongFocus);
}


void NavQuarterWidget::Tf::NavigationWidget::InitializeGL(void)
{
    this->getSoRenderManager()->reinitialize();
}

void NavQuarterWidget::Tf::NavigationWidget::Init()
{
    m_displayRoot = new SoSeparator();

    m_coordinateSystemSwitch = new SoSwitch();
    m_coordinateSystemSwitch->ref();
    m_displayRoot->addChild(m_coordinateSystemSwitch);

    // wcs coordinate system
    m_coordinateSystemSwitch->addChild(CreateCsys());
    m_coordinateSystemSwitch->whichChild = 0;

    m_navigation = std::make_unique<NavigationStyle>();
    m_navigation->SetViewer(this);
    m_displayRoot->addChild(m_navigation->GetPickPointNode());

    SoBaseColor* col = new SoBaseColor;
    col->rgb = SbColor(1, 1, 0);
    m_displayRoot->addChild(col);

    setSceneGraph(m_displayRoot);

    // Set SoPerspectiveCamera to SoOrthographicCamera
    auto oldCamera = getSoRenderManager()->getCamera();
    if (oldCamera->isOfType(SoPerspectiveCamera::getClassTypeId()))
    {
        SoCamera* newcamera = (SoCamera*)SoOrthographicCamera::getClassTypeId().createInstance();
        ConvertPerspective2Ortho((SoPerspectiveCamera*)oldCamera, (SoOrthographicCamera*)newcamera);
        getSoRenderManager()->setCamera(newcamera);
        getSoEventManager()->setCamera(newcamera);

        //if the superscene has a camera we need to replace it too
        SoSeparator* superscene = (SoSeparator*)getSoRenderManager()->getSceneGraph();
        SoSearchAction sa;
        sa.setInterest(SoSearchAction::FIRST);
        sa.setType(SoCamera::getClassTypeId());
        sa.apply(superscene);

        if (sa.getPath()) {
            SoNode* node = sa.getPath()->getTail();
            SoGroup* parent = (SoGroup*)sa.getPath()->getNodeFromTail(1);

            if (node && node->isOfType(SoCamera::getClassTypeId())) {
                parent->replaceChild(node, newcamera);
            }
        }
    }

    FitView();

    // Open anti-aliasing
    this->getSoRenderManager()->getGLRenderAction()->setSmoothing(true);
    this->getSoRenderManager()->getGLRenderAction()->setNumPasses(4);
    this->getSoRenderManager()->getGLRenderAction()->setTransparencyType(SoGLRenderAction::TransparencyType::SORTED_OBJECT_SORTED_TRIANGLE_BLEND);
}

void NavQuarterWidget::Tf::NavigationWidget::SetCameraOrientation(const SbRotation& rot, SbBool moveToCenter /*= false*/)
{
    SoCamera* cam = getSoRenderManager()->getCamera();
    if (!cam)
    {
        return;
    }

    // Find global coordinates of focal point.
    SbVec3f direction;
    cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
    SbVec3f focal1 = cam->position.getValue() + cam->focalDistance.getValue() * direction;
    SbVec3f focal2 = focal1;
    if (moveToCenter)
    {
        SoGetBoundingBoxAction action(getSoRenderManager()->getViewportRegion());
        action.apply(getSoRenderManager()->getSceneGraph());
        SbBox3f box = action.getBoundingBox();
        if (!box.isEmpty())
        {
            rot.multVec(SbVec3f(0, 0, -1), direction);
            focal2 = box.getCenter();
        }
    }

    cam->orientation.setValue(rot);
    cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
    cam->position = focal2 - cam->focalDistance.getValue() * direction;
}

void NavQuarterWidget::Tf::NavigationWidget::SwitchCoordinateSystem(bool isShow)
{
    m_coordinateSystemSwitch->whichChild = (isShow ? 0 : -1);
}

void NavQuarterWidget::Tf::NavigationWidget::FitView()
{
    SoCamera* cam = this->getSoRenderManager()->getCamera();
    cam->orientation.setValue(SbRotation(SbVec3f(1.0f, 0.0f, 0.0f), M_PI_2) *
                              SbRotation(SbVec3f(0.0f, 0.0f, 1.0f), M_PI / 4.0f) *
                              SbRotation(SbVec3f(1.0f, 1.0f, 0.0f), -M_PI / 6.0f));//rotate 45 around Z-axis and rotate 30 around x-y=0

    SoGetBoundingBoxAction action(this->getSoRenderManager()->getViewportRegion());
    action.apply(getSceneGraph());
    SbBox3f box = action.getBoundingBox();
    float minx = 0.0f, miny = 0.0f, minz = 0.0f, maxx = 0.0f, maxy = 0.0f, maxz = 0.0f;
    box.getBounds(minx, miny, minz, maxx, maxy, maxz);
    bool isViewEmpty = false;
    if ((maxx - minx) < 1.0e-10f && (maxy - miny) < 1.0e-10f && (maxz - minz) < 1.0e-10f)
    {
        isViewEmpty = true;
    }

    // Set the height angle to 45 deg

    if (cam && cam->getTypeId().isDerivedFrom(SoPerspectiveCamera::getClassTypeId()))
    {
        static_cast<SoPerspectiveCamera*>(cam)->heightAngle = (float)(M_PI / 4.0);
    }

    // make sure everything is visible
    if (cam)
    {
        if (isViewEmpty)
        {
            SoCube* cube = new SoCube();
            cube->width = 1000.0;
            cube->height = 1000.0;
            cube->depth = 0.0;

            // fake a scenegraph with the desired bounding size
            SoSeparator* graph = new SoSeparator();
            graph->ref();
            graph->addChild(cube);
            cam->viewAll(graph, this->getSoRenderManager()->getViewportRegion());
            graph->unref();
        }
        else
        {
            cam->viewAll(getSceneGraph(), this->getSoRenderManager()->getViewportRegion());
        }
    }
}

void NavQuarterWidget::Tf::NavigationWidget::AddTestModel()
{
    if (m_displayRoot)
    {
        auto testMeshSeparator = new SoSeparator();

        SoTranslation* translation = new SoTranslation();
        translation->translation.setValue(5.0f, 5.0f, 5.0f);
        testMeshSeparator->addChild(translation);

        testMeshSeparator->addChild(new SoCube());
        if (testMeshSeparator)
        {
            m_displayRoot->addChild(testMeshSeparator);
            m_modelList.push_back(testMeshSeparator);
        }
    }

    FitView();
}

void NavQuarterWidget::Tf::NavigationWidget::Clear()
{
    if (!m_displayRoot)
    {
        return;
    }
    for (const auto& sep : m_modelList)
    {
        if (NULL != sep)
        {
            int index = m_displayRoot->findChild(sep);
            if (-1 != index)
            {
                m_displayRoot->removeChild(index);
                index = m_displayRoot->findChild(sep);
                assert(-1 == index);
            }
        }
    }
}

void NavQuarterWidget::Tf::NavigationWidget::setSceneGraph(SoNode* root)
{
    if (root == m_scene)
    {
        return;
    }

    if (m_scene)
    {
        m_scene->unref();
        m_scene = NULL;
    }

    SoCamera* camera = NULL;
    SoSeparator* superscene = NULL;

    if (root)
    {
        m_scene = root;
        m_scene->ref();
        superscene = new SoSeparator;

        //// Add lights
        superscene->addChild(m_ambientLight);
        superscene->addChild(m_pointLightTopLeft);
        superscene->addChild(m_pointLightTopRight);
        superscene->addChild(m_pointLightBottomLeft);
        superscene->addChild(m_pointLightBottomRight);

        //Must have, otherwise the entity is very large
        if (!(camera = searchForCamera(root)))
        {
            camera = new SoPerspectiveCamera;
            superscene->addChild(camera);
        }

        superscene->addChild(root);
    }

    this->getSoEventManager()->setCamera(camera);
    this->getSoRenderManager()->setCamera(camera);
    this->getSoEventManager()->setSceneGraph(superscene);
    this->getSoRenderManager()->setSceneGraph(superscene);
}

SoNode* NavQuarterWidget::Tf::NavigationWidget::getSceneGraph(void) const
{
    return m_scene;
}

SoRenderManager* NavQuarterWidget::Tf::NavigationWidget::getSoRenderManager(void) const
{
    return m_soRenderManager;
}

void NavQuarterWidget::Tf::NavigationWidget::setSoRenderManager(SoRenderManager* manager)
{
    bool carrydata = false;
    SoNode* scene = NULL;
    SoCamera* camera = NULL;
    SbViewportRegion viewportRegion;
    if (m_soRenderManager && (NULL != manager))
    {
        scene = m_soRenderManager->getSceneGraph();
        camera = m_soRenderManager->getCamera();
        viewportRegion = m_soRenderManager->getViewportRegion();
        carrydata = true;
    }

    // Ref before deleting the old scene manager to avoid that the nodes are deleted
    if (scene)
    {
        scene->ref();
    }

    if (camera)
    {
        camera->ref();
    }

    delete m_soRenderManager;
    m_soRenderManager = manager;

    if (carrydata)
    {
        m_soRenderManager->setSceneGraph(scene);
        m_soRenderManager->setCamera(camera);
        m_soRenderManager->setViewportRegion(viewportRegion);
    }

    if (scene)
    {
        scene->unref();
    }

    if (camera)
    {
        camera->unref();
    }
}

SoEventManager* NavQuarterWidget::Tf::NavigationWidget::getSoEventManager(void) const
{
    return m_soEventManager;
}

void NavQuarterWidget::Tf::NavigationWidget::setSoEventManager(SoEventManager* manager)
{
    bool carryData = false;
    SoNode* scene = NULL;
    SoCamera* camera = NULL;
    SbViewportRegion viewRegion;

    if (m_soEventManager && (NULL != manager))
    {
        scene = m_soEventManager->getSceneGraph();
        camera = m_soEventManager->getCamera();
        viewRegion = m_soEventManager->getViewportRegion();
        carryData = true;
    }

    // Ref before deleting the old scene manager to avoid that the nodes are deleted
    if (scene)
    {
        scene->ref();
    }

    if (camera)
    {
        camera->ref();
    }


    delete m_soEventManager;
    m_soEventManager = manager;

    if (carryData)
    {
        m_soEventManager->setSceneGraph(scene);
        m_soEventManager->setCamera(camera);
        m_soEventManager->setViewportRegion(viewRegion);
    }

    if (scene)
    {
        scene->unref();
    }

    if (camera)
    {
        camera->unref();
    }
}

void NavQuarterWidget::Tf::NavigationWidget::setBackgroundColor(const QColor& color)
{
    SbColor4f bgColor(SbClamp(color.red() / 255.0, 0.0, 1.0),
                      SbClamp(color.green() / 255.0, 0.0, 1.0),
                      SbClamp(color.blue() / 255.0, 0.0, 1.0),
                      SbClamp(color.alpha() / 255.0, 0.0, 1.0));

    this->getSoRenderManager()->setBackgroundColor(bgColor);
    this->getSoRenderManager()->scheduleRedraw();
}

QColor NavQuarterWidget::Tf::NavigationWidget::backgroundColor(void) const
{
    SbColor4f backgroundColor = this->getSoRenderManager()->getBackgroundColor();

    return QColor(SbClamp(int(backgroundColor[0] * 255.0), 0, 255),
                  SbClamp(int(backgroundColor[1] * 255.0), 0, 255),
                  SbClamp(int(backgroundColor[2] * 255.0), 0, 255),
                  SbClamp(int(backgroundColor[3] * 255.0), 0, 255));
}

void NavQuarterWidget::Tf::NavigationWidget::SetLights()
{
    m_ambientLight = new SoDirectionalLight;
    m_ambientLight->intensity = 1.0f;
    m_ambientLight->direction.setValue(0.0f, 0.0f, 1.0f);
    m_ambientLight->ref();

    m_pointLightTopLeft = new SoDirectionalLight;
    m_pointLightTopLeft->intensity = 0.68f;
    m_pointLightTopLeft->direction.setValue(1.f, -1.f, -1.f);
    m_pointLightTopLeft->ref();

    m_pointLightTopRight = new SoDirectionalLight;
    m_pointLightTopRight->intensity = 0.4f;
    m_pointLightTopRight->direction.setValue(-1.f, -1.f, -1.f);
    m_pointLightTopRight->ref();

    m_pointLightBottomLeft = new SoDirectionalLight;
    m_pointLightBottomLeft->intensity = 0.3f;
    m_pointLightBottomLeft->direction.setValue(1.f, 1.f, -1.f);
    m_pointLightBottomLeft->ref();

    m_pointLightBottomRight = new SoDirectionalLight;
    m_pointLightBottomRight->intensity = 0.3f;
    m_pointLightBottomRight->direction.setValue(-1.f, 1.f, -1.f);
    m_pointLightBottomRight->ref();
}

void NavQuarterWidget::Tf::NavigationWidget::Redraw(void)
{
    this->viewport()->update();
}

void NavQuarterWidget::Tf::NavigationWidget::Rendercb(void* userdata, SoRenderManager*)
{
    NavigationWidget* graphicsWidget = static_cast<NavigationWidget*>(userdata);
    graphicsWidget->Redraw();
}
