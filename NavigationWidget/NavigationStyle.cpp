/*========================================================================
Copyright (c) 2022
Unpublished - All rights reserved

==========================================================================
File description:
NavigationStyle Init
==========================================================================
Date            Name        Description of Change
2023-05-17      fjh         Init
2023-05-30      fjh         Renaming of variables
2023-10-27      FJH         Change the left mouse pan to middle mouse pan
2024-03-13      FJH         modify from change QuarterWidget to QGraphicsView
2024-03-28      FJH         change the speed of zoom
==========================================================================*/
#include "CommonHeaderFiles.h"
#include "NavigationStyle.h"
#include "NavigationWidget.h"

using namespace NavQuarterWidget::Tf;

struct NavigationStyleP {
    int animationsteps;
    int animationdelta;
    SbVec3f focal1, focal2;
    SbVec3f rotationCenter;
    SbBool rotationCenterFound;
    SbRotation endRotation;
    SoTimerSensor* animsensor;
    float sensitivity;
    SbBool resetcursorpos;

    NavigationStyleP()
    {
        this->animationsteps = 0;
        this->animationdelta = 0;
        this->animsensor = 0;
        this->sensitivity = 2.0f;
        this->resetcursorpos = false;
        this->rotationCenterFound = false;
        this->rotationCenter = SbVec3f(0, 0, 0);
    }
    static void viewAnimationCB(void* data, SoSensor* sensor);
};

class SphereSheetProjector : public SbSphereSheetProjector {

public:
    enum OrbitStyle {
        OS_Turntable,
        OS_Trackball
    };

    SphereSheetProjector(const SbSphere& sph, const SbBool orienttoeye = true)
        : SbSphereSheetProjector(sph, orienttoeye), orbit(OS_Trackball)
    {
    }

    void setViewVolume(const SbViewVolume& vol)
    {
        SbSphereSheetProjector::setViewVolume(vol);
    }

    void setWorkingSpace(const SbMatrix& space)
    {
        //inherited::setWorkingSpace(space);
        this->worldToScreen = space.inverse();
    }

    SbVec3f project(const SbVec2f& point)
    {
        return SbSphereSheetProjector::project(point);
    }

    SbRotation getRotation(const SbVec3f& point1, const SbVec3f& point2)
    {
        SbRotation rot = SbSphereSheetProjector::getRotation(point1, point2);
        if (orbit == OS_Trackball)
            return rot;

        // 0000333: Turntable camera rotation
        SbVec3f axis;
        float angle;
        rot.getValue(axis, angle);
        SbVec3f dif = point1 - point2;
        if (fabs(dif[1]) > fabs(dif[0])) {
            SbVec3f xaxis(1, 0, 0);
            if (dif[1] < 0)
                angle = -angle;
            rot.setValue(xaxis, angle);
        }
        else {
            SbVec3f zaxis(0, 0, 1);
            this->worldToScreen.multDirMatrix(zaxis, zaxis);
            if (zaxis[1] < 0) {
                if (dif[0] < 0)
                    angle = -angle;
            }
            else {
                if (dif[0] > 0)
                    angle = -angle;
            }
            rot.setValue(zaxis, angle);
        }

        return rot;
    }

    void setOrbitStyle(OrbitStyle style)
    {
        this->orbit = style;
    }

    OrbitStyle getOrbitStyle() const
    {
        return this->orbit;
    }

private:
    SbMatrix worldToScreen;
    OrbitStyle orbit;
};

#define PRIVATE(ptr) (ptr->m_pimpl)

//TYPE_SYSTEM_SOURCE_ABSTRACT(NavigationStyle, Uds::Base::BaseClass)

NavigationStyle::NavigationStyle() : m_viewer(0)
{
    PRIVATE(this) = new NavigationStyleP();
    PRIVATE(this)->animsensor = new SoTimerSensor(NavigationStyleP::viewAnimationCB, this);
    Initialize();
}

NavigationStyle::~NavigationStyle()
{
    Finalize();
    if (PRIVATE(this)->animsensor->isScheduled())
        PRIVATE(this)->animsensor->unschedule();
    delete PRIVATE(this)->animsensor;
    delete PRIVATE(this);
}

