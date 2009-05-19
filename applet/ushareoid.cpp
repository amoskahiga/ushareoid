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

#include <QGraphicsGridLayout>
#include <QTreeView>
#include <QStringListModel>
#include <QHeaderView>
#include <KUrl>
#include <KDirSelectDialog>
#include <KPushButton>
#include <KTextEdit>
#include <KConfigDialog>
#include <Plasma/ToolTipManager>
#include <Plasma/TreeView>
#include <Plasma/TextEdit>
#include <Plasma/PushButton>

#include "ushareoid.h"

/**
 * Constructor.
 * @param parent Owner.
 * @param args
 */
Ushareoid::Ushareoid(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
    m_widget(0),
    m_configDialog(0),
    m_sharingStatus(NOT_SHARING)
{
    // this will get us the standard applet background, for free!
    setBackgroundHints(DefaultBackground);

    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setPopupIcon("ushareoid");

    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateStatusEdit()));
    connect(m_process, SIGNAL(started()), this, SLOT(processStarted()));
    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this,
            SLOT(processFinished(int,QProcess::ExitStatus)));
}

/**
 * Destructor.
 */
Ushareoid::~Ushareoid()
{
    if (hasFailedToLaunch()) {
        // Do some cleanup here
    } else {
        // Save settings
        KConfigGroup configGroup = globalConfig();
        configGroup.writePathEntry("defaultFolders", m_folderListModel->stringList());
    }

    delete m_process;
}

/**
 * Gets the widget that will get shown in the applet or in a Dialog, depending on the form factor of
 * the applet (it may be docked in a panel or displayed on a desktop).
 */
QGraphicsWidget* Ushareoid::graphicsWidget()
{
    if (!m_widget) {

        // Get the configuration settings
        KConfigGroup configGroup = globalConfig();
        QStringList folderList = configGroup.readPathEntry("defaultFolders", QStringList());
        m_settings.ushareExecutable = configGroup.readEntry("ushareExecutable", "ushare");
        m_settings.friendlyName = configGroup.readEntry("friendlyName", "uShareoid");
        m_settings.networkInterface = configGroup.readEntry("networkInterface", "");
        m_settings.port = configGroup.readEntry("port", 0);
        m_settings.enableXboxCompliantProfile =
                configGroup.readEntry("enableXboxCompliantProfile", true);
        m_settings.enableWebPageControl = configGroup.readEntry("enableWebPageControl", false);
        m_settings.enableTelnetControl = configGroup.readEntry("enableTelnetControl", false);

        // Set default ushare executable name if none is set.
        if(m_settings.ushareExecutable.isEmpty())
            m_settings.ushareExecutable = "ushare";

        setAspectRatioMode(Plasma::IgnoreAspectRatio);

        // Register taskbar tooltips
        Plasma::ToolTipManager::self()->registerWidget(this);

        m_widget = new QGraphicsWidget(this);
        m_layout = new QGraphicsGridLayout(m_widget);

        m_folderListModel = new QStringListModel(m_widget);
        m_folderListModel->setStringList(folderList);

        /**
         * To change the header for the m_folderListModel (Displayed in the TreeView), we would
         * have to subclass QStringListModel and reimplement setHeaderData()
         *
         * @see <http://www.qtsoftware.com/developer/task-tracker/index_html?id=188749&method=entry>

        qDebug() << m_folderListModel->setHeaderData( 0, Qt::Horizontal, "Shared Folders",
                                                      Qt::DisplayRole);
        */

        m_folderView = new Plasma::TreeView(m_widget);
        m_folderView->setModel(m_folderListModel);
        m_folderView->nativeWidget()->header()->hide();

        m_statusEdit = new Plasma::TextEdit(m_widget);
        m_statusEdit->nativeWidget()->setReadOnly(true);
        m_statusEdit->hide();

        m_addFolderButton = new Plasma::PushButton(m_widget);
        m_addFolderButton->nativeWidget()->setIcon(KIcon("list-add"));
        m_addFolderButton->setText("Add Folder...");
        connect(m_addFolderButton, SIGNAL(clicked()), this, SLOT(addFolderButtonClick()));

        m_removeFolderButton = new Plasma::PushButton(m_widget);
        m_removeFolderButton->nativeWidget()->setIcon(KIcon("list-remove"));
        m_removeFolderButton->setText("Remove Folder");
        connect(m_removeFolderButton, SIGNAL(clicked()), this, SLOT(removeFolderButtonClick()));

        m_shareButton = new Plasma::PushButton(m_widget);
        m_shareButton->nativeWidget()->setIcon(KIcon("media-playback-start"));
        m_shareButton->setText("Start Sharing");
        connect(m_shareButton, SIGNAL(clicked()), this, SLOT(shareButtonClick()));

        // Add widgets to the layout.
        m_layout->addItem(m_folderView, 0, 0, 4, 1);
        //m_layout->addItem(m_statusEdit, 3, 0);
        m_layout->addItem(m_addFolderButton, 0, 1);
        m_layout->addItem(m_removeFolderButton, 1, 1);
        m_layout->addItem(m_shareButton, 3, 1, Qt::AlignBottom);

        m_widget->setPreferredSize(500, 300);
    }

    return m_widget;
}

/**
 * Displays a file dialog allowing the user to specify the folder to add.  The selected folder is
 * added to the folder lists.
 */
