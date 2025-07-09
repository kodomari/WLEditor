#include "MainWindow.h"
#include <QTextCursor>
#include <QFileInfo>
#include <QFontDialog>
#include <QInputDialog>
#include <QToolBar>
#include <QRegularExpression>
#include <QKeyEvent>
#include <QScrollBar>
#include <QTimer>
#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QPaintEvent>
#include <QProcess>

// CustomTextEdit実装
CustomTextEdit::CustomTextEdit(QWidget *parent)
    : QTextEdit(parent)
    , wrapCharacters(80)
    , useCharacterWrap(true)
    , waitingForCtrlQ(false)
    , waitingForCtrlK(false)
    , blockMode(false)
    , currentClipboardIndex(0)
{
    updateWrapWidth();
    setAcceptRichText(false);
    
    // リセットタイマーの設定
    resetTimer = new QTimer(this);
    resetTimer->setSingleShot(true);
    resetTimer->setInterval(3000);
    connect(resetTimer, &QTimer::timeout, this, &CustomTextEdit::resetTwoKeyMode);
}

void CustomTextEdit::setWrapWidth(int characters)
{
    wrapCharacters = characters;
    useCharacterWrap = (characters > 0);
    updateWrapWidth();
}

void CustomTextEdit::updateWrapWidth()
{
    if (!useCharacterWrap || wrapCharacters <= 0) {
        setLineWrapMode(QTextEdit::NoWrap);
    } else {
        setLineWrapMode(QTextEdit::FixedColumnWidth);
        setLineWrapColumnOrWidth(wrapCharacters);
    }
}

void CustomTextEdit::resizeEvent(QResizeEvent *event)
{
    QTextEdit::resizeEvent(event);
    updateWrapWidth();
}

void CustomTextEdit::keyPressEvent(QKeyEvent *event)
{
    // 🔧 一番最初に追加
    qDebug() << "keyPressEvent: key=" << event->key() << "modifiers=" << event->modifiers();
    
    // 🔧 Ctrl+キーの詳細ログ
    if (event->modifiers() == Qt::ControlModifier) {
        qDebug() << "=== CTRL KEY PRESSED ===";
        qDebug() << "Key code:" << event->key() << "(" << QChar(event->key()).toLatin1() << ")";
        qDebug() << "waitingForCtrlQ:" << waitingForCtrlQ;
        qDebug() << "waitingForCtrlK:" << waitingForCtrlK;
    }

    // ESCキーでブロックモードキャンセル
    if (event->key() == Qt::Key_Escape) {
        if (blockMode) {
            blockMode = false;
            update();
            QTextCursor cursor = textCursor();
            cursor.clearSelection();
            setTextCursor(cursor);
            return;
        } else if (textCursor().hasSelection()) {
            QTextCursor cursor = textCursor();
            cursor.clearSelection();
            setTextCursor(cursor);
            return;
        } else if (waitingForCtrlQ || waitingForCtrlK) {
            resetTwoKeyMode();
            return;
        }
    }
    
    // 2段階キーバインドの処理
    if (waitingForCtrlQ) {
        MainWindow *mainWindow = qobject_cast<MainWindow*>(window());
        if (mainWindow) {
            mainWindow->statusBar()->showMessage("Processing Ctrl+Q command...", 1000);
        }
        handleCtrlQ(event);
        return;
    }
    if (waitingForCtrlK) {
        MainWindow *mainWindow = qobject_cast<MainWindow*>(window());
        if (mainWindow) {
            mainWindow->statusBar()->showMessage("Processing Ctrl+K command...", 1000);
        }
        handleCtrlK(event);
        return;
    }
    
    // Ctrl修飾子が押されている場合のWordStarキーバインド
    if (event->modifiers() == Qt::ControlModifier) {
        qDebug() << "=== ENTERING CTRL SWITCH ===";  // 🔧 追加
        qDebug() << "About to check key:" << event->key();  // 🔧 追加
        switch (event->key()) {
        case Qt::Key_L: // Ctrl+L - 最後の検索を繰り返し
            {
                qDebug() << "Processing Ctrl+L";
                MainWindow *mainWindow = qobject_cast<MainWindow*>(window());
                if (mainWindow) {
                    mainWindow->wordstarFindNext();
                }
            }
            return;
        case Qt::Key_E: // 上へ
            moveCursor(QTextCursor::Up);
            if (blockMode) updateBlockSelection();
            return;
        case Qt::Key_S: // 左へ
            moveCursor(QTextCursor::Left);
            if (blockMode) updateBlockSelection();
            return;
        case Qt::Key_D: // 右へ
            moveCursor(QTextCursor::Right);
            if (blockMode) updateBlockSelection();
            return;
        case Qt::Key_X: // 下へ
            moveCursor(QTextCursor::Down);
            if (blockMode) updateBlockSelection();
            return;
        case Qt::Key_R: // ページアップ
            {
                QFontMetrics fm(font());
                int lineHeight = fm.height();
                int visibleLines = viewport()->height() / lineHeight;
                
                QTextCursor cursor = textCursor();
                for (int i = 0; i < visibleLines - 1; i++) {
                    if (!cursor.movePosition(QTextCursor::Up)) break;
                }
                setTextCursor(cursor);
                ensureCursorVisible();
            }
            return;
        case Qt::Key_C: // ページダウン
            {
                QFontMetrics fm(font());
                int lineHeight = fm.height();
                int visibleLines = viewport()->height() / lineHeight;
                
                QTextCursor cursor = textCursor();
                for (int i = 0; i < visibleLines - 1; i++) {
                    if (!cursor.movePosition(QTextCursor::Down)) break;
                }
                setTextCursor(cursor);
                ensureCursorVisible();
            }
            return;
        case Qt::Key_G: // 右の文字を削除
            {
                QTextCursor cursor = textCursor();
                cursor.deleteChar();
            }
            return;
        case Qt::Key_H: // 左の文字を削除
            {
                QTextCursor cursor = textCursor();
                cursor.deletePreviousChar();
            }
            return;
        case Qt::Key_T: // 単語の右部分を削除
            {
                QTextCursor cursor = textCursor();
                cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
            }
            return;
        case Qt::Key_Y: // 行全体削除
            {
                QTextCursor cursor = textCursor();
                cursor.select(QTextCursor::LineUnderCursor);
                cursor.removeSelectedText();
                cursor.deleteChar();
            }
            return;
        case Qt::Key_A: // 単語の左へ
            moveCursor(QTextCursor::PreviousWord);
            return;
        case Qt::Key_F: // 単語の右へ
            moveCursor(QTextCursor::NextWord);
            return;
       case Qt::Key_Q: // Ctrl+Q系コマンドの開始
            waitingForCtrlQ = true;
            resetTimer->start();
            {
                MainWindow *mainWindow = qobject_cast<MainWindow*>(window());
                if (mainWindow) {
                    mainWindow->statusBar()->showMessage("Ctrl+Q pressed, waiting for second key...", 3000);
                }
            }
            return;
        case Qt::Key_K: // Ctrl+K系コマンドの開始
            {
    qDebug() << "*** CTRL+K CASE REACHED ***";
    qDebug() << "Before setting waitingForCtrlK";  // 🔧 追加
    waitingForCtrlK = true;
    waitingForCtrlQ = false;
    resetTimer->start();
    qDebug() << "After setting waitingForCtrlK to:" << waitingForCtrlK;  // 🔧 追加
    
    MainWindow *mainWindow = qobject_cast<MainWindow*>(window());
    if (mainWindow) {
        mainWindow->statusBar()->showMessage("Ctrl+K pressed, waiting for second key...", 3000);
    }
    qDebug() << "About to return from Ctrl+K case";  // 🔧 追加
    return;

}
        }
    }
    
    // 通常のキー処理
    QTextEdit::keyPressEvent(event);
}

