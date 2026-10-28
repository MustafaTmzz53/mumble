// Copyright The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Themes.h"
#include "MainWindow.h"
#include "MumbleApplication.h"
#include "Settings.h"
#include "Global.h"

#ifdef Q_OS_MAC
#	include <QProcess>
#endif

#include <QStyleHints>

#include <optional>

#ifdef Q_OS_MAC
// declared in ThemeSwitch_macx.mm
extern void setLightAppearance();
extern void setDarkAppearance();
extern void resetAppearanceToSystem();
#endif

std::optional< ThemeInfo::StyleInfo > Themes::getThemeStyle(const Settings &settings, bool darkMode) {
	QString themeStyleName = darkMode ? settings.themeDarkStyleName : settings.themeStyleName;
	QString themeName      = darkMode ? settings.themeDarkName : settings.themeName;

	if (themeName.isEmpty() && themeStyleName.isEmpty()) {
		return std::nullopt;
	}

	const ThemeMap themes = getThemes();
	auto themeIt          = themes.find(themeName);
	if (themeIt == themes.end()) {
		qWarning() << "Could not find configured theme" << themeName;
		return std::nullopt;
	}

	auto styleIt = themeIt->styles.find(themeStyleName);
	if (styleIt == themeIt->styles.end()) {
		qWarning() << "Configured theme \"" << themeName << "\" does not have configured style \"" << themeStyleName
				   << "\"";
		return std::nullopt;
	}

	return *styleIt;
}