NavigationStyle& NavigationStyle::operator = (const NavigationStyle& ns)
{
    this->m_panningplane = ns.m_panningplane;
    this->m_spinanimatingallowed = ns.m_spinanimatingallowed;
    static_cast<SphereSheetProjector*>(this->m_spinprojector)->setOrbitStyle
    (static_cast<SphereSheetProjector*>(ns.m_spinprojector)->getOrbitStyle());
    return *this;
}

void NavigationStyle::SetResetCursorPosition(SbBool on)
{
    PRIVATE(this)->resetcursorpos = on;
}

SbBool NavigationStyle::IsResetCursorPosition() const
{
    return PRIVATE(this)->resetcursorpos;
}

void NavigationStyle::Initialize()
{
    this->m_currentmode = VM_IDLE;
    this->m_spinanimatingallowed = true;
    this->m_spinsamplecounter = 0;
    this->m_spinincrement = SbRotation::identity();
    this->m_spinRotation.setValue(SbVec3f(0, 0, -1), 0);

    // FIXME: use a smaller sphere than the default one to have a larger
    // area close to the borders that gives us "z-axis rotation"?
    // 19990425 mortene.
    this->m_spinprojector = new SphereSheetProjector(SbSphere(SbVec3f(0, 0, 0), 0.8f));
    SbViewVolume volume;
    volume.ortho(-1, 1, -1, 1, -1, 1);
    this->m_spinprojector->setViewVolume(volume);

    m_pickRadius = 0.5;
    m_pickpoint = new SoSeparator();
    m_pickpoint->ref();

    this->m_log.size = 16;
    this->m_log.position = new SbVec2s[16];
    this->m_log.time = new SbTime[16];
    this->m_log.historysize = 0;

    this->m_button1down = false;
    this->m_button2down = false;
    this->m_button3down = false;
    this->m_ctrldown = false;
    this->m_shiftdown = false;
    this->m_altdown = false;
    this->m_invertZoom = true;
    this->m_zoomAtCursor = true;
    this->m_zoomStep = 0.1f;
    this->m_isfit = false;
}
void NavigationStyle::Finalize()
{
    delete this->m_spinprojector;
    delete[] this->m_log.position;
    delete[] this->m_log.time;

    m_pickpoint->unref();
}

void NavigationStyle::SetViewer(NavigationWidget* viewer)
{
    this->m_viewer = viewer;
}

void NavigationStyle::Zoom(SoCamera* cam, float diffvalue)
{
    if (cam == NULL) return; // can happen for empty scenegraph
    SoType t = cam->getTypeId();
    SbName tname = t.getName();

    // This will be in the range of <0, ->>.
    float multiplicator = float(exp(diffvalue));
    if (t.isDerivedFrom(SoOrthographicCamera::getClassTypeId())) {

        // Since there's no perspective, "zooming" in the original sense
        // of the word won't have any visible effect. So we just increase
        // or decrease the field-of-view values of the camera instead, to
        // "shrink" the projection size of the model / scene.
        SoOrthographicCamera* oc = (SoOrthographicCamera*)cam;
        oc->height = oc->height.getValue() * multiplicator;
    }
    else {
        // FrustumCamera can be found in the SmallChange CVS module (it's
        // a camera that lets you specify (for instance) an off-center
        // frustum (similar to glFrustum())
        if (!t.isDerivedFrom(SoPerspectiveCamera::getClassTypeId()) &&
            tname != "FrustumCamera") {
            /*         static SbBool first = true;
                       if (first) {
                           SoDebugError::postWarning("SoGuiFullViewerP::zoom",
                                                     "Unknown camera type, "
                                                     "will zoom by moving position, but this might not be correct.");
                           first = false;
                       }*/
        }

        const float oldfocaldist = cam->focalDistance.getValue();
        const float newfocaldist = oldfocaldist * multiplicator;

        SbVec3f direction;
        cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);

        const SbVec3f oldpos = cam->position.getValue();
        const SbVec3f newpos = oldpos + (newfocaldist - oldfocaldist) * -direction;

        // This catches a rather common user interface "buglet": if the
        // user zooms the camera out to a distance from origo larger than
        // what we still can safely do floating point calculations on
        // (i.e. without getting NaN or Inf values), the faulty floating
        // point values will propagate until we start to get debug error
        // messages and eventually an assert failure from core Coin code.
        //
        // With the below bounds check, this problem is avoided.
        //
        // (But note that we depend on the input argument ''diffvalue'' to
        // be small enough that zooming happens gradually. Ideally, we
        // should also check distorigo with isinf() and isnan() (or
        // inversely; isinfite()), but those only became standardized with
        // C99.)
        const float distorigo = newpos.length();
        // sqrt(FLT_MAX) == ~ 1e+19, which should be both safe for further
        // calculations and ok for the end-user and app-programmer.
        if (distorigo > float(sqrt(FLT_MAX))) {
            // do nothing here
        }
        else {
            cam->position = newpos;
            cam->focalDistance = newfocaldist;
        }
    }
}

