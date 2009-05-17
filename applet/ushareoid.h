/***************************************************************************
 *   Copyright (C) 2009 Amos Kariuki    <amoskahiga@hotmail.com>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef USHAREOID_H
#define USHAREOID_H

#include <Plasma/PopupApplet>
#include <QProcess>

class QGraphicsGridLayout;
class QStringListModel;
class QGraphicsWidget;

namespace Plasma
{
    class PushButton;
    class TextEdit;
    class TreeView;
    class IconWidget;
}

// Define our plasma Applet
class Ushareoid : public Plasma::PopupApplet
{
    Q_OBJECT

    private:

        QGraphicsGridLayout* m_layout;
        QGraphicsWidget *m_widget;
        QStringListModel* m_folderListModel;
        QProcess* m_process;
        Plasma::TreeView* m_folderView;
        Plasma::PushButton* m_removeFolderButton;
        Plasma::PushButton* m_addFolderButton;
        Plasma::PushButton* m_shareButton;
        Plasma::TextEdit* m_statusEdit;
        Plasma::IconWidget* m_icon;
        enum {NOT_SHARING, SHARING} m_sharingStatus;

        virtual QGraphicsWidget *graphicsWidget();

    public:

        // Basic Create/Destroy
        Ushareoid(QObject *parent, const QVariantList &args);
        ~Ushareoid();

    private slots:

        void addFolderButtonClick();
        void removeFolderButtonClick();
        void shareButtonClick();
        void updateStatusEdit();
        void processStarted();
        void processFinished(int,QProcess::ExitStatus);
        void toolTipAboutToShow();

};

// This is command links the applet to the .desktop file
K_EXPORT_PLASMA_APPLET(ushareoid, Ushareoid)

#endif // USHAREOID_H
