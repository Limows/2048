#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "2048.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QMainWindow>
#include <QWidget>
#include <QKeyEvent>
#include <QPainter>
#include <QSwipeGesture>
#include <QtCore/qmath.h>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

protected:
    void keyPressEvent(QKeyEvent *keyEvent);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);
    bool event(QEvent *event);



private slots:
    void on_actionNew_Game_triggered();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

private:
    void drawFinal(QPainter &painter);
    inline int offsetCoords(int coord);
    void drawTile(QPainter &painter, int value, int x, int y);
    bool gestureEvent(QGestureEvent *event);
    void swipeTriggered(QSwipeGesture *gesture);
    void save();
    void load();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H