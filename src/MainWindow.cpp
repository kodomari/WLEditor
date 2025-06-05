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
    setAcceptRichText(false); // プレーンテキストのみ
    
    // リセットタイマーの設定
    resetTimer = new QTimer(this);
    resetTimer->setSingleShot(true);
    resetTimer->setInterval(3000); // 3秒でリセット
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
    // 2段階キーバインドの処理
    if (waitingForCtrlQ) {
        handleCtrlQ(event);
        return;
    }
    if (waitingForCtrlK) {
        handleCtrlK(event);
        return;
    }
    
    // Ctrl修飾子が押されている場合のWordStarキーバインド
    if (event->modifiers() == Qt::ControlModifier) {
        switch (event->key()) {
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
        case Qt::Key_G: // 右の文字を削除（Delete）
            {
                QTextCursor cursor = textCursor();
                cursor.deleteChar();
            }
            return;
        case Qt::Key_H: // 左の文字を削除（Backspace）
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
        case Qt::Key_Y: // 行削除（カーソル位置から行末まで）
            {
                QTextCursor cursor = textCursor();
                cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
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
            return;
        case Qt::Key_K: // Ctrl+K系コマンドの開始
            waitingForCtrlK = true;
            resetTimer->start();
            return;
        }
    }
    
    // 通常のキー処理
    QTextEdit::keyPressEvent(event);
}

void CustomTextEdit::handleCtrlQ(QKeyEvent *event)
{
    resetTwoKeyMode();
    
    if (event->modifiers() == Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_R: // Ctrl+Q, R - ファイル先頭へ
            moveCursor(QTextCursor::Start);
            break;
        case Qt::Key_C: // Ctrl+Q, C - ファイル末尾へ
            moveCursor(QTextCursor::End);
            break;
        case Qt::Key_S: // Ctrl+Q, S - 行頭へ
            moveCursor(QTextCursor::StartOfLine);
            break;
        case Qt::Key_D: // Ctrl+Q, D - 行末へ
            moveCursor(QTextCursor::EndOfLine);
            break;
        case Qt::Key_E: // Ctrl+Q, E - 画面上端へ
            {
                QTextCursor cursor = textCursor();
                QScrollBar *scrollBar = verticalScrollBar();
                int topValue = scrollBar->value();
                
                // 画面の上端に表示されている位置を計算
                QPoint topPoint(0, topValue);
                QTextCursor topCursor = cursorForPosition(topPoint);
                setTextCursor(topCursor);
            }
            break;
        case Qt::Key_X: // Ctrl+Q, X - 画面下端へ
            {
                QTextCursor cursor = textCursor();
                QScrollBar *scrollBar = verticalScrollBar();
                int bottomValue = scrollBar->value() + viewport()->height();
                
                // 画面の下端に表示されている位置を計算
                QPoint bottomPoint(0, bottomValue);
                QTextCursor bottomCursor = cursorForPosition(bottomPoint);
                setTextCursor(bottomCursor);
            }
            break;
        }
    }
}