void NavigationStyle::ZoomByCursor(const SbVec2f& thispos, const SbVec2f& prevpos)
{
    // There is no "geometrically correct" value, 20 just seems to give
    // about the right "feel".
    float value = (thispos[1] - prevpos[1]) * /*10.0f*/20.0f;
    if (this->m_invertZoom)
        value = -value;
    Zoom(m_viewer->getSoRenderManager()->getCamera(), value);
}

void NavigationStyle::ZoomIn()
{
    Zoom(m_viewer->getSoRenderManager()->getCamera(), -this->m_zoomStep);
}

void NavigationStyle::ZoomOut()
{
    Zoom(m_viewer->getSoRenderManager()->getCamera(), this->m_zoomStep);
}

int NavigationStyle::GetDelta() const
{
    return 120;
}

void NavigationStyle::DoZoom(SoCamera* camera, int wheeldelta, const SbVec2f& pos)
{
    float value = this->m_zoomStep * wheeldelta / float(GetDelta());
    if (this->m_invertZoom)
        value = -value;
    DoZoom(camera, value, pos);
}

void NavigationStyle::DoZoom(SoCamera* camera, float logfactor, const SbVec2f& pos)
{
    // something is asking for big zoom factor. This func is made for interactive zooming,
    // where the changes are per mouse move and thus are small.
    if (fabs(logfactor) > 4.0)
        return;
    SbBool zoomAtCur = this->m_zoomAtCursor;
    if (zoomAtCur) {
        const SbViewportRegion& vp = m_viewer->getSoRenderManager()->getViewportRegion();
        float ratio = vp.getViewportAspectRatio();
        SbViewVolume vv = camera->getViewVolume(vp.getViewportAspectRatio());
        SbPlane panplane = vv.getPlane(camera->focalDistance.getValue());
        PanCamera(m_viewer->getSoRenderManager()->getCamera(), ratio, panplane, SbVec2f(0.5, 0.5), pos);
    }

    Zoom(camera, logfactor);

    if (zoomAtCur) {
        const SbViewportRegion& vp = m_viewer->getSoRenderManager()->getViewportRegion();
        float ratio = vp.getViewportAspectRatio();
        SbViewVolume vv = camera->getViewVolume(vp.getViewportAspectRatio());
        SbPlane panplane = vv.getPlane(camera->focalDistance.getValue());
        PanCamera(m_viewer->getSoRenderManager()->getCamera(), ratio, panplane, pos, SbVec2f(0.5, 0.5));
    }
}

SbVec3f NavigationStyle::GetRotationCenter(SbBool* ok) const
{
    if (ok)
        *ok = PRIVATE(this)->rotationCenterFound;
    return PRIVATE(this)->rotationCenter;
}

void NavigationStyle::SetRotationCenter(const SbVec3f& cnt)
{
    PRIVATE(this)->rotationCenter = cnt;
    PRIVATE(this)->rotationCenterFound = true;
}

SbVec3f NavigationStyle::GetFocalPoint() const
{
    SoCamera* cam = m_viewer->getSoRenderManager()->getCamera();
    if (cam == 0)
        return SbVec3f(0, 0, 0);

    // Find global coordinates of focal point.
    SbVec3f direction;
    cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
    SbVec3f focal = cam->position.getValue() +
        cam->focalDistance.getValue() * direction;
    return focal;
}

