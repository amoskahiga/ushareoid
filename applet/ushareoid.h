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

#include "configdialog.h"

class QGraphicsGridLayout;
class QStringListModel;
class QGraphicsWidget;
class KTempDir;

namespace Plasma
{
    class PushButton;
    class TextEdit;
    class TreeView;
    class IconWidget;
    class Label;
}

/**
 * A plasmoid that provides a front-end to the popular GeeXboX uShare utility. It starts a uShare
 * instance with a list of shared folders.
 *
 * @see Hosted at ushareoid: <http://www.kde-look.org/content/show.php?content=105025>
 * @see Git repository: <http://github.com/amoskahiga/ushareoid>
 */
class Ushareoid : public Plasma::PopupApplet
{
    Q_OBJECT

    private:

        QGraphicsGridLayout* m_layout;
        QGraphicsWidget *m_widget;
        QStringListModel* m_folderListModel;
        QProcess* m_process;
        KTempDir* m_virtualFolder;
        Plasma::Label* m_sharedLabel;
        Plasma::TreeView* m_folderView;
        Plasma::PushButton* m_removeFolderButton;
        Plasma::PushButton* m_addFolderButton;
        Plasma::PushButton* m_shareButton;
        Plasma::TextEdit* m_statusEdit;
        Plasma::IconWidget* m_icon;
        ConfigDialog* m_configDialog;
        ConfigDialog::Settings m_settings;
        enum {NOT_SHARING, SHARING} m_sharingStatus;

        virtual QGraphicsWidget *graphicsWidget();
        void createConfigurationInterface(KConfigDialog* parent);
        QString createVirtualFolder(QStringList folderList);

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
        void configAccepted();

};

// This is command links the applet to the .desktop file
K_EXPORT_PLASMA_APPLET(ushareoid, Ushareoid)

#endif // USHAREOID_H
