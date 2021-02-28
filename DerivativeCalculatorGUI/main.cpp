/*
Author: Marek Machliński
Date: 6.12.2017
*/
#include <QApplication>
#include <QLibrary>
#include "mainwindow.h"

const LPCSTR DerivativeDLLName = "?getDerivative@DerivativeLibrary@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBV23@@Z";
const LPCSTR DerivativeValueDLLName = "?getDerivativeValue@DerivativeLibrary@@YAOAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBO@Z";
const LPCSTR FunctionValueDLLName = "?getFunctionValue@DerivativeLibrary@@YAOAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBO@Z";
derivativeCall GetDerivative = nullptr;
derivativeValueCall GetDerivativeValue = nullptr;
derivativeValueCall GetFunctionValue = nullptr;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    HMODULE dllHandle = LoadLibrary(L"DerivativeLibrary.dll");
    if (dllHandle == nullptr)
    {
        QMessageBox::information(nullptr, "Error", "Error loading library \"DeriverativeLibrary.dll\"", QMessageBox::Ok);
        exit(-1);
    }
    GetDerivative = (derivativeCall)GetProcAddress(dllHandle, DerivativeDLLName);
    if (GetDerivative == nullptr)
    {
        QMessageBox::information(nullptr, "Error", "Error loading function \"GetDerivative\"", QMessageBox::Ok);
        exit(1);
    }
    GetDerivativeValue = (derivativeValueCall)GetProcAddress(dllHandle, DerivativeValueDLLName);
    if (GetDerivativeValue == nullptr)
    {
        QMessageBox::information(nullptr, "Error", "Error loading function \"GetDerivativeValue\"", QMessageBox::Ok);
        exit(2);
    }
    GetFunctionValue = (derivativeValueCall)GetProcAddress(dllHandle, FunctionValueDLLName);
    if (GetFunctionValue == nullptr)
    {
        QMessageBox::information(nullptr, "Error", "Error loading function \"GetFunctionValue\"", QMessageBox::Ok);
        exit(3);
    }
    window.show();
    window.setWindowTitle("Derivative Calculator");
    return app.exec();
}
