#include <QApplication>
#include "MainWindow.h"

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtCore/private/qandroidextras_p.h>
#include <QDir>
#include <QStandardPaths>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("WLEditor");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("WLEditor");
    app.setOrganizationDomain("wleditor.com");
    
#ifdef Q_OS_ANDROID
    // Android固有の初期化
    app.setApplicationDisplayName("WLEditor");
    
    // Android用のフォントパスを設定
    QDir::addSearchPath("fonts", "assets:/fonts");
    
    // Android用のデフォルトフォントサイズを調整
    QFont defaultFont = app.font();
    defaultFont.setPointSize(12); // タッチデバイス用に少し大きめ
    app.setFont(defaultFont);
    
    // Android用のスタイルシートを設定（タッチフレンドリー）
    app.setStyleSheet(
        "QTextEdit { "
        "    font-family: 'Noto Sans Mono CJK JP', 'Droid Sans Mono', monospace; "
        "    font-size: 12pt; "
        "    line-height: 1.4; "
        "    selection-background-color: #3399ff; "
        "    selection-color: white; "
        "} "
        "QScrollBar:vertical { "
        "    width: 20px; "
        "    background-color: #f0f0f0; "
        "    border: 1px solid #d0d0d0; "
        "} "
        "QScrollBar::handle:vertical { "
        "    background-color: #808080; "
        "    min-height: 30px; "
        "    border-radius: 4px; "
        "} "
        "QScrollBar::handle:vertical:hover { "
        "    background-color: #606060; "
        "} "
        "QMenuBar::item { "
        "    padding: 8px 12px; "
        "    margin: 2px; "
        "} "
        "QMenu::item { "
        "    padding: 12px 20px; "
        "    margin: 1px; "
        "} "
        "QPushButton { "
        "    padding: 10px 16px; "
        "    min-height: 20px; "
        "} "
        "QToolBar { "
        "    spacing: 6px; "
        "    padding: 4px; "
        "} "
        "QToolBar QToolButton { "
        "    padding: 8px; "
        "    margin: 2px; "
        "    min-width: 32px; "
        "    min-height: 32px; "
        "}"
    );
#endif
    
    MainWindow window;
    window.show();
    
    return app.exec();
}