void CustomTextEdit::handleCtrlK(QKeyEvent *event)
{
    resetTwoKeyMode();
    
    if (event->modifiers() == Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_B: // Ctrl+K, B - 選択開始
            blockStartCursor = textCursor();
            blockMode = true;
            // 視覚的フィードバックのために再描画
            update();
            break;
case Qt::Key_K: // Ctrl+K, K - 選択終了＋コピー（テキストを残す）
    if (blockMode) {
        QTextCursor endCursor = textCursor();
        QTextCursor selectionCursor = blockStartCursor;
        
        // 開始位置から終了位置まで選択
        int startPos = qMin(blockStartCursor.position(), endCursor.position());
        int endPos = qMax(blockStartCursor.position(), endCursor.position());
        
        selectionCursor.setPosition(startPos);
        selectionCursor.setPosition(endPos, QTextCursor::KeepAnchor);
        setTextCursor(selectionCursor);
        
        // 選択されたテキストをクリップボード履歴に保存（コピーのみ、削除しない）
        if (textCursor().hasSelection()) {
            QString selectedText = textCursor().selectedText();
            
            // クリップボード履歴に追加
            clipboardHistory.prepend(selectedText);
            if (clipboardHistory.size() > 10) {
                clipboardHistory.removeLast();
            }
            currentClipboardIndex = 0;
            
            // システムクリップボードにもコピー（テキストは削除しない）
            QApplication::clipboard()->setText(selectedText);
            
            // 選択を解除（テキストは残す）
            QTextCursor cursor = textCursor();
            cursor.clearSelection();
            setTextCursor(cursor);
        }
        blockMode = false;
        update(); // 選択表示をクリア
    }
    break;            
        case Qt::Key_C: // Ctrl+K, C - ペースト（履歴はそのまま）
            if (!clipboardHistory.isEmpty() && currentClipboardIndex < clipboardHistory.size()) {
                // 現在のクリップボード項目をペースト（履歴インデックスは変更しない）
                QTextCursor cursor = textCursor();
                cursor.insertText(clipboardHistory[currentClipboardIndex]);
            } else {
                // 履歴がない場合は通常のペースト
                paste();
            }
            break;
        case Qt::Key_V: // Ctrl+K, V - ペースト＆履歴を戻す
            if (!clipboardHistory.isEmpty()) {
                // 現在のクリップボード項目をペースト
                if (currentClipboardIndex < clipboardHistory.size()) {
                    QTextCursor cursor = textCursor();
                    cursor.insertText(clipboardHistory[currentClipboardIndex]);
                    
                    // 次回は一つ前の履歴を使用
                    currentClipboardIndex++;
                    if (currentClipboardIndex >= clipboardHistory.size()) {
                        currentClipboardIndex = 0; // 循環
                    }
                }
            } else {
                // 履歴がない場合は通常のペースト
                paste();
            }
            break;
case Qt::Key_Y: // Ctrl+K, Y - 選択部分をカット（削除）または1行削除
    if (blockMode) {
        // ブロックモード中の場合：選択部分をカット
        QTextCursor endCursor = textCursor();
        QTextCursor selectionCursor = blockStartCursor;
        
        // 開始位置から終了位置まで選択
        int startPos = qMin(blockStartCursor.position(), endCursor.position());
        int endPos = qMax(blockStartCursor.position(), endCursor.position());
        
        selectionCursor.setPosition(startPos);
        selectionCursor.setPosition(endPos, QTextCursor::KeepAnchor);
        setTextCursor(selectionCursor);
        
        // 選択されたテキストをクリップボード履歴に保存してカット
        if (textCursor().hasSelection()) {
            QString selectedText = textCursor().selectedText();
            
            // クリップボード履歴に追加
            clipboardHistory.prepend(selectedText);
            if (clipboardHistory.size() > 10) {
                clipboardHistory.removeLast();
            }
            currentClipboardIndex = 0;
            
            // テキストを削除（カット）
            textCursor().removeSelectedText();
            
            // システムクリップボードにもコピー
            QApplication::clipboard()->setText(selectedText);
        }
        blockMode = false;
        update(); // 選択表示をクリア
    } else {
        // ブロックモードでない場合：現在行を削除（元の動作を維持）
        QTextCursor cursor = textCursor();
        cursor.select(QTextCursor::LineUnderCursor);
        QString deletedLine = cursor.selectedText();
        
        // 削除した行をクリップボード履歴に追加
        clipboardHistory.prepend(deletedLine);
        if (clipboardHistory.size() > 10) {
            clipboardHistory.removeLast();
        }
        currentClipboardIndex = 0;
        
        cursor.removeSelectedText();
        cursor.deleteChar(); // 改行文字も削除
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
    
    // ブロックモード中の視覚的フィードバック
    if (blockMode) {
        QPainter painter(viewport());
        
        // 選択範囲をハイライト
        QTextCursor currentCursor = textCursor();
        QTextCursor selectionCursor = blockStartCursor;
        
        int startPos = qMin(blockStartCursor.position(), currentCursor.position());
        int endPos = qMax(blockStartCursor.position(), currentCursor.position());
        
        selectionCursor.setPosition(startPos);
        selectionCursor.setPosition(endPos, QTextCursor::KeepAnchor);
        
        if (selectionCursor.hasSelection()) {
            // 選択範囲の背景を青色でハイライト
            QList<QTextEdit::ExtraSelection> extraSelections;
            QTextEdit::ExtraSelection selection;
            
            QColor lineColor = QColor(Qt::blue).lighter(160);
            selection.format.setBackground(lineColor);
            selection.cursor = selectionCursor;
            extraSelections.append(selection);
            
            setExtraSelections(extraSelections);
        }
        
        // 選択開始位置にマーカーを描画
        painter.setPen(QPen(Qt::blue, 2, Qt::DashLine));
        QRect startRect = cursorRect(blockStartCursor);
        painter.drawRect(startRect.x() - 2, startRect.y(), 4, startRect.height());
    } else {
        // ブロックモードでない場合は、ハイライトをクリア
        setExtraSelections(QList<QTextEdit::ExtraSelection>());
    }
}

void CustomTextEdit::updateBlockSelection()
{
    if (blockMode) {
        // 選択表示はpaintEventで行い、ここではカーソル位置だけ更新
        update(); // 再描画をトリガー
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
    , toolBarVisible(true)           // 新しく追加
    , statusExtrasVisible(true)      // 新しく追加
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
    setWindowTitle("WordStar Editor");
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
    // WordStarキーバインドを優先するため、標準ショートカットを削除
    newAction->setStatusTip("Create a new file");
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAction);
    
    openAction = new QAction("&Open", this);
    openAction->setStatusTip("Open an existing file");
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    fileMenu->addAction(openAction);
    
    saveAction = new QAction("&Save", this);
    // Ctrl+S をWordStarの左移動に使うため、ショートカットを削除
    saveAction->setStatusTip("Save the current file");
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    fileMenu->addAction(saveAction);
    
    saveAsAction = new QAction("Save &As...", this);
    saveAsAction->setStatusTip("Save the file with a new name");
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);
    fileMenu->addAction(saveAsAction);
    
    fileMenu->addSeparator();
    
    exitAction = new QAction("E&xit", this);
    // Ctrl+QをWordStarの拡張移動に使うため、終了ショートカットを無効化
    exitAction->setShortcut(QKeySequence("Alt+F4")); // 代替ショートカット
    exitAction->setStatusTip("Exit the application");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
    
    // 編集メニュー
    QMenu *editMenu = menuBar()->addMenu("&Edit");
    
    undoAction = new QAction("&Undo", this);
    // WordStarキーバインドを優先するため、標準ショートカットを削除
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
    // WordStarキーバインドを優先するため、標準ショートカットを削除
    cutAction->setStatusTip("Cut the selected text");
    cutAction->setEnabled(false);
    connect(cutAction, &QAction::triggered, this, &MainWindow::cut);
    editMenu->addAction(cutAction);
    
    copyAction = new QAction("&Copy", this);
    // WordStarキーバインドを優先するため、標準ショートカットを削除
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
    // WordStarキーバインドを優先するため、標準ショートカットを削除
    selectAllAction->setStatusTip("Select all text");
    connect(selectAllAction, &QAction::triggered, this, &MainWindow::selectAll);
    editMenu->addAction(selectAllAction);
    
    editMenu->addSeparator();
    
    findReplaceAction = new QAction("&Find/Replace", this);
    // WordStarの検索を実装するまで、F3キーを使用
    findReplaceAction->setShortcut(QKeySequence(Qt::Key_F3));
    findReplaceAction->setStatusTip("Find and replace text");
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
    
    // 新しく追加：UI要素の表示/非表示
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
    
    // 設定メニュー
    preferencesAction = new QAction("&Preferences...", this);
    preferencesAction->setStatusTip("Configure application settings");
    connect(preferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);
    viewMenu->addAction(preferencesAction);
}

void MainWindow::setupToolBar()
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
    
    // オブジェクト名を設定（設定保存用）
    mainToolBar->setObjectName("MainToolBar");
}