void CustomTextEdit::handleCtrlQ(QKeyEvent *event)
{
    resetTwoKeyMode();
    
    // 🔧 修正: Ctrl修飾子の有無に関わらず処理
    if (event->modifiers() == Qt::ControlModifier || event->modifiers() == Qt::NoModifier) {
        switch (event->key()) {
        case Qt::Key_F: // Ctrl+Q, F または Ctrl+Q, Ctrl+F - 検索ダイアログ
            {
                MainWindow *mainWindow = qobject_cast<MainWindow*>(window());
                if (mainWindow) {
                    mainWindow->wordstarFind();
                }
            }
            break;
        case Qt::Key_A: // Ctrl+Q, A または Ctrl+Q, Ctrl+A - 置換ダイアログ  
            {
                MainWindow *mainWindow = qobject_cast<MainWindow*>(window());
                if (mainWindow) {
                    mainWindow->wordstarReplace();
                }
            }
            break;
        case Qt::Key_R: // Ctrl+Q, R または Ctrl+Q, Ctrl+R - ファイル先頭へ
            {
                QTextCursor cursor = textCursor();
                cursor.movePosition(QTextCursor::Start);
                setTextCursor(cursor);
                ensureCursorVisible();
            }
            break;
        case Qt::Key_C: // Ctrl+Q, C または Ctrl+Q, Ctrl+C - ファイル末尾へ
            {
                QTextCursor cursor = textCursor();
                cursor.movePosition(QTextCursor::End);
                setTextCursor(cursor);
                ensureCursorVisible();
            }
            break;
        case Qt::Key_S: // Ctrl+Q, S または Ctrl+Q, Ctrl+S - 行頭へ
            {
                QTextCursor cursor = textCursor();
                cursor.movePosition(QTextCursor::StartOfLine);
                setTextCursor(cursor);
            }
            break;
        case Qt::Key_D: // Ctrl+Q, D または Ctrl+Q, Ctrl+D - 行末へ
            {
                QTextCursor cursor = textCursor();
                cursor.movePosition(QTextCursor::EndOfLine);
                setTextCursor(cursor);
            }
            break;
        case Qt::Key_E: // Ctrl+Q, E または Ctrl+Q, Ctrl+E - 画面上端へ
            {
                QTextCursor cursor = cursorForPosition(QPoint(0, 0));
                setTextCursor(cursor);
                ensureCursorVisible();
            }
            break;
        case Qt::Key_X: // Ctrl+Q, X または Ctrl+Q, Ctrl+X - 画面下端へ
            {
                QTextCursor cursor = cursorForPosition(QPoint(0, viewport()->height()));
                setTextCursor(cursor);
                ensureCursorVisible();
            }
            break;
        }
    }
}

