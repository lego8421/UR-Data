#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QThread>
#include <QList>
#include <vector>
#include <iostream>

#include "ur10.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionOpen_triggered() {
    QDir dir = QDir(QDir::homePath() + "/Desktop");
    QString filePath =  QFileDialog::getOpenFileName(this, "Open", dir.path(), "Text files (*.txt)");


    if (!filePath.isEmpty()) {
        QFile file(filePath);
        QStringList datas;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setCodec("UTF-8");

            while(!in.atEnd()) {
                QString line = in.readLine();
                datas << line;
            }
            file.close();


            //            QThread thread;
            //            QEventLoop loop;
            //            QObject context;
            //            context.moveToThread(&thread);
            //            connect(&thread, &QThread::started, &context, [&]() {
            displayData(datas);
            //                loop.quit();
            //            });
            //            thread.start();
            //            loop.exec();
            //            thread.quit();
            //            thread.wait();
        }
    }
}

void MainWindow::displayData(QStringList datas) {
    if (datas.size()) {
        QStringList display;
        QStringList header;
        header << "time"
               << "J1 Voltages"
               << "J2 Voltages"
               << "J3 Voltages"
               << "J4 Voltages"
               << "J5 Voltages"
               << "J6 Voltages"
               << "J1 Currents"
               << "J2 Currents"
               << "J3 Currents"
               << "J4 Currents"
               << "J5 Currents"
               << "J6 Currents";
        display << header.join(",");

        for (auto data : datas) {
            std::vector<char> buffer;
            QStringList splited = data.split(",");
            for (auto elements : splited) {
                buffer.push_back(static_cast<char>(elements.toInt()));
            }
            Ur10RealTimePacket packet = UR10::convert(buffer);

            QStringList row;
            row << QString::number(packet.m_fTime, 'f', 3);

            for (int i = 0; i < 6; i++) {
                row << QString::number(packet.m_fArrVActual[i], 'f', 3);
            }
            for (int i = 0; i < 6; i++) {
                row << QString::number(packet.m_fArrIActual[i], 'f', 3);
            }
            display << row.join(",");
        }
        ui->plainTextEdit->setPlainText(display.join("\n"));
    }
}
