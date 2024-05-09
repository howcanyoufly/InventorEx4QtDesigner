/*========================================================================
Copyright (c) 2024
Unpublished - All rights reserved

==========================================================================
File description:
Reconstruction QGraphicsView
==========================================================================
Date            Name        Description of Change
2024-03-05      FJH         Init
==========================================================================*/
#ifndef TF_NAVIGATION_WIDGET_H
#define TF_NAVIGATION_WIDGET_H

#include "CommonHeaderFiles.h"
#include "NavigationStyle.h"

#include <Quarter/QuarterWidget.h>
#include <QtWidgets/QGraphicsView>

#include <memory>

class SoSeparator;
class SoGroup;
class SoSwitch;
class SoRenderManager;
class SoEventManager;
class SoNodeSensor;
class SbVec2s;
class SoEvent;
class SoScXMLStateMachine;
class SoDirectionalLight;
class SbColor;

class QOpenGLWidget;
class QSurfaceFormat;

namespace Uds
{
    namespace Geom
    {
        class Mesh;
    }
}

namespace NavQuarterWidget
{
    namespace Tf
    {
        class Material;
        class EventFilter;

        class __declspec(dllexport) NavigationWidget : public QGraphicsView
        {
            Q_OBJECT

                Q_PROPERTY(QUrl NavigationModeFile READ NavigationModeFile WRITE SetNavigationModeFile RESET ResetNavigationModeFile)

        public:

            explicit NavigationWidget(QWidget* parent = 0, const QOpenGLWidget* sharewidget = 0, Qt::WindowFlags f = Qt::WindowFlags());
            explicit NavigationWidget(const QSurfaceFormat& format, QWidget* parent = 0, const QOpenGLWidget* shareWidget = 0, Qt::WindowFlags f = Qt::WindowFlags());
            ~NavigationWidget();

            void Constructor(const QSurfaceFormat& format, const QOpenGLWidget* shareWidget);
            void InitializeGL(void);

            void Init();
            void SetCameraOrientation(const SbRotation& rot, SbBool moveToCenter = false);

            void SwitchCoordinateSystem(bool isShow);
            void FitView();
            void Clear();

            void setSceneGraph(SoNode* root);
            SoNode* getSceneGraph(void) const;

            SoRenderManager* getSoRenderManager(void) const;
            void setSoRenderManager(SoRenderManager* manager);

            SoEventManager* getSoEventManager(void) const;
            void setSoEventManager(SoEventManager* manager);

            void setBackgroundColor(const QColor& color);
            QColor backgroundColor(void) const;

            void SetLights();

            void ResetNavigationModeFile(void);
            void SetNavigationModeFile(const QUrl& url = QUrl(QString::fromLatin1(SIM::Coin3D::Quarter::DEFAULT_NAVIGATIONFILE)));
            const QUrl& NavigationModeFile(void) const;
            void AddStateMachine(SoScXMLStateMachine* statemachine);
            void RemoveStateMachine(SoScXMLStateMachine* statemachine);

            void ViewAll();

            void AddTestModel();

        protected:

            virtual void paintEvent(QPaintEvent*) override;
            virtual void resizeEvent(QResizeEvent*) override;

            virtual void mouseMoveEvent(QMouseEvent* event) override;
            virtual void mousePressEvent(QMouseEvent* event) override;
            virtual void mouseReleaseEvent(QMouseEvent* event) override;
            virtual void keyReleaseEvent(QKeyEvent* event) override;
            virtual void keyPressEvent(QKeyEvent* event) override;
            virtual void wheelEvent(QWheelEvent* event) override;


        public slots:
            void Redraw(void);
            static void Rendercb(void* userdata, SoRenderManager*);

        protected:

            SoSeparator* m_displayRoot;                    //根节点
            SoSwitch* m_coordinateSystemSwitch;            //轴系

            SoRenderManager* m_soRenderManager;
            SoEventManager* m_soEventManager;
            SoNode* m_scene;

            // directoinal light 灯光节点
            SoDirectionalLight* m_ambientLight;
            SoDirectionalLight* m_pointLightTopLeft;
            SoDirectionalLight* m_pointLightTopRight;
            SoDirectionalLight* m_pointLightBottomLeft;
            SoDirectionalLight* m_pointLightBottomRight;

            SoScXMLStateMachine* m_currentStateMachine;
            QUrl m_navigationModeFile;

        private:
            std::unique_ptr<NavigationStyle> m_navigation;
            QPointF m_zoomCenter;

            std::vector<SoSeparator*> m_modelList;
        };
    }
}

#endif // !TI_NAVIGATION_QUARTER_WIDGET_H