void CustomTextEdit::handleCtrlK(QKeyEvent *event)
{
    resetTwoKeyMode();
    
    // 🔧 修正: Ctrl修飾子の有無に関わらず処理
    if (event->modifiers() == Qt::ControlModifier || event->modifiers() == Qt::NoModifier) {
        switch (event->key()) {
        case Qt::Key_B: // Ctrl+K, B または Ctrl+K, Ctrl+B - 選択開始
            qDebug() << "Starting block selection at position:" << textCursor().position();
            blockStartCursor = textCursor();
            blockMode = true;
            update();
            break;
            
        case Qt::Key_K: // Ctrl+K, K または Ctrl+K, Ctrl+K - 選択終了＋コピー
            qDebug() << "Processing Ctrl+K+K - Copy and end selection";
            if (blockMode) {
                QTextCursor currentCursor = textCursor();
                
                // 選択範囲を決定
                int startPos = qMin(blockStartCursor.position(), currentCursor.position());
                int endPos = qMax(blockStartCursor.position(), currentCursor.position());
                
                qDebug() << "Block selection from" << startPos << "to" << endPos;
                
                // 選択範囲のテキストを取得
                QTextCursor selectionCursor = blockStartCursor;
                selectionCursor.setPosition(startPos);
                selectionCursor.setPosition(endPos, QTextCursor::KeepAnchor);
                
                QString selectedText = selectionCursor.selectedText();
                qDebug() << "Selected text length:" << selectedText.length();
                
                if (!selectedText.isEmpty()) {
                    // クリップボードにコピー
                    QApplication::clipboard()->setText(selectedText);
                    
                    // クリップボード履歴に追加
                    clipboardHistory.prepend(selectedText);
                    if (clipboardHistory.size() > 10) {
                        clipboardHistory.removeLast();
                    }
                    currentClipboardIndex = 0;
                    
                    qDebug() << "Text copied to clipboard:" << selectedText.left(50) + "...";
                }
                
                // ブロックモード終了
                blockMode = false;
                
                // 選択解除
                QTextCursor cursor = textCursor();
                cursor.clearSelection();
                setTextCursor(cursor);
                
                // 画面更新
                update();
                
                qDebug() << "Block mode ended, selection cleared";
                
                // ステータス表示
                MainWindow *mainWindow = qobject_cast<MainWindow*>(window());
                if (mainWindow) {
                    mainWindow->statusBar()->showMessage("Block copied to clipboard", 2000);
                }
            } else {
                qDebug() << "Ctrl+K+K pressed but not in block mode";
                // ブロックモードでない場合の通常のコピー処理
                if (textCursor().hasSelection()) {
                    copy();
                    QTextCursor cursor = textCursor();
                    cursor.clearSelection();
                    setTextCursor(cursor);
                }
            }
            break;
            
        case Qt::Key_C: // Ctrl+K, C または Ctrl+K, Ctrl+C - ペースト
            if (!clipboardHistory.isEmpty()) {
                QString textToPaste = clipboardHistory.at(currentClipboardIndex);
                insertPlainText(textToPaste);
                qDebug() << "Pasted from history index:" << currentClipboardIndex;
            } else {
                paste();
            }
            break;
            
        case Qt::Key_V: // Ctrl+K, V または Ctrl+K, Ctrl+V - ペースト＆履歴を戻す
            if (!clipboardHistory.isEmpty()) {
                QString textToPaste = clipboardHistory.at(currentClipboardIndex);
                insertPlainText(textToPaste);
                
                // 履歴インデックスを進める
                currentClipboardIndex = (currentClipboardIndex + 1) % clipboardHistory.size();
                qDebug() << "Pasted and moved to history index:" << currentClipboardIndex;
            } else {
                paste();
            }
            break;
            
        case Qt::Key_Y: // Ctrl+K, Y または Ctrl+K, Ctrl+Y - 選択部分をカット
            qDebug() << "Processing Ctrl+K+Y - Cut and end selection";
            if (blockMode) {
                QTextCursor currentCursor = textCursor();
                
                // 選択範囲を決定
                int startPos = qMin(blockStartCursor.position(), currentCursor.position());
                int endPos = qMax(blockStartCursor.position(), currentCursor.position());
                
                // 選択範囲のテキストを取得してカット
                QTextCursor selectionCursor = blockStartCursor;
                selectionCursor.setPosition(startPos);
                selectionCursor.setPosition(endPos, QTextCursor::KeepAnchor);
                
                QString selectedText = selectionCursor.selectedText();
                
                if (!selectedText.isEmpty()) {
                    // クリップボードにコピー
                    QApplication::clipboard()->setText(selectedText);
                    
                    // クリップボード履歴に追加
                    clipboardHistory.prepend(selectedText);
                    if (clipboardHistory.size() > 10) {
                        clipboardHistory.removeLast();
                    }
                    currentClipboardIndex = 0;
                    
                    // テキストを削除
                    selectionCursor.removeSelectedText();
                    
                    qDebug() << "Text cut to clipboard:" << selectedText.left(50) + "...";
                }
                
                // ブロックモード終了
                blockMode = false;
                update();
                
                // ステータス表示
                MainWindow *mainWindow = qobject_cast<MainWindow*>(window());
                if (mainWindow) {
                    mainWindow->statusBar()->showMessage("Block cut to clipboard", 2000);
                }
            } else {
                // ブロックモードでない場合の通常のカット処理
                if (textCursor().hasSelection()) {
                    cut();
                }
            }
            break;
        }
    }
}
void CustomTextEdit::resetTwoKeyMode()
{
    waitingForCtrlQ = false;
    waitingForCtrlK = false;
    resetTimer->stop();
}

void CustomTextEdit::paintEvent(QPaintEvent *event)
{
    QTextEdit::paintEvent(event);
    
    if (blockMode) {
        QPainter painter(viewport());
        
        QTextCursor currentCursor = textCursor();
        QTextCursor selectionCursor = blockStartCursor;
        
        int startPos = qMin(blockStartCursor.position(), currentCursor.position());
        int endPos = qMax(blockStartCursor.position(), currentCursor.position());
        
        selectionCursor.setPosition(startPos);
        selectionCursor.setPosition(endPos, QTextCursor::KeepAnchor);
        
        if (selectionCursor.hasSelection()) {
            QList<QTextEdit::ExtraSelection> extraSelections;
            QTextEdit::ExtraSelection selection;
            
            QColor lineColor = QColor(Qt::blue).lighter(160);
            selection.format.setBackground(lineColor);
            selection.cursor = selectionCursor;
            extraSelections.append(selection);
            
            setExtraSelections(extraSelections);
        }
        
        painter.setPen(QPen(Qt::blue, 2, Qt::DashLine));
        QRect startRect = cursorRect(blockStartCursor);
        painter.drawRect(startRect.x() - 2, startRect.y(), 4, startRect.height());
        
        painter.setPen(QPen(Qt::blue, 1));
        painter.drawText(10, 20, "Block Mode - ESC to cancel, Ctrl+K,K to copy, Ctrl+K,Y to cut");
    } else {
        setExtraSelections(QList<QTextEdit::ExtraSelection>());
    }
}

