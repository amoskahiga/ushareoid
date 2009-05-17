/***************************************************************************
 *   Copyright (C) 200p Amos Kariuki    <amoskahiga@hotmail.com>           *
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
#include "ushareoid.h"

#include <QGraphicsGridLayout>
#include <QTreeView>
#include <QStringListModel>
#include <QHeaderView>
#include <QFileDialog>
#include <KPushButton>
#include <KTextEdit>
#include <Plasma/ToolTipManager>
#include <Plasma/TreeView>
#include <Plasma/TextEdit>
#include <Plasma/PushButton>

/**
 * Constructor.
 * @param parent
 * @param args
 */
Ushareoid::Ushareoid(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
    m_widget(0),
    m_sharingStatus(NOT_SHARING)
{
    // this will get us the standard applet background, for free!
    setBackgroundHints(DefaultBackground);

    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    //setPreferredSize(300, 500);
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
        configGroup.config()->sync();
    }
}

/**
 * Initializer called once the applet is loaded and added to a Corona.
 * If the applet requires a QGraphicsScene or has an particularly intensive set of initialization
 * routines to go through, consider implementing it in this method instead of the constructor.
 *
 * Note: paintInterface may get called before init() depending on initialization order. Painting is
 * managed by the canvas (QGraphisScene), and may schedule a paint event prior to init() being
 * called.
 */
QGraphicsWidget* Ushareoid::graphicsWidget()
{
    setAspectRatioMode(Plasma::IgnoreAspectRatio);

    // Register taskbar tooltips
    Plasma::ToolTipManager::self()->registerWidget(this);

    if (!m_widget) {

        m_widget = new QGraphicsWidget(this);
        m_layout = new QGraphicsGridLayout(m_widget);

        // Load the last loaded folder entries
        KConfigGroup configGroup = globalConfig();
        m_folderListModel = new QStringListModel(m_widget);
        m_folderListModel->setStringList(
                configGroup.readPathEntry("defaultFolders", QStringList()));

        m_folderView = new Plasma::TreeView(m_widget);
        m_folderView->setModel(m_folderListModel);
        m_folderView->nativeWidget()->header()->hide();        

        m_statusEdit = new Plasma::TextEdit(m_widget);
        m_statusEdit->nativeWidget()->setReadOnly(true);
        m_statusEdit->hide();

        m_addFolderButton = new Plasma::PushButton(m_widget);
        m_addFolderButton->setText("Add Folder...");
        connect(m_addFolderButton, SIGNAL(clicked()), this, SLOT(addFolderButtonClick()));

        m_removeFolderButton = new Plasma::PushButton(m_widget);
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
    QString dir = QFileDialog::getExistingDirectory(NULL, tr("Open Directory"),
                                                 "~",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    QStringList list = m_folderListModel->stringList();
    list.append(dir);
    list.removeDuplicates();
    list.sort();
    m_folderListModel->setStringList(list);

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
 * Starts a "ushare" process.  The folders listed in the string-list view will be shared.
 */
void Ushareoid::shareButtonClick()
{
    QString program = "ushare";
    QStringList arguments;
    arguments << "-x";

    foreach (QString dir, m_folderListModel->stringList()) {
        arguments << "-c" << dir;
    }

    if(m_process->state() == QProcess::Running) {
        m_process->close();
    }
    else {
        m_process->start(program, arguments);
    }

}

/**
 * Updates the Status view with the output of the ushare process.  All messages to the stdout or
 * stderr will be written to the view.
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

    if(m_sharingStatus == SHARING) {
         data.setSubText("Sharing...");
     }
    else
    {   // NOT_SHARING
        data.setSubText("Not Sharing...");
    }
    Plasma::ToolTipManager::self()->setContent(this, data);;
}

#include "ushareoid.moc"