static QString getPresetStylesheet(const ThemeInfo::StyleInfo &style) {
	if (style.themeName != QLatin1String("Mumble")) {
		return QString();
	}

	if (style.name == QLatin1String("Discord Dark")) {
		return QStringLiteral(R"(
ApplicationPalette {
	qproperty-window: #313338;
	qproperty-windowtext: #dbdee1;
	qproperty-base: #1e1f22;
	qproperty-alternatebase: #2b2d31;
	qproperty-text: #dbdee1;
	qproperty-button: #2b2d31;
	qproperty-buttontext: #dbdee1;
	qproperty-highlight: #5865f2;
	qproperty-highlightedtext: #ffffff;
	qproperty-link: #00a8fc;
	qproperty-linkvisited: #00a8fc;
}
QObject { color: #dbdee1; selection-background-color: #5865f2; selection-color: #ffffff; }
QMainWindow, QDockWidget, QDialog, QWizard *, QCalendarWidget *, #qswPages > QObject { background-color: #313338; }
QToolBar, QMenuBar, QStatusBar { background-color: #313338; border: none; }
QGroupBox { background-color: #2b2d31; border: 1px solid #1e1f22; border-radius: 6px; margin-top: 8px; padding: 8px; }
QTreeView, QTreeWidget, QListView { background-color: #2b2d31; border: none; padding: 4px; outline: none; }
QTreeView::item, QTreeWidget::item, QListView::item { border: none; border-radius: 4px; padding: 5px 8px; }
QTreeView::item:hover, QTreeWidget::item:hover, QListView::item:hover { background-color: #35373c; }
QTreeView::item:selected, QTreeWidget::item:selected, QListView::item:selected { background-color: #404249; color: #ffffff; }
QPushButton, QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
	background-color: #1e1f22; color: #dbdee1; border: 1px solid #1e1f22; border-radius: 4px; padding: 5px;
}
QPushButton:hover, QComboBox:hover { background-color: #35373c; }
QPushButton:focus, QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus { border-color: #5865f2; }
QMenu { background-color: #2b2d31; border: 1px solid #1e1f22; }
QMenu::item:selected { background-color: #404249; }
QTabBar::tab { background-color: #2b2d31; border: none; border-radius: 4px; padding: 6px 10px; }
QTabBar::tab:selected { background-color: #404249; }
)");
	}

	if (style.name == QLatin1String("Discord Light")) {
		return QStringLiteral(R"(
ApplicationPalette {
	qproperty-window: #f2f3f5;
	qproperty-windowtext: #232428;
	qproperty-base: #ffffff;
	qproperty-alternatebase: #ebedef;
	qproperty-text: #232428;
	qproperty-button: #ebedef;
	qproperty-buttontext: #232428;
	qproperty-highlight: #5865f2;
	qproperty-highlightedtext: #ffffff;
	qproperty-link: #4752c4;
	qproperty-linkvisited: #4752c4;
}
QObject { color: #232428; selection-background-color: #5865f2; selection-color: #ffffff; }
QMainWindow, QDockWidget, QDialog, QWizard *, QCalendarWidget *, #qswPages > QObject { background-color: #f2f3f5; }
QToolBar, QMenuBar, QStatusBar { background-color: #f2f3f5; border: none; }
QGroupBox { background-color: #ffffff; border: 1px solid #d9dce1; border-radius: 6px; margin-top: 8px; padding: 8px; }
QTreeView, QTreeWidget, QListView { background-color: #ffffff; border: 1px solid #d9dce1; padding: 4px; outline: none; }
QTreeView::item, QTreeWidget::item, QListView::item { border: none; border-radius: 4px; padding: 5px 8px; }
QTreeView::item:hover, QTreeWidget::item:hover, QListView::item:hover { background-color: #ebedef; }
QTreeView::item:selected, QTreeWidget::item:selected, QListView::item:selected { background-color: #5865f2; color: #ffffff; }
QPushButton, QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
	background-color: #ffffff; color: #232428; border: 1px solid #d9dce1; border-radius: 4px; padding: 5px;
}
QPushButton:hover, QComboBox:hover { background-color: #ebedef; }
QPushButton:focus, QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus { border-color: #5865f2; }
QMenu { background-color: #ffffff; border: 1px solid #d9dce1; }
QMenu::item:selected { background-color: #ebedef; }
QTabBar::tab { background-color: #ebedef; border: none; border-radius: 4px; padding: 6px 10px; }
QTabBar::tab:selected { background-color: #ffffff; }
)");
	}

	if (style.name == QLatin1String("Midnight AMOLED")) {
		return QStringLiteral(R"(
ApplicationPalette {
	qproperty-window: #000000;
	qproperty-windowtext: #e6e6e6;
	qproperty-base: #000000;
	qproperty-alternatebase: #0a0a0a;
	qproperty-text: #e6e6e6;
	qproperty-button: #111111;
	qproperty-buttontext: #e6e6e6;
	qproperty-highlight: #5865f2;
	qproperty-highlightedtext: #ffffff;
}
QObject { color: #e6e6e6; selection-background-color: #5865f2; selection-color: #ffffff; }
QMainWindow, QDockWidget, QDialog, QWizard *, QCalendarWidget *, #qswPages > QObject { background-color: #000000; }
QToolBar, QMenuBar, QStatusBar { background-color: #000000; border: none; }
QGroupBox { background-color: #080808; border: 1px solid #1c1c1c; border-radius: 6px; margin-top: 8px; padding: 8px; }
QTreeView, QTreeWidget, QListView { background-color: #050505; border: 1px solid #151515; padding: 4px; outline: none; }
QTreeView::item, QTreeWidget::item, QListView::item { border: none; border-radius: 4px; padding: 5px 8px; }
QTreeView::item:hover, QTreeWidget::item:hover, QListView::item:hover { background-color: #161616; }
QTreeView::item:selected, QTreeWidget::item:selected, QListView::item:selected { background-color: #252525; color: #ffffff; }
QPushButton, QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
	background-color: #0a0a0a; color: #e6e6e6; border: 1px solid #202020; border-radius: 4px; padding: 5px;
}
QPushButton:hover, QComboBox:hover { background-color: #1b1b1b; }
QPushButton:focus, QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus { border-color: #5865f2; }
QMenu { background-color: #080808; border: 1px solid #202020; }
QMenu::item:selected { background-color: #252525; }
QTabBar::tab { background-color: #0a0a0a; border: none; border-radius: 4px; padding: 6px 10px; }
QTabBar::tab:selected { background-color: #202020; }
)");
	}

	if (style.name == QLatin1String("Neomorphic")) {
		return QStringLiteral(R"(
ApplicationPalette {
	qproperty-window: #e0e5ec;
	qproperty-windowtext: #394150;
	qproperty-base: #e0e5ec;
	qproperty-alternatebase: #e7ebf1;
	qproperty-text: #394150;
	qproperty-button: #e0e5ec;
	qproperty-buttontext: #394150;
	qproperty-highlight: #667eea;
	qproperty-highlightedtext: #ffffff;
}
QObject { color: #394150; selection-background-color: #667eea; selection-color: #ffffff; }
QMainWindow, QDockWidget, QDialog, QWizard *, QCalendarWidget *, #qswPages > QObject { background-color: #e0e5ec; }
QToolBar, QMenuBar, QStatusBar { background-color: #e0e5ec; border: none; }
QGroupBox { background-color: #e0e5ec; border: 1px solid #cbd2dc; border-radius: 10px; margin-top: 8px; padding: 8px; }
QTreeView, QTreeWidget, QListView { background-color: #e0e5ec; border: 1px solid #cbd2dc; border-radius: 8px; padding: 4px; outline: none; }
QTreeView::item, QTreeWidget::item, QListView::item { border: none; border-radius: 6px; padding: 5px 8px; }
QTreeView::item:hover, QTreeWidget::item:hover, QListView::item:hover { background-color: #d3dae5; }
QTreeView::item:selected, QTreeWidget::item:selected, QListView::item:selected { background-color: #667eea; color: #ffffff; }
QPushButton, QComboBox, QLineEdit, QSpinBox, QDoubleSpinBox {
	background-color: #e0e5ec; color: #394150; border: 1px solid #cbd2dc; border-radius: 8px; padding: 5px;
}
QPushButton:hover, QComboBox:hover { background-color: #d3dae5; }
QPushButton:focus, QComboBox:focus, QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus { border-color: #667eea; }
QMenu { background-color: #e0e5ec; border: 1px solid #cbd2dc; border-radius: 8px; }
QMenu::item:selected { background-color: #d3dae5; }
QTabBar::tab { background-color: #d3dae5; border: none; border-radius: 6px; padding: 6px 10px; }
QTabBar::tab:selected { background-color: #e0e5ec; }
)");
	}

	return QString();
}

std::optional< ThemeInfo::StyleInfo > Themes::getConfiguredStyle(const Settings &settings) {
	switch (settings.styleType) {
		case StyleType::Light:
			return getThemeStyle(settings, false);
		case StyleType::Dark:
			return getThemeStyle(settings, true);
		case StyleType::Auto:
			return getThemeStyle(settings, detectSystemDarkTheme());
	}
	assert(false);
	return std::nullopt; // fallback (should not normally reach here)
}

bool Themes::setConfiguredStyleHelper(QString &themeName, QString &themeStyleName,
									  std::optional< ThemeInfo::StyleInfo > style) {
	if (style) {
		if (themeName != style->themeName || themeStyleName != style->name) {
			themeName      = style->themeName;
			themeStyleName = style->name;
			return true;
		}
	} else {
		if (!themeName.isEmpty() || !themeStyleName.isEmpty()) {
			themeName = themeStyleName = QString();
			return true;
		}
	}
	return false;
}

bool Themes::setConfiguredStyle(Settings &settings, std::optional< ThemeInfo::StyleInfo > style) {
	return setConfiguredStyleHelper(settings.themeName, settings.themeStyleName, style);
}

bool Themes::setConfiguredDarkStyle(Settings &settings, std::optional< ThemeInfo::StyleInfo > style) {
	return setConfiguredStyleHelper(settings.themeDarkName, settings.themeDarkStyleName, style);
}

void Themes::applyFallback() {
	qWarning() << "Applying fallback style sheet";

	QStringList skinPaths;
	skinPaths << QLatin1String(":/themes/Default");
	QString defaultTheme = getDefaultStylesheet();
	setTheme(defaultTheme, skinPaths);
}

bool Themes::applyConfigured() {
	std::optional< ThemeInfo::StyleInfo > style = Themes::getConfiguredStyle(Global::get().s);
	if (!style) {
		return false;
	}

	const QFileInfo qssFile(style->getPlatformQss());

	qWarning() << "Theme:" << style->themeName;
	qWarning() << "Style:" << style->name;
	qWarning() << "--> qss:" << qssFile.absoluteFilePath();

#ifdef Q_OS_MAC
	switch (Global::get().s.styleType) {
		case StyleType::Light:
			setLightAppearance();
			break;
		case StyleType::Dark:
			setDarkAppearance();
			break;
		case StyleType::Auto:
			resetAppearanceToSystem();
			break;
	}
#endif

	QFile file(qssFile.absoluteFilePath());
	if (!file.open(QFile::ReadOnly)) {
		qWarning() << "Failed to open theme stylesheet:" << file.errorString();
		return false;
	}

	QStringList skinPaths;
	skinPaths << qssFile.path();
	skinPaths << QLatin1String(":/themes/Default"); // Some skins might want to fall-back on our built-in resources

	QString themeQss = QString::fromUtf8(file.readAll());
	themeQss += getPresetStylesheet(*style);
	setTheme(themeQss, skinPaths);
	return true;
}

void Themes::setTheme(QString &themeQss, QStringList &skinPaths) {
	QDir::setSearchPaths(QLatin1String("skin"), skinPaths);

	QString userStylesheetFn = userStylesheetPath();
	QString userStylesheetContent;
	if (readStylesheet(userStylesheetFn, userStylesheetContent)) {
		qWarning("Themes: allowing user stylesheet at '%s' to override the stylesheet", qPrintable(userStylesheetFn));
	}

	qApp->setStyleSheet(themeQss + QLatin1String("\n") + userStylesheetContent);
}

bool Themes::apply() {
	const bool result = applyConfigured();
	if (!result) {
		applyFallback();
	}

	if (Global::get().mw) {
		Global::get().mw->qteLog->document()->setDefaultStyleSheet(qApp->styleSheet());
	}
	return result;
}

bool Themes::detectSystemDarkTheme() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
	Qt::ColorScheme colorScheme = QGuiApplication::styleHints()->colorScheme();
	if (colorScheme != Qt::ColorScheme::Unknown) {
		return colorScheme == Qt::ColorScheme::Dark;
	}
	// colorScheme() can return Unknown on some platforms (e.g. wlroots compositors),
	// so fall back to comparing palette lightness values
	QPalette defaultPalette;
	return defaultPalette.color(QPalette::WindowText).lightness() > defaultPalette.color(QPalette::Window).lightness();
#else
#	if defined(Q_OS_WIN)
	QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
					   QSettings::NativeFormat);
	return settings.value("AppsUseLightTheme", 1).toInt() == 0; // 0 means dark mode
#	elif defined(Q_OS_MAC)
	QProcess process;
	process.start("defaults", { "read", "-g", "AppleInterfaceStyle" });
	process.waitForFinished(100);
	QString output = process.readAllStandardOutput().trimmed();
	// If no output, assume Light Mode
	return output == "Dark";
#	else
	// Fallback for other OSes
	QByteArray platform = qgetenv("QT_QPA_PLATFORM");
	if (platform.contains("darkmode=2")) {
		return true;
	} else if (platform.contains("darkmode=1")) {
		QPalette defaultPalette;
		return defaultPalette.color(QPalette::WindowText).lightness()
			   > defaultPalette.color(QPalette::Window).lightness();
	}
	return false;
#	endif
#endif
}

ThemeMap Themes::getThemes() {
	return ThemeInfo::scanDirectories(getSearchDirectories());
}

QDir Themes::getUserThemesDirectory() {
	return QDir(Global::get().qdBasePath.absolutePath() + QLatin1String("/Themes"));
}

QVector< QDir > Themes::getSearchDirectories() {
	QVector< QDir > themeSearchDirectories;

	// Built-in themes contained in the binary have the lowest priority
	themeSearchDirectories << QDir(QLatin1String(":themes"));
	// Next come themes found in the applications Themes directory
	themeSearchDirectories << QDir(MumbleApplication::instance()->applicationVersionRootPath()
								   + QLatin1String("/Themes"));
	// Highest priority have themes located in the user directory
	themeSearchDirectories << getUserThemesDirectory();

	return themeSearchDirectories;
}

QString Themes::userStylesheetPath() {
	return Global::get().qdBasePath.absolutePath() + QLatin1String("/user.qss");
}

bool Themes::readStylesheet(const QString &stylesheetFn, QString &stylesheetContent) {
	QFile file(stylesheetFn);
	if (!file.open(QFile::ReadOnly)) {
		stylesheetContent = QString();
		return false;
	}

	stylesheetContent = QString::fromUtf8(file.readAll());
	return true;
}

QString Themes::getDefaultStylesheet() {
	return QLatin1String(".log-channel{text-decoration:none;}.log-user{text-decoration:none;}p{margin:0;}#qwMacWarning,"
						 "#qwInlineNotice{background-color:#FFFEDC;border-radius:5px;border:1px solid #B5B59E;}"
						 "#qwMacWarning > QLabel,#qwInlineNotice > QLabel{color:#333;}");
}
