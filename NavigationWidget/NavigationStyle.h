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
2024-03-13      FJH         modify from change QuarterWidget to QGraphicsView
==========================================================================*/
#ifndef TF_NAVIGATION_STYLE_H
#define TF_NAVIGATION_STYLE_H

#include <Inventor/SbPlane.h>
#include <Inventor/SbTime.h>
#include <Inventor/SbRotation.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/nodes/SoSeparator.h>

class SoQtViewer;
class SoCamera;
class SbVec3f;
class SbVec2f;
class SbVec2s;
class SbSphereSheetProjector;
struct NavigationStyleP;
namespace NavQuarterWidget
{
    namespace Tf
    {
        class NavigationQuarterWidget;
        class NavigationWidget;
        class __declspec(dllexport) NavigationStyle
        {
            friend class NavigationQuarterWidget;
            friend class NavigationWidget;
            friend struct NavigationStyleP;
        public:
            enum ViewerMode {
                VM_IDLE,
                VM_PANNING,
                VM_DRAGGING,
            };

            enum class RotationCenterMode {
                RCM_WindowCenter = 0, /**< The center of the window */
                RCM_ScenePointAtCursor = 1, /**< Find the point in the scene at the cursor position. If there is no point then the focal plane is used */
                RCM_FocalPointAtCursor = 2, /**< Find the point on the focal plane at the cursor position. */
                RCM_BoundingBoxCenter = 4  /**< Find the center point of the bounding box of the scene. */
            };
            Q_DECLARE_FLAGS(RotationCenterModes, RotationCenterMode)

        public:
            NavigationStyle();
            virtual ~NavigationStyle();

            NavigationStyle& operator = (const NavigationStyle& ns);

            void SetViewer(NavigationWidget* viewer);

            void SetResetCursorPosition(SbBool on);
            SbBool IsResetCursorPosition() const;

            void SetZoomInverted(SbBool on);
            SbBool IsZoomInverted() const;
            void SetZoomStep(float val);
            void SetZoomAtCursor(SbBool on);
            SbBool IsZoomAtCursor() const;
            void ZoomIn();
            void ZoomOut();

            void SetRotationCenter(const SbVec3f& cnt);
            SbVec3f GetRotationCenter(SbBool* ok) const;

            SbVec3f GetFocalPoint() const;

            void SaveCursorPosition(const SoEvent* const ev);
            void MoveCursorPosition();

            float GetSensitivity() const;

            void SetViewingMode(const ViewerMode newmode);
            int GetViewingMode() const;

            SoSeparator* GetPickPointNode();

        protected:
            void Initialize();
            void Finalize();

            void SetPointIsHighLight(const SbVec3f& position, const bool& on);

            void ReorientCamera(SoCamera* camera, const SbRotation& rot);
            void PanCamera(SoCamera* camera, float vpaspect, const SbPlane& panplane,
                           const SbVec2f& previous, const SbVec2f& current);
            void Pan(SoCamera* camera);

            //zoom
            int GetDelta() const;
            void Zoom(SoCamera* camera, float diffvalue);
            void ZoomByCursor(const SbVec2f& thispos, const SbVec2f& prevpos);
            void DoZoom(SoCamera* camera, int wheeldelta, const SbVec2f& pos);
            void DoZoom(SoCamera* camera, float logzoomfactor, const SbVec2f& pos);

            //平移
            void DoPan(SoCamera* camera, const SbVec2f& posn);

            //旋转
            void DoSpin(SoCamera* camera, const SbVec2s& pos, const SbTime& time, const SbVec2f& posn);

            //spin
            void Spin(const SbVec2f& pointerpos);

            void ClearLog();
            void AddToLog(const SbVec2s pos, const SbTime time);

        public:
            virtual SbBool ProcessSoEvent(const SoEvent* const ev);

        private:
            NavigationStyle(const NavigationStyle&);
        protected:
            struct { // tracking mouse movement in a log
                short size;
                short historysize;
                SbVec2s* position;
                SbTime* time;
            } m_log;

            float m_pickRadius;

            //NavigationQuarterWidget* m_viewer;
            NavigationWidget* m_viewer;
            ViewerMode m_currentmode;
            SbVec2f m_lastmouseposition;
            SbVec2s m_globalPos;
            SbVec2s m_localPos;
            SbPlane m_panningplane;
            SbTime m_centerTime;
            SbBool m_ctrldown, m_shiftdown, m_altdown;
            SbBool m_button1down, m_button2down, m_button3down;
            SbBool m_invertZoom;
            SbBool m_zoomAtCursor;
            float m_zoomStep;
            SbBool m_spinanimatingallowed;
            int m_spinsamplecounter;
            SbRotation m_spinincrement;
            SbRotation m_spinRotation;
            SbSphereSheetProjector* m_spinprojector;
            SoSeparator* m_pickpoint;
            SbBool m_isfit;

        private:
            struct NavigationStyleP* m_pimpl;
        };
    }
}

Q_DECLARE_OPERATORS_FOR_FLAGS(NavQuarterWidget::Tf::NavigationStyle::RotationCenterModes)
#endif // TI_NAVIGATION_STYLE_H