void MainWindow::setupStatusBar()
{
    statusLabel = new QLabel("Ready - WordStar like Keys Enabled");
    statusBar()->addWidget(statusLabel);
    
    // ステータスバーの詳細情報をまとめるウィジェット
    statusExtrasWidget = new QWidget();
    QHBoxLayout *extrasLayout = new QHBoxLayout(statusExtrasWidget);
    extrasLayout->setContentsMargins(0, 0, 0, 0);
    
    // 折り返し幅設定
    extrasLayout->addWidget(new QLabel("Wrap:"));
    wrapWidthSpinBox = new QSpinBox();
    wrapWidthSpinBox->setRange(0, 200);
    wrapWidthSpinBox->setValue(80);
    wrapWidthSpinBox->setSpecialValueText("No wrap");
    wrapWidthSpinBox->setSuffix(" chars");
    wrapWidthSpinBox->setToolTip("Set line wrap (0=no wrap)");
    connect(wrapWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onWrapWidthChanged);
    extrasLayout->addWidget(wrapWidthSpinBox);
    
    // フォント設定
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
            "Open File", "", "Text Files (*.txt);;All Files (*)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                textEditor->setPlainText(in.readAll());
                setCurrentFile(fileName);
                statusLabel->setText("File opened: " + QFileInfo(fileName).fileName() + " - WordStar Keys Enabled");
            } else {
                QMessageBox::warning(this, "WordStar Editor",
                    QString("Cannot read file %1:\n%2.")
                    .arg(fileName).arg(file.errorString()));
            }
        }
    }
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
        QMessageBox::warning(this, "WordStar Editor",
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
        "Characters per line (0=no wrap):", wrapWidthSpinBox->value(), 0, 200, 1, &ok);
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
            "WordStar Editor",
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
    setWindowTitle(QString("%1[*] - WordStar Editor").arg(shownName));
}

