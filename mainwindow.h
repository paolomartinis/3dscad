#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "scenecontroller.h"
#include "theme.h"

#include <QMainWindow>

class QByteArray;
class QCloseEvent;
class QLabel;
class QMenu;
class QSettings;
class AnimatedTitleBar;
class CodeEditorPanel;
class ExampleBrowserMenu;
class SceneTreeGraphicsWidget;
class ViewportWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    // Opens a .scad file as the current document (used for File > Open and
    // for a path passed on the command line / via file association).
    bool openScadFile(const QString &filePath);

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void addCube();
    void addSphere();
    void addCylinder();
    void addCone();
    void addUnionGroup();
    void addDifferenceGroup();
    void addIntersectionGroup();
    void loadExample(const QString &filePath);
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();

private:
    void buildUi();
    void buildHardwareMenu(QMenu *menu);
    void insertHardware(const QString &moduleName, const QString &call);
    bool loadScadIntoEditor(const QString &filePath, const QString &dialogTitle);
    bool writeScadFile(const QString &filePath);
    bool maybeSave();
    void updateWindowTitle();
    void rebuildRecentFilesMenu();
    void addRecentFile(const QString &filePath);
    void refreshShapeList();
    void refreshProperties();
    void refreshOpenScadCode();
    void refreshCsgStatus();
    void refreshSceneViews();
    void onSelectionChanged(int nodeId);
    void highlightOpenScadSelection();
    void clearSelection();
    void saveAppearanceSettings();
    void rebuildSavedThemeMenus();
    void openThemeEditor();
    void applySavedWindowTheme(const QString &name);
    void applySavedTreeTheme(const QString &name);
    void applySavedViewportTheme(const QString &name);

private:
    SceneController *m_controller = nullptr;
    QSettings       *m_settings = nullptr;
    QString          m_applicationThemeId;
    QString          m_customWindowThemeName;
    QString          m_customTreeThemeName;
    QString          m_customViewportThemeName;
    ThemeSpec        m_activeWindowTheme;
    AnimatedTitleBar *m_titleBar = nullptr;
    QMenu            *m_savedWindowThemeMenu = nullptr;
    QMenu            *m_savedTreeThemeMenu = nullptr;
    QMenu            *m_savedViewportThemeMenu = nullptr;
    QMenu            *m_recentFilesMenu = nullptr;
    QString           m_currentFilePath;
    bool              m_backupWritten = false;

    ExampleBrowserMenu      *m_exampleBrowser          = nullptr;
    ExampleBrowserMenu      *m_exampleBrowserTutorials = nullptr;
    ExampleBrowserMenu      *m_exampleBrowserTests     = nullptr;
    ViewportWidget          *m_viewport          = nullptr;
    SceneTreeGraphicsWidget *m_sceneTreeGraphics  = nullptr;
    CodeEditorPanel         *m_codeEditorPanel    = nullptr;
    QLabel                  *m_csgStatusLabel     = nullptr;
};

#endif
