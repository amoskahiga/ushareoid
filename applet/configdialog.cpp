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

#include "configdialog.h"

/**
 * Constructor that build and initialize the widgets UI.
 */
ConfigDialog::ConfigDialog(QWidget *parent)
        : QWidget(parent)
{
    configUi.setupUi(this);
}

/**
 * Destructor.
 */
ConfigDialog::~ConfigDialog()
{

}

/**
 * Gets the settings currently set in the UI.
 * @return Current configuration values held by the UI elements.
 */
ConfigDialog::Settings ConfigDialog::getSettings()
{
    Settings settings;

    settings.ushareExecutable = configUi.ushareExecutableKurlrequester->text().trimmed();
    settings.friendlyName = configUi.friendlyNameLineEdit->text().trimmed();
    settings.networkInterface = configUi.networkInterfaceLineEdit->text().trimmed();
    settings.port = configUi.portSpinBox->value();
    settings.enableXboxCompliantProfile = configUi.enableXboxCompliantProfileCheckBox->isChecked();
    settings.enableTelnetControl = configUi.enableTelnetControlCheckBox->isChecked();
    settings.enableWebPageControl = configUi.enableWebPageControlCheckBox->isChecked();

    return settings;
}

/**
 * Initializes the UI elements with the given values.
 * @param newSettings New configuration to display.  The old values as discarded.
 */
void ConfigDialog::setSettings(Settings newSettings)
{
    configUi.ushareExecutableKurlrequester->setUrl(newSettings.ushareExecutable);
    configUi.friendlyNameLineEdit->setText(newSettings.friendlyName);
    configUi.networkInterfaceLineEdit->setText(newSettings.networkInterface);
    configUi.portSpinBox->setValue(newSettings.port);
    configUi.enableXboxCompliantProfileCheckBox->setChecked(newSettings.enableXboxCompliantProfile);
    configUi.enableTelnetControlCheckBox->setChecked(newSettings.enableTelnetControl);
    configUi.enableWebPageControlCheckBox->setChecked(newSettings.enableWebPageControl);
}