void CustomTextEdit::updateBlockSelection()
{
    if (blockMode) {
        update();
    }
}

bool CustomTextEdit::eventFilter(QObject *obj, QEvent *event)
{
    return QTextEdit::eventFilter(obj, event);
}

// MainWindow実装
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , textEditor(new CustomTextEdit(this))
    , settings(new QSettings(this))
    , findDialog(nullptr)
    , toolBarVisible(true)
    , statusExtrasVisible(true)
    , lastCaseSensitive(false)
    , lastWholeWord(false)
{
    setCentralWidget(textEditor);
    
    setupMenus();
    setupToolBar();
    setupStatusBar();
    loadSettings();
    
    // シグナル接続
    connect(textEditor, &QTextEdit::cursorPositionChanged,
            this, &MainWindow::updateStatusBar);
    connect(textEditor->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::documentModified);
    connect(textEditor, &QTextEdit::undoAvailable,
            undoAction, &QAction::setEnabled);
    connect(textEditor, &QTextEdit::redoAvailable,
            redoAction, &QAction::setEnabled);
    connect(textEditor, &QTextEdit::copyAvailable,
            copyAction, &QAction::setEnabled);
    connect(textEditor, &QTextEdit::copyAvailable,
            cutAction, &QAction::setEnabled);
    
    setCurrentFile("");
    setWindowTitle("WLEditor");
    resize(800, 600);
    
    updateStatusBar();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupMenus()
{
    // ファイルメニュー
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    newAction = new QAction("&New", this);
    newAction->setStatusTip("Create a new file");
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAction);
    
    openAction = new QAction("&Open", this);
    openAction->setStatusTip("Open an existing file");
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    fileMenu->addAction(openAction);
    
    openInNewWindowAction = new QAction("Open in &New Window", this);
    openInNewWindowAction->setShortcut(QKeySequence("Ctrl+Shift+O"));
    openInNewWindowAction->setStatusTip("Open file in new window");
    connect(openInNewWindowAction, &QAction::triggered, this, &MainWindow::openInNewWindow);
    fileMenu->addAction(openInNewWindowAction);
    
    saveAction = new QAction("&Save", this);
    saveAction->setStatusTip("Save the current file");
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    fileMenu->addAction(saveAction);
    
    saveAsAction = new QAction("Save &As...", this);
    saveAsAction->setStatusTip("Save the file with a new name");
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);
    fileMenu->addAction(saveAsAction);
    
    fileMenu->addSeparator();
    
    exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence("Alt+F4"));
    exitAction->setStatusTip("Exit the application");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
    
    // 編集メニュー
    QMenu *editMenu = menuBar()->addMenu("&Edit");
    
    undoAction = new QAction("&Undo", this);
    undoAction->setStatusTip("Undo the last action");
    undoAction->setEnabled(false);
    connect(undoAction, &QAction::triggered, this, &MainWindow::undo);
    editMenu->addAction(undoAction);
    
    redoAction = new QAction("&Redo", this);
    redoAction->setStatusTip("Redo the last undone action");
    redoAction->setEnabled(false);
    connect(redoAction, &QAction::triggered, this, &MainWindow::redo);
    editMenu->addAction(redoAction);
    
    editMenu->addSeparator();
    
    cutAction = new QAction("Cu&t", this);
    cutAction->setStatusTip("Cut the selected text");
    cutAction->setEnabled(false);
    connect(cutAction, &QAction::triggered, this, &MainWindow::cut);
    editMenu->addAction(cutAction);
    
    copyAction = new QAction("&Copy", this);
    copyAction->setStatusTip("Copy the selected text");
    copyAction->setEnabled(false);
    connect(copyAction, &QAction::triggered, this, &MainWindow::copy);
    editMenu->addAction(copyAction);
    
    pasteAction = new QAction("&Paste", this);
    pasteAction->setStatusTip("Paste text from clipboard");
    connect(pasteAction, &QAction::triggered, this, &MainWindow::paste);
    editMenu->addAction(pasteAction);
    
    editMenu->addSeparator();
    
    selectAllAction = new QAction("Select &All", this);
    selectAllAction->setStatusTip("Select all text");
    connect(selectAllAction, &QAction::triggered, this, &MainWindow::selectAll);
    editMenu->addAction(selectAllAction);
    
    editMenu->addSeparator();
    
    // WordStar風検索
    QAction *wordstarFindAction = new QAction("&Find (WordStar)...", this);
    //wordstarFindAction->setShortcut(QKeySequence("Ctrl+Q,F"));
    wordstarFindAction->setStatusTip("WordStar style search (Ctrl+Q, F)");
    connect(wordstarFindAction, &QAction::triggered, this, &MainWindow::wordstarFind);
    editMenu->addAction(wordstarFindAction);
    
    QAction *findNextAction = new QAction("Find &Next", this);
    findNextAction->setShortcut(QKeySequence("Ctrl+L"));
    findNextAction->setStatusTip("Repeat last search (Ctrl+L)");
    connect(findNextAction, &QAction::triggered, this, &MainWindow::wordstarFindNext);
    editMenu->addAction(findNextAction);
    
    QAction *wordstarReplaceAction = new QAction("&Replace (WordStar)...", this);
    //wordstarReplaceAction->setShortcut(QKeySequence("Ctrl+Q,A"));
    wordstarReplaceAction->setStatusTip("WordStar style replace (Ctrl+Q, A)");
    connect(wordstarReplaceAction, &QAction::triggered, this, &MainWindow::wordstarReplace);
    editMenu->addAction(wordstarReplaceAction);
    
    editMenu->addSeparator();
    
    findReplaceAction = new QAction("Find/Replace (&Advanced)...", this);
    findReplaceAction->setShortcut(QKeySequence(Qt::Key_F3));
    findReplaceAction->setStatusTip("Advanced find and replace dialog (F3)");
    connect(findReplaceAction, &QAction::triggered, this, &MainWindow::findReplace);
    editMenu->addAction(findReplaceAction);

    // 表示メニュー
    QMenu *viewMenu = menuBar()->addMenu("&View");
    
    QAction *fontAction = new QAction("&Font...", this);
    fontAction->setStatusTip("Change font and size");
    connect(fontAction, &QAction::triggered, this, &MainWindow::setFont);
    viewMenu->addAction(fontAction);
    
    QAction *wrapAction = new QAction("&Wrap Width...", this);
    wrapAction->setStatusTip("Set text wrap width");
    connect(wrapAction, &QAction::triggered, this, &MainWindow::setWrapWidth);
    viewMenu->addAction(wrapAction);
    
    viewMenu->addSeparator();
    
    toggleToolBarAction = new QAction("&Tool Bar", this);
    toggleToolBarAction->setCheckable(true);
    toggleToolBarAction->setChecked(true);
    toggleToolBarAction->setStatusTip("Show/hide tool bar");
    connect(toggleToolBarAction, &QAction::triggered, this, &MainWindow::toggleToolBar);
    viewMenu->addAction(toggleToolBarAction);
    
    toggleStatusExtrasAction = new QAction("&Status Bar Details", this);
    toggleStatusExtrasAction->setCheckable(true);
    toggleStatusExtrasAction->setChecked(true);
    toggleStatusExtrasAction->setStatusTip("Show/hide font and wrap info in status bar");
    connect(toggleStatusExtrasAction, &QAction::triggered, this, &MainWindow::toggleStatusBarExtras);
    viewMenu->addAction(toggleStatusExtrasAction);
    
    viewMenu->addSeparator();
    
    preferencesAction = new QAction("&Preferences...", this);
    preferencesAction->setStatusTip("Configure application settings");
    connect(preferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);
    viewMenu->addAction(preferencesAction);

}