void NavigationStyle::Spin(const SbVec2f& pointerpos)
{
    if (this->m_log.historysize < 3) return;
    assert(this->m_spinprojector != NULL);

    const SbViewportRegion& vp = m_viewer->getSoRenderManager()->getViewportRegion();
    SbVec2s glsize(vp.getViewportSizePixels());
    SbVec2f lastpos;
    lastpos[0] = float(this->m_log.position[1][0]) / float(std::max((int)(glsize[0] - 1), 1));
    lastpos[1] = float(this->m_log.position[1][1]) / float(std::max((int)(glsize[1] - 1), 1));

    if (PRIVATE(this)->rotationCenterFound) {
        SbVec3f hitpoint = PRIVATE(this)->rotationCenter;

        // set to the given position
        SbVec3f direction;
        m_viewer->getSoRenderManager()->getCamera()->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
        m_viewer->getSoRenderManager()->getCamera()->position = hitpoint - m_viewer->getSoRenderManager()->getCamera()->focalDistance.getValue() * direction;
    }

    // 0000333: Turntable camera rotation
    SbMatrix mat;
    m_viewer->getSoRenderManager()->getCamera()->orientation.getValue().getValue(mat);
    this->m_spinprojector->setWorkingSpace(mat);

    this->m_spinprojector->project(lastpos);
    SbRotation r;
    this->m_spinprojector->projectAndGetRotation(pointerpos, r);
    float sensitivity = GetSensitivity();
    if (sensitivity > 1.0f) {
        SbVec3f axis;
        float radians;
        r.getValue(axis, radians);
        radians = sensitivity * radians;
        r.setValue(axis, radians);
    }
    r.invert();
    this->ReorientCamera(m_viewer->getSoRenderManager()->getCamera(), r);

    if (PRIVATE(this)->rotationCenterFound) {
        float ratio = vp.getViewportAspectRatio();
        SbViewVolume vv = m_viewer->getSoRenderManager()->getCamera()->getViewVolume(vp.getViewportAspectRatio());
        SbPlane panplane = vv.getPlane(m_viewer->getSoRenderManager()->getCamera()->focalDistance.getValue());
        SbVec2f posn;
        posn[0] = float(this->m_localPos[0]) / float(std::max((int)(glsize[0] - 1), 1));
        posn[1] = float(this->m_localPos[1]) / float(std::max((int)(glsize[1] - 1), 1));
        PanCamera(m_viewer->getSoRenderManager()->getCamera(), ratio, panplane, posn, SbVec2f(0.5, 0.5));
    }

    // Calculate an average angle magnitude value to make the transition
    // to a possible spin animation mode appear smooth.

    SbVec3f dummy_axis, newaxis;
    float acc_angle, newangle;
    this->m_spinincrement.getValue(dummy_axis, acc_angle);
    acc_angle *= this->m_spinsamplecounter; // weight
    r.getValue(newaxis, newangle);
    acc_angle += newangle;

    this->m_spinsamplecounter++;
    acc_angle /= this->m_spinsamplecounter;
    // FIXME: accumulate and average axis vectors as well? 19990501 mortene.
    this->m_spinincrement.setValue(newaxis, acc_angle);

    // Don't carry too much baggage, as that'll give unwanted results
    // when the user quickly trigger (as in "click-drag-release") a spin
    // animation.
    if (this->m_spinsamplecounter > 3) this->m_spinsamplecounter = 3;
}

void NavigationStyle::SaveCursorPosition(const SoEvent* const ev)
{
    m_viewer->SwitchCoordinateSystem(false);

    const SbViewportRegion& vp = m_viewer->getSoRenderManager()->getViewportRegion();
    float ratio = vp.getViewportAspectRatio();

    SoCamera* cam = m_viewer->getSoRenderManager()->getCamera();
    if (!cam) return; // no camera

    SoGetBoundingBoxAction action(m_viewer->getSoRenderManager()->getViewportRegion());
    action.apply(m_viewer->getSceneGraph());
    SbBox3f boundingBox = action.getBoundingBox();
    SbVec3f boundingBoxCenter = boundingBox.getCenter();
    SetRotationCenter(boundingBoxCenter);

    m_viewer->SwitchCoordinateSystem(true);
    // To drag around the center point of the bbox we have to determine
    // its projection on the screen because this information is used in
    // NavigationStyle::spin() for the panning
    SbViewVolume vv = cam->getViewVolume(ratio);
    vv.projectToScreen(boundingBoxCenter, boundingBoxCenter);
    SbVec2s size = vp.getViewportSizePixels();
    short tox = static_cast<short>(boundingBoxCenter[0] * size[0]);
    short toy = static_cast<short>(boundingBoxCenter[1] * size[1]);
    this->m_localPos.setValue(tox, toy);
}


float NavigationStyle::GetSensitivity() const
{
    return PRIVATE(this)->sensitivity;
}

