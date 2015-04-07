/*
    Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "plasmaprojectmanager.h"
#include "plasmaitem.h"

#include <KPluginLoader>
#include <KPluginFactory>
#include <KAboutData>

K_PLUGIN_FACTORY_WITH_JSON(PlasmaSupportFactory, "kdevplasmamanager.json", registerPlugin<PlasmaProjectManager>();)

PlasmaProjectManager::PlasmaProjectManager(QObject* parent, const QVariantList &args)
    : AbstractFileManagerPlugin("kdevplasmamanager", parent, args)
{
}

PlasmaProjectManager::~PlasmaProjectManager()
{
}

KDevelop::ProjectFileItem* PlasmaProjectManager::createFileItem(IProject *project, const Path &path,
                                                                ProjectBaseItem *parent)
{
    PlasmaFileItem *fileItem = new PlasmaFileItem(project, path, parent);
    return fileItem;
}

KDevelop::ProjectFolderItem* PlasmaProjectManager::createFolderItem(IProject *project, const Path &path,
                                                                    ProjectBaseItem *parent)
{
    PlasmaFolderItem *folderItem = new PlasmaFolderItem(project, path, parent);
    return folderItem;
}

#include "plasmaprojectmanager.moc"
