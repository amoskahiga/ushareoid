/***************************************************************************
 *   Copyright (C) 2009 by Amos Kariuki <amoskahiga@hotmail.com>           *
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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "ui_configdialog.h"

/**
 * A configuration widget containing the settings for the Ushareoid applet.  This class serves as
 * a simple view for the settings.
 */
class ConfigDialog : public QWidget

{
    Q_OBJECT

    public:

        struct Settings
        {
            QString ushareExecutable;
            QString networkInterface;
            int port;
            bool enableXboxCompliantProfile;
            bool enableWebPageControl;
            bool enableTelnetControl;
         };

        ConfigDialog(QWidget* parent = 0);
        ~ConfigDialog();
        void setSettings(Settings newSettings);
        Settings getSettings();

    private:
        Ui::ConfigDialog configUi;
};

#endif // CONFIGDIALOG_H