void MainWindow::setupToolBar()    // 🔧 強制的にCtrl+Q系を処理するアクション
{
    mainToolBar = addToolBar("Main");
    mainToolBar->addAction(newAction);
    mainToolBar->addAction(openAction);
    mainToolBar->addAction(saveAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(cutAction);
    mainToolBar->addAction(copyAction);
    mainToolBar->addAction(pasteAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(undoAction);
    mainToolBar->addAction(redoAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(findReplaceAction);
    
    mainToolBar->setObjectName("MainToolBar");
}

void MainWindow::setupStatusBar()
{
    statusLabel = new QLabel("Ready - WordStar like Keys Enabled");
    statusBar()->addWidget(statusLabel);
    
    statusExtrasWidget = new QWidget();
    QHBoxLayout *extrasLayout = new QHBoxLayout(statusExtrasWidget);
    extrasLayout->setContentsMargins(0, 0, 0, 0);
    
    extrasLayout->addWidget(new QLabel("Wrap:"));
    wrapWidthSpinBox = new QSpinBox();
    wrapWidthSpinBox->setRange(0, 500);
    wrapWidthSpinBox->setValue(80);
    wrapWidthSpinBox->setSpecialValueText("No wrap");
    wrapWidthSpinBox->setSuffix(" chars");
    wrapWidthSpinBox->setToolTip("Set line wrap (0=no wrap, max 500 chars)");
    connect(wrapWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onWrapWidthChanged);
    extrasLayout->addWidget(wrapWidthSpinBox);
    
    extrasLayout->addWidget(new QLabel("Font:"));
    fontComboBox = new QFontComboBox();
    fontComboBox->setMaximumWidth(150);
    fontComboBox->setToolTip("Select font family");
    connect(fontComboBox, &QFontComboBox::currentFontChanged,
            [this](const QFont &font) { 
                QFont newFont = font;
                newFont.setPointSize(fontSizeSpinBox->value());
                textEditor->setFont(newFont);
            });
    extrasLayout->addWidget(fontComboBox);
    
    fontSizeSpinBox = new QSpinBox();
    fontSizeSpinBox->setRange(8, 48);
    fontSizeSpinBox->setValue(12);
    fontSizeSpinBox->setSuffix("pt");
    fontSizeSpinBox->setToolTip("Select font size");
    connect(fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            [this](int size) {
                QFont font = textEditor->font();
                font.setPointSize(size);
                textEditor->setFont(font);
            });
    extrasLayout->addWidget(fontSizeSpinBox);
    
    statusBar()->addPermanentWidget(statusExtrasWidget);
    
    positionLabel = new QLabel("Line: 1, Col: 1");
    statusBar()->addPermanentWidget(positionLabel);
    
    encodingLabel = new QLabel("UTF-8");
    statusBar()->addPermanentWidget(encodingLabel);
}

void MainWindow::onWrapWidthChanged(int value)
{
    textEditor->setWrapWidth(value);
    if (value == 0) {
        statusLabel->setText("Line wrap disabled - WordStar Keys Enabled");
    } else {
        statusLabel->setText(QString("Wrap: %1 chars - WordStar Keys Enabled").arg(value));
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        textEditor->clear();
        setCurrentFile("");
        statusLabel->setText("New file created - WordStar Keys Enabled");
    }
}

void MainWindow::openFile()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, 
            "Open File", "", 
            "Text Files (*.txt *.cpp *.h *.py *.java *.js *.html *.css *.md *.xml *.json);;"
            "C++ Files (*.cpp *.cxx *.cc *.c *.h *.hpp *.hxx);;"
            "Python Files (*.py *.pyw);;"
            "Web Files (*.html *.htm *.css *.js *.json *.xml);;"
            "All Files (*)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                textEditor->setPlainText(in.readAll());
                setCurrentFile(fileName);
                statusLabel->setText("File opened: " + QFileInfo(fileName).fileName() + " - WordStar Keys Enabled");
            } else {
                QMessageBox::warning(this, "WLEditor",
                    QString("Cannot read file %1:\n%2.")
                    .arg(fileName).arg(file.errorString()));
            }
        }
    }
}

