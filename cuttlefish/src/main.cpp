/*
    SPDX-FileCopyrightText: 2014 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Qt
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>

// Frameworks
#include <KConfigGroup>
#include <KLocalizedString>
#include <KPackage/PackageLoader>
#include <Plasma/Theme>

// Own
#include "colorschemes.h"
#include "iconmodel.h"
#include "sortfiltermodel.h"

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if ((msg.contains("qt.svg") && msg.contains("Could not resolve property: #linearGradient")) || msg.contains("Could not resolve property: #pattern")) {
        return;
    }

    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "\u001b[33mWarning\u001b[0m: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "\u001b[33;1mCritical\u001b[0m: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "\u001b[31;1mFatal\u001b[0m: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }
}

int main(int argc, char **argv)
{
    qInstallMessageHandler(messageOutput);

    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("cuttlefish");

    app.setApplicationVersion(PROJECT_VERSION);
    app.setDesktopFileName(QStringLiteral("org.kde.cuttlefish"));

    app.setOrganizationName("KDE");
    app.setOrganizationDomain("org.kde");
    app.setApplicationName("Cuttlefish");

    const static auto _category = QStringLiteral("category");
    QCommandLineOption category = QCommandLineOption(QStringList() << QStringLiteral("c") << _category, i18n("Start with category"), i18n("category"));

    const static auto _f = QStringLiteral("fullscreen");
    QCommandLineOption fullscreen = QCommandLineOption(QStringList() << QStringLiteral("f") << _f, i18n("Start full-screen"));

    const static auto _p = QStringLiteral("picker");
    QCommandLineOption picker = QCommandLineOption(QStringList() << QStringLiteral("p") << _p, i18n("Run in icon-picker mode"));

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.setApplicationDescription("Cuttlefish Icon Browser");
    parser.addHelpOption();
    parser.addOption(category);
    parser.addOption(fullscreen);
    parser.addOption(picker);

    parser.process(app);

    QString _cc = parser.value(category);

    QQmlApplicationEngine engine;

    auto l10nContext = new KLocalizedContext(&engine);
    l10nContext->setTranslationDomain(QStringLiteral("cuttlefish"));
    engine.rootContext()->setContextObject(l10nContext);

    auto package = KPackage::PackageLoader::self()->loadPackage("Plasma/Generic");
    package.setPath("org.kde.plasma.cuttlefish");

    if (!package.isValid() || !package.metadata().isValid()) {
        qWarning() << "Could not load package org.kde.plasma.cuttlefish:" << package.path();
        return -1;
    }

    auto iconModel = new CuttleFish::IconModel(engine.rootContext());
    auto proxyModel = new CuttleFish::SortFilterModel(engine.rootContext());
    proxyModel->setSourceModel(iconModel);
    proxyModel->sort(0);
    auto colorSchemes = new CuttleFish::ColorSchemes(engine.rootContext());

    engine.rootContext()->setContextProperty("iconModel", iconModel);
    engine.rootContext()->setContextProperty("proxyModel", proxyModel);
    engine.rootContext()->setContextProperty("pickerMode", parser.isSet("picker"));
    engine.rootContext()->setContextProperty("colorSchemes", colorSchemes);

    engine.load(QUrl::fromLocalFile(package.filePath("mainscript")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    qmlRegisterAnonymousType<CuttleFish::IconModel>("org.kde.plasma.sdk", 1);
    qmlRegisterAnonymousType<CuttleFish::ColorSchemes>("org.kde.plasma.sdk", 1);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    return app.exec();
}