void NavigationStyle::ReorientCamera(SoCamera* cam, const SbRotation& rot)
{
    if (cam == NULL) return;

    // Find global coordinates of focal point.
    SbVec3f direction;
    cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
    SbVec3f focalpoint = cam->position.getValue() +
        cam->focalDistance.getValue() * direction;

    // Set new orientation value by accumulating the new rotation.
    cam->orientation = rot * cam->orientation.getValue();

    // Reposition camera so we are still pointing at the same old focal point.
    cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
    cam->position = focalpoint - cam->focalDistance.getValue() * direction;
}

void NavigationStyle::SetZoomInverted(SbBool on)
{
    this->m_invertZoom = on;
}

SbBool NavigationStyle::IsZoomInverted() const
{
    return this->m_invertZoom;
}

void NavigationStyle::SetZoomStep(float val)
{
    this->m_zoomStep = val;
}

void NavigationStyle::SetZoomAtCursor(SbBool on)
{
    this->m_zoomAtCursor = on;
}

SbBool NavigationStyle::IsZoomAtCursor() const
{
    return this->m_zoomAtCursor;
}

void NavigationStyle::ClearLog()
{
    this->m_log.historysize = 0;
}

void NavigationStyle::AddToLog(const SbVec2s pos, const SbTime time)
{
    // In case someone changes the const size setting at the top of this
    // file too small.
    assert(this->m_log.size > 2 && "mouse log too small!");

    if (this->m_log.historysize > 0 && pos == this->m_log.position[0]) {
        return;
    }

    int lastidx = this->m_log.historysize;
    // If we've filled up the log, we should throw away the last item:
    if (lastidx == this->m_log.size) { lastidx--; }

    assert(lastidx < this->m_log.size);
    for (int i = lastidx; i > 0; i--) {
        this->m_log.position[i] = this->m_log.position[i - 1];
        this->m_log.time[i] = this->m_log.time[i - 1];
    }

    this->m_log.position[0] = pos;
    this->m_log.time[0] = time;
    if (this->m_log.historysize < this->m_log.size)
        this->m_log.historysize += 1;
}

void NavigationStyle::MoveCursorPosition()
{
    if (!IsResetCursorPosition())
        return;

    QPoint cpos = QCursor::pos();
    if (abs(cpos.x() - m_globalPos[0]) > 10 ||
        abs(cpos.y() - m_globalPos[1]) > 10) {
        QCursor::setPos(m_globalPos[0], m_globalPos[1] - 1);
        this->m_log.position[0] = m_localPos;
    }
}

void NavigationStyle::SetViewingMode(const ViewerMode newmode)
{
    const ViewerMode oldmode = this->m_currentmode;
    if (newmode == oldmode) { return; }

    switch (newmode) {
    case VM_DRAGGING:
        // Set up initial projection point for the projector object when
        // first starting a drag operation.
    {
        this->m_spinprojector->project(this->m_lastmouseposition);
        this->ClearLog();
        break;
    }

    case VM_PANNING:
    {
        Pan(m_viewer->getSoRenderManager()->getCamera());
        break;
    }

    default: // include default to avoid compiler warnings.
    {
        break;
    }
    }
    this->m_currentmode = newmode;
}

int NavigationStyle::GetViewingMode() const
{
    return (int)this->m_currentmode;
}

SoSeparator* NavigationStyle::GetPickPointNode()
{
    return m_pickpoint;
}

//paint the center point
void NavigationStyle::SetPointIsHighLight(const SbVec3f& position, const bool& on = true)
{
    if (on)
    {
        SoSeparator* pointx = new SoSeparator();
        SoSphere* point = new SoSphere();
        point->ref();
        point->radius.setValue(6.0f);
        SoTransform* tran = new SoTransform();
        tran->translation.setValue(position);

        pointx->addChild(tran);

        m_pickpoint->addChild(pointx);
    }
    else
    {
        m_pickpoint->removeAllChildren();
    }
}

void NavQuarterWidget::Tf::NavigationStyle::DoPan(SoCamera* camera, const SbVec2f& posn)
{
    const SbVec2f prevnormalized = m_lastmouseposition;
    m_lastmouseposition = posn;

    float ratio = m_viewer->getSoRenderManager()->getViewportRegion().getViewportAspectRatio();
    PanCamera(m_viewer->getSoRenderManager()->getCamera(), ratio, m_panningplane, posn, prevnormalized);

    //SaveCursorPosition(NULL);
    Pan(m_viewer->getSoRenderManager()->getCamera());
}