void MainWindow::openInNewWindow()
{
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Open File in New Window", "", 
        "Text Files (*.txt *.cpp *.h *.py *.java *.js *.html *.css *.md *.xml *.json);;"
        "C++ Files (*.cpp *.cxx *.cc *.c *.h *.hpp *.hxx);;"
        "Python Files (*.py *.pyw);;"
        "Web Files (*.html *.htm *.css *.js *.json *.xml);;"
        "All Files (*)");
    if (!fileName.isEmpty()) {
        QProcess::startDetached(QApplication::applicationFilePath(), 
                              QStringList() << fileName);
    }
}

void MainWindow::openFileFromArgs(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "WLEditor", 
                           QString("Cannot read file %1:\n%2")
                           .arg(fileName)
                           .arg(file.errorString()));
        return;
    }
    
    QTextStream in(&file);
    textEditor->setPlainText(in.readAll());
    setCurrentFile(fileName);
    statusLabel->setText("File opened: " + QFileInfo(fileName).fileName() + " - WordStar Keys Enabled");
}

void MainWindow::saveFile()
{
    if (currentFile.isEmpty()) {
        saveAsFile();
        return;
    }
    
    QFile file(currentFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << textEditor->toPlainText();
        textEditor->document()->setModified(false);
        statusLabel->setText("File saved: " + QFileInfo(currentFile).fileName() + " - WordStar Keys Enabled");
    } else {
        QMessageBox::warning(this, "WLEditor",
            QString("Cannot write file %1:\n%2.")
            .arg(currentFile).arg(file.errorString()));
    }
}

void MainWindow::saveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save File", "", "Text Files (*.txt);;All Files (*)");
    if (!fileName.isEmpty()) {
        setCurrentFile(fileName);
        saveFile();
    }
}

void MainWindow::copy()
{
    textEditor->copy();
    statusLabel->setText("Text copied - WordStar Keys Enabled");
}

void MainWindow::cut()
{
    textEditor->cut();
    statusLabel->setText("Text cut - WordStar Keys Enabled");
}

void MainWindow::paste()
{
    textEditor->paste();
    statusLabel->setText("Text pasted - WordStar Keys Enabled");
}

void MainWindow::undo()
{
    textEditor->undo();
    statusLabel->setText("Undo - WordStar Keys Enabled");
}

void MainWindow::redo()
{
    textEditor->redo();
    statusLabel->setText("Redo - WordStar Keys Enabled");
}

void MainWindow::selectAll()
{
    textEditor->selectAll();
    statusLabel->setText("All text selected - WordStar Keys Enabled");
}

void MainWindow::findReplace()
{
    if (!findDialog) {
        findDialog = new FindReplaceDialog(this);
        findDialog->setTextEdit(textEditor);
    }
    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}

void MainWindow::wordstarFind()
{
    QDialog *findDialog = new QDialog(this);
    findDialog->setWindowTitle("WordStar Search");
    findDialog->setModal(true);
    findDialog->resize(400, 150);
    
    QVBoxLayout *layout = new QVBoxLayout(findDialog);
    
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel("Find:"));
    QLineEdit *searchEdit = new QLineEdit(lastSearchText);
    searchEdit->selectAll();
    searchLayout->addWidget(searchEdit);
    layout->addLayout(searchLayout);
    
    QHBoxLayout *optionLayout = new QHBoxLayout();
    QCheckBox *caseSensitive = new QCheckBox("Case sensitive");
    caseSensitive->setChecked(lastCaseSensitive);
    QCheckBox *wholeWord = new QCheckBox("Whole word");
    wholeWord->setChecked(lastWholeWord);
    optionLayout->addWidget(caseSensitive);
    optionLayout->addWidget(wholeWord);
    layout->addLayout(optionLayout);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *findButton = new QPushButton("Find Next");
    QPushButton *cancelButton = new QPushButton("Cancel");
    findButton->setDefault(true);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(findButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
    
    connect(findButton, &QPushButton::clicked, [=]() {
        QString searchText = searchEdit->text();
        if (!searchText.isEmpty()) {
            lastSearchText = searchText;
            lastCaseSensitive = caseSensitive->isChecked();
            lastWholeWord = wholeWord->isChecked();
            
            performWordStarSearch();
            findDialog->accept();
        }
    });
    
    connect(cancelButton, &QPushButton::clicked, findDialog, &QDialog::reject);
    connect(searchEdit, &QLineEdit::returnPressed, findButton, &QPushButton::click);
    
    searchEdit->setFocus();
    findDialog->exec();
    delete findDialog;
}

void MainWindow::wordstarReplace()
{
    if (!findDialog) {
        findDialog = new FindReplaceDialog(this);
        findDialog->setTextEdit(textEditor);
    }
    
    if (!lastSearchText.isEmpty()) {
        findDialog->setSearchText(lastSearchText);
    }
    
    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}

void MainWindow::wordstarFindNext()
{
    if (lastSearchText.isEmpty()) {
        wordstarFind();
    } else {
        performWordStarSearch();
    }
}

