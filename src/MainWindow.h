#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QSpinBox>
#include <QFontComboBox>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
#include <QCloseEvent>
#include <QSettings>
#include <QTextCursor>
#include <QTextDocument>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFontMetrics>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QPaintEvent>

class FindReplaceDialog;

// カスタムテキストエディタクラス（WordStarキーバインド対応）
class CustomTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    CustomTextEdit(QWidget *parent = nullptr);
    void setWrapWidth(int characters);
    int getWrapWidth() const { return wrapCharacters; }

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void updateWrapWidth();
    void handleCtrlQ(QKeyEvent *event);
    void handleCtrlK(QKeyEvent *event);
    void resetTwoKeyMode();
    void updateBlockSelection();
    
    int wrapCharacters;
    bool useCharacterWrap;
    
    // 2段階キーバインド用
    bool waitingForCtrlQ;
    bool waitingForCtrlK;
    QTimer *resetTimer;
    
    // 選択機能用
    QTextCursor blockStartCursor;
    bool blockMode;
    
    // クリップボード履歴
    QStringList clipboardHistory;
    int currentClipboardIndex;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void copy();
    void cut();
    void paste();
    void undo();
    void redo();
    void selectAll();
    void findReplace();
    void setFont();
    void setWrapWidth();
    void updateStatusBar();
    void documentModified();
    void onWrapWidthChanged(int value);

private:
    void setupMenus();
    void setupStatusBar();
    void setupToolBar();
    void closeEvent(QCloseEvent *event) override;
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    void loadSettings();
    void saveSettings();
    
    CustomTextEdit *textEditor;
    QString currentFile;
    QLabel *statusLabel;
    QLabel *positionLabel;
    QLabel *encodingLabel;
    QSpinBox *wrapWidthSpinBox;
    QFontComboBox *fontComboBox;
    QSpinBox *fontSizeSpinBox;
    QSettings *settings;
    
    // アクション
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *copyAction;
    QAction *cutAction;
    QAction *pasteAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *selectAllAction;
    QAction *findReplaceAction;
    
    FindReplaceDialog *findDialog;
};

// 検索・置換ダイアログ
class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    FindReplaceDialog(QWidget *parent = nullptr);
    void setTextEdit(QTextEdit *editor);

private slots:
    void findNext();
    void findPrevious();
    void replace();
    void replaceAll();

private:
    QLineEdit *findLineEdit;
    QLineEdit *replaceLineEdit;
    QPushButton *findNextButton;
    QPushButton *findPrevButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QPushButton *closeButton;
    QCheckBox *caseSensitiveCheckBox;
    QCheckBox *wholeWordCheckBox;
    
    QTextEdit *textEditor;
    QTextCursor lastFoundCursor;
};

#endif // MAINWINDOW_H