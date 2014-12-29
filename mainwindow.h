#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QFile>
#include "qcustomplot.h"
#include "settingsdialog.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class SettingsDialog;

class MainWindow : public QMainWindow
{
          Q_OBJECT

public:
          explicit MainWindow(QWidget *parent = 0);
          ~MainWindow();

private slots:
          void serialReceived();
          void resetRange();
          void labelChange();
          void changeHeight();
          void changeDistance();
          void changeLanguage();
          void newFile();
          void openFile();
          void save();
          void saveData();
          void undo();
          void openSerialPort();
          void closeSerialPort();
          void graphClick(QCPAbstractPlottable *plottable);
          void about();
          void mousePosition(QMouseEvent *event);
          void addPoint(double a, double d, double h, int v);
          void on_plotButton_clicked();
private:
          Ui::MainWindow *ui;
          QSerialPort *serial;
          SettingsDialog  *settings;
          QList<QCPCurve*> listGraph;
          QList<int> list;
};

#endif // MAINWINDOW_H
