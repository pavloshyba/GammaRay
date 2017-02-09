/*
  quickoverlay.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKOVERLAY_H
#define GAMMARAY_QUICKINSPECTOR_QUICKOVERLAY_H

#include <QObject>
#include <QPen>

#include "quickitemgeometry.h"

QT_BEGIN_NAMESPACE
class QQuickItem;
class QQuickWindow;
QT_END_NAMESPACE

namespace GammaRay {

class ItemOrLayoutFacade
{
public:
    ItemOrLayoutFacade();
    ItemOrLayoutFacade(QQuickItem *item); //krazy:exclude=explicit

    /// Get either the layout of the widget or the this-pointer
    QQuickItem *layout() const;

    /// Get either the parent widget of the layout or the this-pointer
    QQuickItem *item() const;

    QRectF geometry() const;
    bool isVisible() const;
    QPointF pos() const;

    inline bool isNull() const { return !m_object; }
    inline QQuickItem* data() { return m_object; }
    inline QQuickItem* operator->() const { Q_ASSERT(!isNull()); return m_object; }
    inline void clear() { m_object = nullptr; }

private:
    bool isLayout() const;
    inline QQuickItem *asLayout() const { return m_object; }
    inline QQuickItem *asItem() const { return m_object; }

    QQuickItem *m_object;
};

class QuickOverlay : public QObject
{
    Q_OBJECT

public:
    QuickOverlay();

    struct RenderInfo {
        RenderInfo(const QuickItemGeometry &itemGeometry = QuickItemGeometry(),
                   const QRectF &viewRect = QRectF(),
                   qreal zoom = 1.0)
            : itemGeometry(itemGeometry)
            , viewRect(viewRect)
            , zoom(zoom)
        { }

        const QuickItemGeometry itemGeometry;
        const QRectF viewRect;
        const qreal zoom;
    };

    QQuickWindow *window() const;
    void setWindow(QQuickWindow *window);

    /**
     * Place the overlay on @p item
     *
     * @param item The overlay can be cover a widget or a layout of the current window
     */
    void placeOn(ItemOrLayoutFacade item);

    bool drawDecorations() const;
    void setDrawDecorations(bool enabled);

    static void drawDecoration(QPainter *painter, const RenderInfo &renderInfo);

    static const QColor BoundingRectColor;
    static const QBrush BoundingRectBrush;

    static const QColor GeometryRectColor;
    static const QBrush GeometryRectBrush;

    static const QColor ChildrenRectColor;
    static const QBrush ChildrenRectBrush;

    static const QColor TransformOriginColor;

    static const QColor CoordinatesColor;

    static const QColor MarginsColor;

    static const QColor PaddingColor;

public slots:
    void requestGrabWindow();

signals:
    void sceneChanged();
    void sceneGrabbed(const QImage &image);

private:
    struct DrawTextInfo {
        DrawTextInfo(const QPen &pen = {}, const QRectF &rect = {},
                     const QString &label = {}, int align = Qt::AlignCenter | Qt::TextDontClip)
            : pen(pen)
            , rect(rect)
            , label(label)
            , align(align)
        { }

        QPen pen;
        QRectF rect;
        QString label;
        int align = 0;
    };

    using DrawTextInfoList = QVector<DrawTextInfo>;

    static void drawArrow(QPainter *p, QPointF first, QPointF second);
    static void drawAnchor(QPainter *p, const RenderInfo &renderInfo, Qt::Orientation orientation,
                           qreal ownAnchorLine, qreal offset);
    static void drawVerticalAnchor(QPainter *p, const RenderInfo &renderInfo, qreal ownAnchorLine, qreal offset);
    static void drawHorizontalAnchor(QPainter *p, const RenderInfo &renderInfo, qreal ownAnchorLine, qreal offset);
    static DrawTextInfo drawAnchorLabel(QPainter *p, const RenderInfo &renderInfo, Qt::Orientation orientation,
                           qreal ownAnchorLine, qreal offset, const QString &label, Qt::Alignment align);
    static DrawTextInfo drawHorizontalAnchorLabel(QPainter *p, const RenderInfo &renderInfo, qreal ownAnchorLine,
                                          qreal offset, const QString &label, Qt::Alignment align);
    static DrawTextInfo drawVerticalAnchorLabel(QPainter *p, const RenderInfo &renderInfo, qreal ownAnchorLine,
                                        qreal offset, const QString &label, Qt::Alignment align);

    void setIsGrabbingMode(bool isGrabbingMode);
    void windowAfterRendering();
    void drawDecorations(const QSize &size, qreal dpr);
    void updateOverlay();
    void itemParentChanged(QQuickItem *parent);
    void itemWindowChanged(QQuickWindow *window);
    void connectItemChanges(QQuickItem *item);
    void disconnectItemChanges(QQuickItem *item);
    void connectTopItemChanges(QQuickItem *item);
    void disconnectTopItemChanges(QQuickItem *item);

    QQuickWindow *m_window;
    QQuickItem *m_currentToplevelItem;
    ItemOrLayoutFacade m_currentItem;
    QuickItemGeometry m_effectiveGeometry;
    bool m_isGrabbingMode;
    bool m_drawDecorations;
};
}

#endif