void NavQuarterWidget::Tf::NavigationStyle::DoSpin(SoCamera* camera, const SbVec2s& pos, const SbTime& time, const SbVec2f& posn)
{
    const SbVec2f prevnormalized = m_lastmouseposition;
    m_lastmouseposition = posn;

    AddToLog(pos, time);
    Spin(posn);
    MoveCursorPosition();
}

SbBool NavigationStyle::ProcessSoEvent(const SoEvent* const ev)
{
    //return viewer->processSoEvent(ev);

    const SoType type(ev->getTypeId());

    const SbViewportRegion& vp = m_viewer->getSoRenderManager()->getViewportRegion();
    const SbVec2s size(vp.getViewportSizePixels());
    const SbVec2f prevnormalized = this->m_lastmouseposition;
    const SbVec2s pos(ev->getPosition());
    const SbVec2f posn((float)pos[0] / (float)std::max((int)(size[0] - 1), 1),
                       (float)pos[1] / (float)std::max((int)(size[1] - 1), 1));

    this->m_lastmouseposition = posn;

    // Set to true if any event processing happened. Note that it is not
    // necessary to restrict ourselves to only do one "action" for an
    // event, we only need this flag to see if any processing happened
    // at all.
    SbBool processed = false;

    const ViewerMode curmode = this->m_currentmode;
    ViewerMode newmode = curmode;

    // Keyboard handling
    if (type.isDerivedFrom(SoKeyboardEvent::getClassTypeId())) {
        const SoKeyboardEvent* const event = (const SoKeyboardEvent*)ev;
        const SbBool press = event->getState() == SoButtonEvent::DOWN ? true : false;
        switch (event->getKey()) {
        case SoKeyboardEvent::LEFT_CONTROL:
        case SoKeyboardEvent::RIGHT_CONTROL:
            this->m_ctrldown = press;
            break;
        case SoKeyboardEvent::LEFT_SHIFT:
        case SoKeyboardEvent::RIGHT_SHIFT:
            this->m_shiftdown = press;
            break;
        case SoKeyboardEvent::LEFT_ALT:
        case SoKeyboardEvent::RIGHT_ALT:
            this->m_altdown = press;
            break;
        }
    }

    // Mouse Button / Spaceball Button handling
    if (type.isDerivedFrom(SoMouseButtonEvent::getClassTypeId())) {
        const SoMouseButtonEvent* const event = (const SoMouseButtonEvent*)ev;
        const int button = event->getButton();
        const SbBool press = event->getState() == SoButtonEvent::DOWN ? true : false;

        // SoDebugError::postInfo("processSoEvent", "button = %d", button);
        switch (button) {
        case SoMouseButtonEvent::BUTTON1:
            this->m_button1down = press;

            if (press && this->m_currentmode == VM_PANNING) {
                processed = true;
            }
            else if (press && (this->m_currentmode == VM_IDLE)) {
                processed = true;
            }
            else if (!press && (this->m_currentmode == VM_DRAGGING)) {
                processed = true;
            }
            break;
        case SoMouseButtonEvent::BUTTON3:
        {
            this->m_button3down = press;
            break;
        }
        default:
            break;
        }
    }

    // Mouse Movement handling
    if (type.isDerivedFrom(SoLocation2Event::getClassTypeId())) {

        const SoLocation2Event* const event = (const SoLocation2Event*)ev;
        if (this->m_currentmode == VM_PANNING) {
            float ratio = vp.getViewportAspectRatio();
            PanCamera(m_viewer->getSoRenderManager()->getCamera(), ratio, this->m_panningplane, posn, prevnormalized);
            processed = true;
        }
        else if (this->m_currentmode == VM_DRAGGING) {
            this->AddToLog(event->getPosition(), event->getTime());
            this->Spin(posn);
            MoveCursorPosition();
            processed = true;
        }
    }
    enum {
        BUTTON1DOWN = 1 << 0,
        BUTTON3DOWN = 1 << 1,
        CTRLDOWN = 1 << 2,
        SHIFTDOWN = 1 << 3,
        BUTTON2DOWN = 1 << 4

    };
    unsigned int combo =
        (this->m_button1down ? BUTTON1DOWN : 0) |
        (this->m_button2down ? BUTTON2DOWN : 0) |
        (this->m_button3down ? BUTTON3DOWN : 0) |
        (this->m_ctrldown ? CTRLDOWN : 0) |
        (this->m_shiftdown ? SHIFTDOWN : 0);

    switch (combo) {
    case 0:
        newmode = VM_IDLE;
        SetPointIsHighLight({ 0,0,0 }, false);
        break;
    case BUTTON3DOWN:
    {
        if (newmode != VM_DRAGGING)
        {
            SaveCursorPosition(ev);
        }
        newmode = VM_DRAGGING;
        this->m_isfit = false;
    }
    break;
    case SHIFTDOWN | BUTTON3DOWN:
        newmode = VM_PANNING;
        break;
    default:
        break;
    }
    if (newmode != curmode) {
        this->SetViewingMode(newmode);
    }

    // If not handled in this class, pass on upwards in the inheritance
    // hierarchy.
    if (!processed) {
        //processed = m_viewer->processNavigationSoEvent(ev);
    }
    else
    {
        return true;
    }
    return processed;
}