void MainWindow::loadSettings()
{
    // ウィンドウの位置とサイズを復元
    restoreGeometry(settings->value("geometry").toByteArray());
    
    // フォント設定を復元（デフォルトをNoto Sans Mono CJK JPに変更）
    QFont defaultFont("Noto Sans Mono CJK JP", 12);
    
    // フォントが利用できない場合の代替フォント
    if (!QFontDatabase().families().contains("Noto Sans Mono CJK JP")) {
        // 代替候補を順に試す
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
    
// 折り返し幅を復元
    int wrapWidth = settings->value("wrapWidth", 80).toInt();
    wrapWidthSpinBox->setValue(wrapWidth);
    textEditor->setWrapWidth(wrapWidth);
    
    // UI表示設定を復元
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

// FindReplaceDialog実装
FindReplaceDialog::FindReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , textEditor(nullptr)
{
    setWindowTitle("Find/Replace");
    setModal(false);
    resize(400, 200);
    
    // UI要素作成
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
    
    // レイアウト
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
    
    // シグナル接続
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

void FindReplaceDialog::findNext()
{
    if (!textEditor || findLineEdit->text().isEmpty()) return;
    
    QString searchText = findLineEdit->text();
    QTextDocument::FindFlags flags = QTextDocument::FindFlags();
    
    if (caseSensitiveCheckBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if (wholeWordCheckBox->isChecked())
        flags |= QTextDocument::FindWholeWords;
    
    // QTextEditのfindメソッドを使用
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

// 新しく追加するスロット関数
void MainWindow::toggleToolBar()
{
    toolBarVisible = !toolBarVisible;
    mainToolBar->setVisible(toolBarVisible);
    toggleToolBarAction->setChecked(toolBarVisible);
    
    // 設定を保存
    settings->setValue("toolBarVisible", toolBarVisible);
}

void MainWindow::toggleStatusBarExtras()
{
    statusExtrasVisible = !statusExtrasVisible;
    statusExtrasWidget->setVisible(statusExtrasVisible);
    toggleStatusExtrasAction->setChecked(statusExtrasVisible);
    
    // 設定を保存
    settings->setValue("statusExtrasVisible", statusExtrasVisible);
}

void MainWindow::showPreferences()
{
    // 簡単な設定ダイアログ
    QDialog *prefDialog = new QDialog(this);
    prefDialog->setWindowTitle("Preferences");
    prefDialog->setModal(true);
    prefDialog->resize(400, 300);
    
    QVBoxLayout *layout = new QVBoxLayout(prefDialog);
    
    // UI表示設定グループ
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
    
    // ボタン
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