void MainWindow::performWordStarSearch()
{
    if (lastSearchText.isEmpty()) return;
    
    QTextDocument::FindFlags flags = QTextDocument::FindFlags();
    
    if (lastCaseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (lastWholeWord) {
        flags |= QTextDocument::FindWholeWords;
    }
    
    bool found = textEditor->find(lastSearchText, flags);
    
    if (found) {
        statusLabel->setText(QString("Found: \"%1\" - WordStar Keys Enabled").arg(lastSearchText));
    } else {
        QTextCursor cursor = textEditor->textCursor();
        cursor.movePosition(QTextCursor::Start);
        textEditor->setTextCursor(cursor);
        
        found = textEditor->find(lastSearchText, flags);
        if (found) {
            statusLabel->setText(QString("Found from beginning: \"%1\" - WordStar Keys Enabled").arg(lastSearchText));
        } else {
            statusLabel->setText(QString("Not found: \"%1\" - WordStar Keys Enabled").arg(lastSearchText));
        }
    }
}

void MainWindow::setFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, textEditor->font(), this);
    if (ok) {
        textEditor->setFont(font);
        fontComboBox->setCurrentFont(font);
        fontSizeSpinBox->setValue(font.pointSize());
        statusLabel->setText("Font changed - WordStar Keys Enabled");
    }
}

void MainWindow::setWrapWidth()
{
    bool ok;
    int width = QInputDialog::getInt(this, "Wrap Width",
        "Characters per line (0=no wrap):", wrapWidthSpinBox->value(), 0, 500, 1, &ok);
    if (ok) {
        wrapWidthSpinBox->setValue(width);
        textEditor->setWrapWidth(width);
        if (width == 0) {
            statusLabel->setText("Line wrap disabled - WordStar Keys Enabled");
        } else {
            statusLabel->setText(QString("Wrap: %1 chars - WordStar Keys Enabled").arg(width));
        }
    }
}

void MainWindow::updateStatusBar()
{
    QTextCursor cursor = textEditor->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    positionLabel->setText(QString("Line: %1, Col: %2").arg(line).arg(col));
}

void MainWindow::documentModified()
{
    setWindowModified(textEditor->document()->isModified());
}

bool MainWindow::maybeSave()
{
    if (textEditor->document()->isModified()) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this,
            "WLEditor",
            "The document has been modified.\n"
            "Do you want to save your changes?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (ret == QMessageBox::Save) {
            saveFile();
            return !textEditor->document()->isModified();
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    currentFile = fileName;
    textEditor->document()->setModified(false);
    setWindowModified(false);
    
    QString shownName = currentFile.isEmpty() ? "untitled.txt" : 
                       QFileInfo(currentFile).fileName();
    setWindowTitle(QString("%1[*] - WLEditor").arg(shownName));
}

void MainWindow::loadSettings()
{
    restoreGeometry(settings->value("geometry").toByteArray());
    
    QFont defaultFont("Noto Sans Mono CJK JP", 12);
    
    if (!QFontDatabase().families().contains("Noto Sans Mono CJK JP")) {
        QStringList fallbackFonts = {
            "Noto Sans Mono",
            "DejaVu Sans Mono", 
            "Liberation Mono",
            "Consolas",
            "Monaco",
            "Courier New"
        };
        
        for (const QString &fontName : fallbackFonts) {
            if (QFontDatabase().families().contains(fontName)) {
                defaultFont.setFamily(fontName);
                break;
            }
        }
    }
    
    QFont font = settings->value("font", defaultFont).value<QFont>();
    textEditor->setFont(font);
    fontComboBox->setCurrentFont(font);
    fontSizeSpinBox->setValue(font.pointSize());
    
    int wrapWidth = settings->value("wrapWidth", 80).toInt();
    wrapWidthSpinBox->setValue(wrapWidth);
    textEditor->setWrapWidth(wrapWidth);
    
    toolBarVisible = settings->value("toolBarVisible", true).toBool();
    statusExtrasVisible = settings->value("statusExtrasVisible", true).toBool();
    
    mainToolBar->setVisible(toolBarVisible);
    statusExtrasWidget->setVisible(statusExtrasVisible);
    
    toggleToolBarAction->setChecked(toolBarVisible);
    toggleStatusExtrasAction->setChecked(statusExtrasVisible);
}