void NavigationStyle::PanCamera(SoCamera* cam, float aspectratio, const SbPlane& panplane,
                                const SbVec2f& currpos, const SbVec2f& prevpos)
{
    if (cam == NULL) return; // can happen for empty scenegraph
    if (currpos == prevpos) return; // useless invocation


    // Find projection points for the last and current mouse coordinates.
    SbViewVolume vv = cam->getViewVolume(aspectratio);

    // See note in Coin docs for SoCamera::getViewVolume re:viewport mapping
    if (aspectratio < 1.0)
        vv.scale(1.0 / aspectratio);

    SbLine line;
    vv.projectPointToLine(currpos, line);

    SbVec3f current_planept;
    panplane.intersect(line, current_planept);
    vv.projectPointToLine(prevpos, line);
    SbVec3f old_planept;
    panplane.intersect(line, old_planept);

    // Reposition camera according to the vector difference between the
    // projected points.
    cam->position.setValue(cam->position.getValue() - (current_planept - old_planept));

    m_viewer->update();
}

void NavigationStyle::Pan(SoCamera* camera)
{
    // The plane we're projecting the mouse coordinates to get 3D
    // coordinates should stay the same during the whole pan
    // operation, so we should calculate this value here.
    if (camera == NULL) { // can happen for empty scenegraph
        this->m_panningplane = SbPlane(SbVec3f(0, 0, 1), 0);
    }
    else {
        const SbViewportRegion& vp = m_viewer->getSoRenderManager()->getViewportRegion();
        float aspectratio = vp.getViewportAspectRatio();
        SbViewVolume vv = camera->getViewVolume(aspectratio);

        // See note in Coin docs for SoCamera::getViewVolume re:viewport mapping
        if (aspectratio < 1.0)
            vv.scale(1.0 / aspectratio);

        this->m_panningplane = vv.getPlane(camera->focalDistance.getValue());
    }
}

void NavigationStyleP::viewAnimationCB(void* data, SoSensor* sensor)
{
    Q_UNUSED(sensor);
    NavigationStyle* that = reinterpret_cast<NavigationStyle*>(data);
    that->m_pimpl;
    if (PRIVATE(that)->animationsteps > 0) {
        // here the camera rotates from the current rotation to a given
        // rotation (e.g. the standard views). To get this movement animated
        // we calculate an interpolated rotation and update the view after
        // each step
        float step = std::min<float>((float)PRIVATE(that)->animationsteps / 100.0f, 1.0f);
        SbRotation slerp = SbRotation::slerp(that->m_spinRotation, PRIVATE(that)->endRotation, step);
        SbVec3f focalpoint = (1.0f - step) * PRIVATE(that)->focal1 + step * PRIVATE(that)->focal2;
        SoCamera* cam = that->m_viewer->getSoRenderManager()->getCamera();
        if (!cam) return; // no camera

        SbVec3f direction;
        cam->orientation.setValue(slerp);
        cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
        cam->position = focalpoint - cam->focalDistance.getValue() * direction;

        PRIVATE(that)->animationsteps += PRIVATE(that)->animationdelta;
        if (PRIVATE(that)->animationsteps > 100) {
            // now we have reached the end of the movement
            PRIVATE(that)->animationsteps = 0;
            PRIVATE(that)->animsensor->unschedule();
            //   that->interactiveCountDec();
               // set to the actual given rotation
            cam->orientation.setValue(PRIVATE(that)->endRotation);
            cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
            cam->position = PRIVATE(that)->focal2 - cam->focalDistance.getValue() * direction;
        }
    }
}