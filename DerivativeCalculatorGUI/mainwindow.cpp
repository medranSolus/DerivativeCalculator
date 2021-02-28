/*
Author: Marek Machliński
Date: 6.12.2017
*/
#include <QtCharts/QSplineSeries>
#include <QChartView>
#include <QGridLayout>
#include <limits>
#include "mainwindow.h"
#include "ui_mainwindow.h"

int RangeX = 10;
int RangeY = 10;
bool IsDisplayed = false;
QWidget * View = nullptr;
QGridLayout * ViewLayout = nullptr;

std::string NumberToString(long double x)
{
    std::string tmp = std::to_string(x);
    if (tmp.find('.') != std::string::npos)
    {
        for (size_t I = tmp.size(), i = 0; ;)
        {
            if (tmp.at(--I) == '0')
                ++i;
            else
            {
                if (tmp.at(I) != '.')
                    ++I;
                else
                    ++i;
                tmp.erase(I, i);
                break;
            }
        }
    }
    return tmp;
}

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPushButton * button = MainWindow::centralWidget()->findChild<QPushButton *>("derivValButton");
    button->setVisible(false);
    QPlainTextEdit * text = MainWindow::centralWidget()->findChild<QPlainTextEdit *>("xInput");
    text->setVisible(false);
    QLabel * label = MainWindow::centralWidget()->findChild<QLabel *>("xLabel");
    label->setVisible(false);
    label = MainWindow::centralWidget()->findChild<QLabel *>("xValLabel");
    label->setVisible(false);
    label = MainWindow::centralWidget()->findChild<QLabel *>("scaleXLabel");
    label->setVisible(false);
    label = MainWindow::centralWidget()->findChild<QLabel *>("scaleYLabel");
    label->setVisible(false);
    QSpinBox * box = MainWindow::centralWidget()->findChild<QSpinBox *>("scaleXBox");
    box->setVisible(false);
    box->setValue(10);
    box = MainWindow::centralWidget()->findChild<QSpinBox *>("scaleYBox");
    box->setVisible(false);
    box->setValue(10);
    View = MainWindow::centralWidget()->findChild<QWidget *>("widget");
    ViewLayout = new QGridLayout(View);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Display()
{
    QPushButton * derivativeButton = MainWindow::centralWidget()->findChild<QPushButton *>("derivValButton");
    derivativeButton->setEnabled(true);
    derivativeButton->setVisible(true);
    QPlainTextEdit text = MainWindow::centralWidget()->findChild<QPlainTextEdit *>("xInput");
    text->setEnabled(true);
    text->setVisible(true);
    QLabel * label = MainWindow::centralWidget()->findChild<QLabel *>("xLabel");
    label->setEnabled(true);
    label->setVisible(true);
    label = MainWindow::centralWidget()->findChild<QLabel *>("xValLabel");
    label->setEnabled(true);
    label->setVisible(true);
    label = MainWindow::centralWidget()->findChild<QLabel *>("scaleXLabel");
    label->setEnabled(true);
    label->setVisible(true);
    label = MainWindow::centralWidget()->findChild<QLabel *>("scaleYLabel");
    label->setEnabled(true);
    label->setVisible(true);
    QSpinBox * box = MainWindow::centralWidget()->findChild<QSpinBox *>("scaleXBox");
    box->setVisible(true);
    box->setEnabled(true);
    box = MainWindow::centralWidget()->findChild<QSpinBox *>("scaleYBox");
    box->setVisible(true);
    box->setEnabled(true);
}

void MainWindow::on_derivButton_clicked()
{
    using namespace QtCharts;
    QPlainTextEdit * text = MainWindow::centralWidget()->findChild<QPlainTextEdit *>("functionInput");
    std::string functionName = GetDerivative(text->toPlainText().toLocal8Bit().constData());
    if(!IsDisplayed)
    {
        IsDisplayed = true;
        Display();
    }
    QChart * chart = new QChart();
    QSplineSeries * function = new QSplineSeries();
    QSplineSeries * functionDerivative = new QSplineSeries();
    function->setName("f(x)=" + text->toPlainText());
    functionDerivative->setName(QString::fromLocal8Bit(functionName.c_str()));
    if(text->toPlainText() == "x")
    {
        function->append(-1 * RangeX, -1 * RangeX);
        function->append(RangeX, RangeX);
        chart->addSeries(function);
        functionDerivative->append(-1 * RangeX, 1);
        functionDerivative->append(RangeX, 1);
        chart->addSeries(functionDerivative);
    }
    else
    {
        for (double i = -1 * RangeX; i <= RangeX; i += 0.001)
        {
            long double temp = GetFunctionValue(text->toPlainText().toLocal8Bit().constData(), i);
            if (temp != OUT_OF_RANGE)
                function->append(i, temp);
        }
        chart->addSeries(function);

        for (long double i = -1 * RangeX; i <= RangeX; i += 0.001)
        {
            long double temp = GetDerivativeValue(text->toPlainText().toLocal8Bit().constData(), i);
            if (temp != OUT_OF_RANGE)
               functionDerivative->append(i, temp);
        }
        chart->addSeries(functionDerivative);
    }
    chart->createDefaultAxes();
    chart->axisY()->setRange(-1 * RangeY, RangeY);
    chart->axisX()->setRange(-1 * RangeX, RangeX);
    QChartView * chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ViewLayout->addWidget(chartView, 0, 0);
}

void MainWindow::on_derivValButton_clicked()
{
    QPlainTextEdit * txt = MainWindow::centralWidget()->findChild<QPlainTextEdit *>("xInput");
    QString tmp = txt->toPlainText();
    long double x = 0;
    int i = -1, I = 0, S = tmp.size();
    std::string Tab = "";
    bool minus = false;
    if(tmp.at(0) == '-')
    {
        ++I;
        minus = true;
    }
    while (I < S && tmp.at(I).isNumber())
    {
        ++i;
        Tab += tmp.at(I).unicode();
        ++I;
    }
    if (I < S && tmp.at(I) == '.')
    {
        ++I;
        int j = 0;
        while (I < S)
        {
            x += (tmp.at(I).unicode() - '0') / pow(10, ++j);
            ++I;
        }
    }
    for (int j = 0; i >= 0; --i, ++j)
        x += (Tab.at(i) - '0') * pow(10, j);
    if(minus)
        x *= -1;
    QLabel * xValLabel = MainWindow::centralWidget()->findChild<QLabel *>("xValLabel");
    txt = MainWindow::centralWidget()->findChild<QPlainTextEdit *>("functionInput");
    Tab = "f'(" + Tab + ")=" + NumberToString(GetDerivativeValue(txt->toPlainText().toLocal8Bit().constData(), x));
    xValLabel->setText(QString::fromLocal8Bit(Tab.c_str()));
}

void MainWindow::on_scaleXBox_valueChanged(int arg1)
{
    RangeX = arg1;
    on_derivButton_clicked();
}

void MainWindow::on_scaleYBox_valueChanged(int arg1)
{
    RangeY = arg1;
    on_derivButton_clicked();
}