void MainWindow::saveSettings()
{
    settings->setValue("geometry", saveGeometry());
    settings->setValue("font", textEditor->font());
    settings->setValue("wrapWidth", wrapWidthSpinBox->value());
    settings->setValue("toolBarVisible", toolBarVisible);
    settings->setValue("statusExtrasVisible", statusExtrasVisible);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        saveSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::toggleToolBar()
{
    toolBarVisible = !toolBarVisible;
    mainToolBar->setVisible(toolBarVisible);
    toggleToolBarAction->setChecked(toolBarVisible);
    settings->setValue("toolBarVisible", toolBarVisible);
}

void MainWindow::toggleStatusBarExtras()
{
    statusExtrasVisible = !statusExtrasVisible;
    statusExtrasWidget->setVisible(statusExtrasVisible);
    toggleStatusExtrasAction->setChecked(statusExtrasVisible);
    settings->setValue("statusExtrasVisible", statusExtrasVisible);
}

void MainWindow::showPreferences()
{
    QDialog *prefDialog = new QDialog(this);
    prefDialog->setWindowTitle("Preferences");
    prefDialog->setModal(true);
    prefDialog->resize(400, 300);
    
    QVBoxLayout *layout = new QVBoxLayout(prefDialog);
    
    QGroupBox *uiGroup = new QGroupBox("Interface", prefDialog);
    QVBoxLayout *uiLayout = new QVBoxLayout(uiGroup);
    
    QCheckBox *toolBarCheck = new QCheckBox("Show Tool Bar", uiGroup);
    toolBarCheck->setChecked(toolBarVisible);
    connect(toolBarCheck, &QCheckBox::toggled, [this](bool checked) {
        toolBarVisible = checked;
        mainToolBar->setVisible(checked);
        toggleToolBarAction->setChecked(checked);
        settings->setValue("toolBarVisible", checked);
    });
    uiLayout->addWidget(toolBarCheck);
    
    QCheckBox *statusExtrasCheck = new QCheckBox("Show Status Bar Details", uiGroup);
    statusExtrasCheck->setChecked(statusExtrasVisible);
    connect(statusExtrasCheck, &QCheckBox::toggled, [this](bool checked) {
        statusExtrasVisible = checked;
        statusExtrasWidget->setVisible(checked);
        toggleStatusExtrasAction->setChecked(checked);
        settings->setValue("statusExtrasVisible", checked);
    });
    uiLayout->addWidget(statusExtrasCheck);
    
    layout->addWidget(uiGroup);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", prefDialog);
    QPushButton *cancelButton = new QPushButton("Cancel", prefDialog);
    
    connect(okButton, &QPushButton::clicked, prefDialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, prefDialog, &QDialog::reject);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    layout->addLayout(buttonLayout);
    
    prefDialog->exec();
    delete prefDialog;
}

// FindReplaceDialog実装
FindReplaceDialog::FindReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , textEditor(nullptr)
{
    setWindowTitle("Find/Replace");
    setModal(false);
    resize(400, 200);
    
    findLineEdit = new QLineEdit();
    findLineEdit->setPlaceholderText("Find what...");
    
    replaceLineEdit = new QLineEdit();
    replaceLineEdit->setPlaceholderText("Replace with...");
    
    findNextButton = new QPushButton("Find &Next");
    findPrevButton = new QPushButton("Find &Previous");
    replaceButton = new QPushButton("&Replace");
    replaceAllButton = new QPushButton("Replace &All");
    closeButton = new QPushButton("&Close");
    
    caseSensitiveCheckBox = new QCheckBox("&Case sensitive");
    wholeWordCheckBox = new QCheckBox("&Whole word");
    
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(new QLabel("Find:"), 0, 0);
    layout->addWidget(findLineEdit, 0, 1, 1, 2);
    layout->addWidget(new QLabel("Replace:"), 1, 0);
    layout->addWidget(replaceLineEdit, 1, 1, 1, 2);
    
    layout->addWidget(caseSensitiveCheckBox, 2, 0);
    layout->addWidget(wholeWordCheckBox, 2, 1);
    
    layout->addWidget(findNextButton, 3, 0);
    layout->addWidget(findPrevButton, 3, 1);
    layout->addWidget(replaceButton, 3, 2);
    layout->addWidget(replaceAllButton, 4, 0);
    layout->addWidget(closeButton, 4, 2);
    
    connect(findNextButton, &QPushButton::clicked, this, &FindReplaceDialog::findNext);
    connect(findPrevButton, &QPushButton::clicked, this, &FindReplaceDialog::findPrevious);
    connect(replaceButton, &QPushButton::clicked, this, &FindReplaceDialog::replace);
    connect(replaceAllButton, &QPushButton::clicked, this, &FindReplaceDialog::replaceAll);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(findLineEdit, &QLineEdit::returnPressed, this, &FindReplaceDialog::findNext);
    connect(replaceLineEdit, &QLineEdit::returnPressed, this, &FindReplaceDialog::replace);
}

void FindReplaceDialog::setTextEdit(QTextEdit *editor)
{
    textEditor = editor;
}

void FindReplaceDialog::setSearchText(const QString &text)
{
    findLineEdit->setText(text);
}

void FindReplaceDialog::setReplaceText(const QString &text)
{
    replaceLineEdit->setText(text);
}

void FindReplaceDialog::findNext()
{
    if (!textEditor || findLineEdit->text().isEmpty()) return;
    
    QString searchText = findLineEdit->text();
    QTextDocument::FindFlags flags = QTextDocument::FindFlags();
    
    if (caseSensitiveCheckBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if (wholeWordCheckBox->isChecked())
        flags |= QTextDocument::FindWholeWords;
    
    bool found = textEditor->find(searchText, flags);
    
    if (!found) {
        QMessageBox::information(this, "Find", "Text not found");
    }
}

void FindReplaceDialog::findPrevious()
{
    if (!textEditor || findLineEdit->text().isEmpty()) return;
    
    QString searchText = findLineEdit->text();
    QTextDocument::FindFlags flags = QTextDocument::FindBackward;
    
    if (caseSensitiveCheckBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if (wholeWordCheckBox->isChecked())
        flags |= QTextDocument::FindWholeWords;
    
    bool found = textEditor->find(searchText, flags);
    
    if (!found) {
        QMessageBox::information(this, "Find", "Text not found");
    }
}

void FindReplaceDialog::replace()
{
    if (!textEditor || findLineEdit->text().isEmpty()) return;
    
    QTextCursor cursor = textEditor->textCursor();
    if (cursor.hasSelection() && cursor.selectedText() == findLineEdit->text()) {
        cursor.insertText(replaceLineEdit->text());
    }
    findNext();
}

void FindReplaceDialog::replaceAll()
{
    if (!textEditor || findLineEdit->text().isEmpty()) return;
    
    QString searchText = findLineEdit->text();
    QString replaceText = replaceLineEdit->text();
    
    QTextCursor cursor = textEditor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    textEditor->setTextCursor(cursor);
    
    int replacements = 0;
    QTextDocument::FindFlags flags = QTextDocument::FindFlags();
    
    if (caseSensitiveCheckBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if (wholeWordCheckBox->isChecked())
        flags |= QTextDocument::FindWholeWords;
    
    while (textEditor->find(searchText, flags)) {
        QTextCursor currentCursor = textEditor->textCursor();
        currentCursor.insertText(replaceText);
        replacements++;
    }
    
    QMessageBox::information(this, "Replace All", 
        QString("Replaced %1 occurrences").arg(replacements));
}