void Ushareoid::addFolderButtonClick()
{
    KDirSelectDialog dialog(KUrl(), true);

    if (dialog.exec()) {
        QString path = dialog.url().path();
        if (!m_folderListModel->stringList().contains(path)) {
            QStringList list = m_folderListModel->stringList();
            list.append(path);
            list.sort();
            m_folderListModel->setStringList(list);
        }
    }
}

/**
 * Removes the currently selected folder item.
 */
void Ushareoid::removeFolderButtonClick()
{
    // Determine the selected folder item.
    QString selectedFolder = m_folderView->nativeWidget()->currentIndex().data().toString();

    // Remove the folder from the list.
    QStringList list = m_folderListModel->stringList();
    list.removeAll(selectedFolder);
    m_folderListModel->setStringList(list);
    m_statusEdit->nativeWidget()->insertPlainText(m_process->readAllStandardOutput());
}

/**
 * Starts a "ushare" process with the loaded settings.  The folders listed in the string-list view
 *  will be shared.
 */
void Ushareoid::shareButtonClick()
{
    if (m_process->state() == QProcess::Running) {

        m_process->close();
    }
    else {

        // Set the default executable name if it's not set.
        if (m_settings.ushareExecutable.isEmpty())
            m_settings.ushareExecutable = "ushare";

        // Set application/process arguments based on possible persisted settings.
        QStringList arguments;

        if (!m_settings.friendlyName.isEmpty())
            arguments << "-n" << m_settings.friendlyName;
        else    // Set the default name that shows up in the UPnP browser.
            arguments << "-n" << "uShareoid";

        if (!m_settings.networkInterface.isEmpty())
            arguments << "-i" << m_settings.networkInterface;

        if (m_settings.port)
            arguments << "-p" << QString::number(m_settings.port);

        if (m_settings.enableXboxCompliantProfile)
            arguments << "-x";

        if (!m_settings.enableTelnetControl)
            arguments << "-t";

        if (!m_settings.enableWebPageControl)
            arguments << "-w";

        foreach (QString dir, m_folderListModel->stringList()) {
            arguments << "-c" << dir;
        }

        m_process->start(m_settings.ushareExecutable, arguments);
    }
}

/**
 * Updates the Status view with the output of the ushare process.  All messages to the stdout or
 * stderr will be written to the view.
 *
 * @todo Does not currently work as intended--the ushare utlitly seems to be buffering the output
 *       thereby preventing the applet from displaying it.
 *       NOTE: Even though the output is immediately displayed when 'ushare' is run from the
 *             terminal, the C runtime library may be detecting the output device and determining
 *             the stdout buffering behaviour.  stderr is not buffered and is available immediately.
 */
void Ushareoid::updateStatusEdit()
{
    //m_statusEdit->nativeWidget()->insertPlainText(m_process->readAll());
    qDebug() << m_process->readAll();
}

/**
 * Toggles the state of the Share button to indicate that uShare is running.
 */
void Ushareoid::processStarted()
{
    m_sharingStatus = SHARING;
    m_shareButton->setText("Stop Sharing");
    m_shareButton->nativeWidget()->setIcon(KIcon("media-playback-stop"));
}

/**
 * Toggles the state of the Share button to indicate that Ushare has ended.
 */
void Ushareoid::processFinished(int exitCode, QProcess::ExitStatus)
{
    Q_UNUSED(exitCode);

    m_sharingStatus = NOT_SHARING;
    m_shareButton->setText("Start Sharing");
    m_shareButton->nativeWidget()->setIcon(KIcon("media-playback-start"));
}

/**
 * Determine and set the sharing status to be displayed in the tooltip.
 */
void Ushareoid::toolTipAboutToShow()
{
    Plasma::ToolTipContent data;
    data.setImage(KIcon("ushareoid").pixmap(IconSize(KIconLoader::Panel)));
    data.setMainText("uShare");

    if (m_sharingStatus == SHARING) {
         data.setSubText("Sharing...");
    }
    else {   // NOT_SHARING
        data.setSubText("Not Sharing...");
    }
    Plasma::ToolTipManager::self()->setContent(this, data);;
}

/**
 * Creates a configuration dialog that will be used to display and set our applet's persisted
 * settings.  The dialog is initialized with our previously persisted settings.
 */
void Ushareoid::createConfigurationInterface(KConfigDialog* parent)
{
    m_configDialog = new ConfigDialog(parent);
    m_configDialog->setSettings(m_settings);
    parent->addPage(m_configDialog, QString("uShare Settings"), icon());
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
}

/**
 * Called when the user chooses to save the changes of the applet's configuration/settings dialog.
 * It persists the settings to the applet's configuration file.
 */
void Ushareoid::configAccepted()
{
    // Get the new settings
    m_settings = m_configDialog->getSettings();

    // Save the configuration settings
    KConfigGroup configGroup = globalConfig();
    configGroup.writeEntry("ushareExecutable", m_settings.ushareExecutable);
    configGroup.writeEntry("friendlyName", m_settings.friendlyName);
    configGroup.writeEntry("networkInterface", m_settings.networkInterface);
    configGroup.writeEntry("port", m_settings.port);
    configGroup.writeEntry("enableXboxCompliantProfile", m_settings.enableXboxCompliantProfile);
    configGroup.writeEntry("enableWebPageControl", m_settings.enableWebPageControl);
    configGroup.writeEntry("enableTelnetControl", m_settings.enableTelnetControl);
}

#include "ushareoid.moc"
