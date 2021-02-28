/*
Author: Marek Machliński
Date: 6.12.2017
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <windows.h>
#include <string>
#undef max
#ifdef DERIVATIVE_LIBRARY
#define DERIVATIVE_API Q_DECL_EXPORT
#else
#define DERIVATIVE_API __declspec(dllimport)
#endif
#define OUT_OF_RANGE -1 * std::numeric_limits<long double>::max() + 1

typedef DERIVATIVE_API std::string (* derivativeCall)(const std::string & txt);
typedef DERIVATIVE_API long double (* derivativeValueCall)(const std::string & txt, const long double & x);
extern derivativeCall GetDerivative;
extern derivativeValueCall GetDerivativeValue;
extern derivativeValueCall GetFunctionValue;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Ui::MainWindow *ui;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();   
private slots:
    void Display();
    void on_derivButton_clicked();
    void on_derivValButton_clicked();
    void on_scaleXBox_valueChanged(int arg1);
    void on_scaleYBox_valueChanged(int arg1);
};

#endif // MAINWINDOW_H
