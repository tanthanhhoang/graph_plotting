#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QDebug>

int value=-1;
double maxXAxis,minXAxis,maxYAxis,minYAxis;
QString dir;
QFile tempFile;
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
          ui->setupUi(this);
          dir = "C:/temp";
          tempFile.setFileName(dir);
          tempFile.resize(0);
          maxXAxis=5;
          minXAxis=-5;
          maxYAxis=0;
          minYAxis=-5;
          serial = new QSerialPort(this);
          settings = new SettingsDialog;

          //setup Button visible
          ui->actionDisconnect->setEnabled(false);
          ui->plotButton->setEnabled(false);

          ui->verticalSlider->setRange(0,500);
          ui->verticalSlider->setValue(100);
          ui->horizontalSlider->setRange(minXAxis*100,maxXAxis*100);
          ui->horizontalSlider->setValue(0);

          connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
          connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
          connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(about()));
          connect(ui->actionUndo,SIGNAL(triggered()),this,SLOT(undo()));
          connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(newFile()));
          connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(openFile()));
          connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(save()));
          connect(ui->actionSave_data,SIGNAL(triggered()),this,SLOT(saveData()));
          connect(ui->actionSetting,SIGNAL(triggered()),settings,SLOT(show()));
          connect(ui->customPlot,SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)),this,SLOT(graphClick(QCPAbstractPlottable*)));
          connect(ui->customPlot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(mousePosition(QMouseEvent*)));
          connect(ui->customPlot,SIGNAL(mouseDoubleClick(QMouseEvent*)),this,SLOT(resetRange()));
          connect(serial,SIGNAL(readyRead()),this,SLOT(serialReceived()));
          connect(settings,SIGNAL(finished(int)),this,SLOT(changeLanguage()));

          connect(ui->verticalSlider,SIGNAL(valueChanged(int)),this,SLOT(changeHeight()));
          connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(changeDistance()));

          connect(ui->heightSB,SIGNAL(valueChanged(double)),this,SLOT(labelChange()));
          connect(ui->distanceSB,SIGNAL(valueChanged(double)),this,SLOT(labelChange()));

          ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
          ui->customPlot->xAxis->setLabel("Distance (m)");
          ui->customPlot->yAxis->setLabel("Height (m)");
          ui->customPlot->addGraph();
          ui->customPlot->addGraph();
          QPen pen1;
          pen1.setWidth(2);
          pen1.setColor(Qt::red);
          ui->customPlot->graph(1)->setPen(pen1);
          ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssPlus);
          double x = (double)(ui->horizontalSlider->value())/100;
          double y = (double)(-ui->verticalSlider->value())/100;
          ui->customPlot->graph(1)->clearData();
          ui->customPlot->graph(1)->addData(x,y);
          ui->customPlot->replot();
          //set range
          ui->customPlot->xAxis->setRange(minXAxis,maxXAxis);
          ui->customPlot->yAxis->setRange(minYAxis,maxYAxis);
          ui->customPlot->xAxis->setAutoTickStep(false);
          ui->customPlot->yAxis->setAutoTickStep(false);
          ui->customPlot->xAxis->setTickStep(0.5);
          ui->customPlot->yAxis->setTickStep(0.5);
}

MainWindow::~MainWindow()
{
          serial->close();
          delete ui;
          delete settings;
}

void MainWindow::serialReceived(){
          if(serial->bytesAvailable()>=16){
                    int temp;
                    QByteArray data_temp;
                    char *data_lux;
                    data_temp=serial->readAll();
                    data_lux=data_temp.data();
                    if(data_lux[14]!=0x19)    {
                              data_lux[8]  -= 0x30;
                              data_lux[9]  -= 0x30;
                              data_lux[10]-= 0x30;
                              char sum = data_lux[8]+ data_lux[9]+ data_lux[10];
                              temp = ((int)(data_lux[11]-0x30)*1000+(int)(data_lux[12]-0x30)*100+(int)(data_lux[13]-0x30)*10+(int)(data_lux[14]-0x30));
                              switch (sum){
                              case 0x0F:
                                        temp *=100;
                                        break;
                              case 0x1E:
                                        temp *=10;
                                        break;
                              case 0x2D:
                                        break;
                              }
                              value=temp;
                              QString msg = QString ("%1").arg(value);
                              ui->label->setText(msg);
                    }
                    else {
                              value=-1;
                              ui->label->setText("XXXX");
                    }
          }
}

void MainWindow::resetRange(){
    ui->customPlot->xAxis->setRange(minXAxis,maxXAxis);
    ui->customPlot->yAxis->setRange(minYAxis,maxYAxis);
    ui->customPlot->replot();
}

void MainWindow::labelChange(){

          double x = (double)(ui->distanceSB->value());
          double y = (double)(ui->heightSB->value());
          ui->customPlot->graph(1)->clearData();
          ui->customPlot->graph(1)->addData(x,y);
          ui->customPlot->replot();
          ui->verticalSlider->setValue((int)(-y*100));
          ui->horizontalSlider->setValue((int)(x*100));

}

void MainWindow::changeHeight(){
    ui->heightSB->setValue((double)(ui->verticalSlider->value()*-1)/100);
}

void MainWindow::changeDistance(){
    ui->distanceSB->setValue((double)(ui->horizontalSlider->value())/100);
}

void MainWindow::changeLanguage(){
    SettingsDialog::Settings p = settings->settings();
    if(p.language == true){
        ui->menuIlluminance_Graph->setTitle("Menu");
        ui->menuAbout->setTitle("About");
        ui->menuSetting->setTitle("Edit");
        ui->menuSetting_2->setTitle("Setting");
        ui->plotButton->setText("Plot");
        ui->text0->setText("Illuminance:");
        ui->text1->setText("Height:");
        ui->text2->setText("Distance:");
        ui->actionNew->setText("New Data");
        ui->actionOpen->setText("Open Data");
        ui->actionSave->setText("Save Image");
        ui->actionSave_data->setText("Save data");
        ui->actionUndo->setText("Undo");
        ui->actionConnect->setText("Connect");
        ui->actionDisconnect->setText("Disconnect");
        ui->actionSetting->setText("Setting");
        ui->actionQuit->setText("Quit");
        ui->actionAbout->setText("About");
        ui->customPlot->xAxis->setLabel("Distance (m)");
        ui->customPlot->yAxis->setLabel("Height (m)");
        ui->customPlot->replot();
    }
    else{
        ui->menuIlluminance_Graph->setTitle("Trình đơn");
        ui->menuAbout->setTitle("Thông tin");
        ui->menuSetting->setTitle("Chỉnh sửa");
        ui->menuSetting_2->setTitle("Cài đặt");
        ui->plotButton->setText("Dựng điểm");
        ui->text0->setText("Độ rọi:");
        ui->text1->setText("Độ cao:");
        ui->text2->setText("Khoảng cách:");
        ui->actionNew->setText("Tạo dữ liệu");
        ui->actionOpen->setText("Mở dữ liệu");
        ui->actionSave->setText("Lưu hình");
        ui->actionSave_data->setText("Lưu dữ liệu");
        ui->actionUndo->setText("Hoàn tác");
        ui->actionConnect->setText("Kết nối");
        ui->actionDisconnect->setText("Ngắt kết nối");
        ui->actionSetting->setText("Cài đặt");
        ui->actionQuit->setText("Thoát");
        ui->actionAbout->setText("Thông tin");
        ui->customPlot->xAxis->setLabel("Khoảng cách (m)");
        ui->customPlot->yAxis->setLabel("Độ cao (m)");
        ui->customPlot->replot();
    }
}

void MainWindow::newFile(){
          dir = "C:/temp";
          tempFile.setFileName(dir);
          tempFile.resize(0);
          listGraph.clear();
          list.clear();
          ui->customPlot->clearPlottables();
          ui->customPlot->addGraph();
          QPen pen;
          pen.setWidth(4);
          pen.setColor(Qt::blue);
          ui->customPlot->graph(0)->setPen(pen);
          ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
          ui->customPlot->addGraph();
          QPen pen1;
          pen1.setWidth(2);
          pen1.setColor(Qt::red);
          ui->customPlot->graph(1)->setPen(pen1);
          ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssPlus);
          ui->horizontalSlider->setValue(0);
          ui->verticalSlider->setValue(100);
          double x = (double)(ui->horizontalSlider->value())/100;
          double y = (double)(-ui->verticalSlider->value())/100;
          ui->customPlot->graph(1)->clearData();
          ui->customPlot->graph(1)->addData(x,y);
          ui->customPlot->xAxis->setRange(minXAxis,maxXAxis);
          ui->customPlot->yAxis->setRange(minYAxis,maxYAxis);
          ui->customPlot->replot();
}

void MainWindow::openFile(){
    ui->customPlot->clearGraphs();
    dir = QFileDialog::getOpenFileName(this, "Get data file name",QDir::homePath(),"Data File(*.data)");
    tempFile.setFileName(dir);
    listGraph.clear();
    list.clear();
    ui->customPlot->clearPlottables();
    ui->customPlot->addGraph();
    ui->customPlot->addGraph();
    QPen pen1;
    pen1.setWidth(2);
    pen1.setColor(Qt::red);
    ui->customPlot->graph(1)->setPen(pen1);
    ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssPlus);
    double x = (double)(ui->horizontalSlider->value())/100;
    double y = (double)(-ui->verticalSlider->value())/100;
    ui->customPlot->graph(1)->clearData();
    ui->customPlot->graph(1)->addData(x,y);
    ui->customPlot->xAxis->setRange(minXAxis,maxXAxis);
    ui->customPlot->yAxis->setRange(minYAxis,maxYAxis);
    ui->customPlot->replot();
    if (!tempFile.open(QIODevice::ReadOnly)) {
      qDebug()<< tempFile.errorString();
      qDebug()<< tempFile.error();
    }
    QTextStream in(&tempFile);
    while(!in.atEnd()){
        QString s = in.readLine();
        QStringList datao = s.split(" ");
        double height = datao[1].toDouble();
        double distance = datao[2].toDouble();
        double angle = atan2(-height,distance);
        int val = datao[0].toInt();
        addPoint(angle,distance,height,val);
    }
    tempFile.close();
    ui->customPlot->replot();
}

void MainWindow::save(){
          ui->customPlot->graph(1)->clearData();
          ui->customPlot->graph(0)->clearData();
          ui->customPlot->replot();
          QString outputDir = QFileDialog::getSaveFileName(this,tr("Save graph"),"",tr("Joint Photographic Group (*.jpg)"));
          QFile file(outputDir);
          if (!file.open(QIODevice::WriteOnly|QFile::WriteOnly))
          {
              QMessageBox::warning(0,"Could not create Image File",
              QObject::tr( "\n Could not create Image File on disk"));
          }

          ui->customPlot->saveJpg( outputDir,  1780, 844, 2.0, 100  );
}

void MainWindow::saveData(){
    QString dir = QFileDialog::getSaveFileName(this, "Save data","","Data File(*.data)");
    QFile file(dir);
    file.resize(0);
    if (tempFile.open(QIODevice::ReadWrite|QFile::Text)) {
        QTextStream in(&tempFile);
        if(file.open(QIODevice::ReadWrite|QIODevice::Truncate|QFile::Text)){
            QTextStream out(&file);
            out << in.readAll();
        }
    }
    file.close();
    tempFile.close();
    tempFile.setFileName(dir);
}

void MainWindow::undo(){
    tempFile.setFileName(dir);
    //delete last line
    if(tempFile.open(QIODevice::ReadWrite|QIODevice::Text)){
        QTextStream t(&tempFile);
        QString line = t.readAll();
        QStringList datau;
        if(line.count("\n")>1){
            line=line.left(line.size()-1);
            datau = line.right(line.size()-line.lastIndexOf("\n")-1).split(" ");
            line=line.left(line.lastIndexOf("\n"))+"\n";
            tempFile.resize(0);
            t<<line;
        }
        else{
            line.left(line.size()-1);
            datau = line.right(line.size()-line.lastIndexOf("\n")-1).split(" ");
            tempFile.resize(0);
            t<<"";
        }
        tempFile.close();
    }
    listGraph.clear();
    list.clear();
    ui->customPlot->clearPlottables();
    ui->customPlot->addGraph();
    ui->customPlot->addGraph();
    QPen pen1;
    pen1.setWidth(2);
    pen1.setColor(Qt::red);
    ui->customPlot->graph(1)->setPen(pen1);
    ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssPlus);
    double x = (double)(ui->horizontalSlider->value())/100;
    double y = (double)(-ui->verticalSlider->value())/100;
    ui->customPlot->graph(1)->clearData();
    ui->customPlot->graph(1)->addData(x,y);
    ui->customPlot->xAxis->setRange(minXAxis,maxXAxis);
    ui->customPlot->yAxis->setRange(minYAxis,maxYAxis);
    ui->customPlot->replot();
    if (!tempFile.open(QIODevice::ReadOnly)) {
      qDebug()<< tempFile.errorString();
      qDebug()<< tempFile.error();
    }
    QTextStream in(&tempFile);
    while(!in.atEnd()){
        QString s = in.readLine();
        QStringList datao = s.split(" ");
        double height = datao[1].toDouble();
        double distance = datao[2].toDouble();
        double angle = atan2(-height,distance);
        int val = datao[0].toInt();
        addPoint(angle,distance,height,val);
    }
    tempFile.close();
    ui->customPlot->replot();
}

void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
            ui->actionSetting->setEnabled(false);
            ui->plotButton->setEnabled(true);
    } else {
        SettingsDialog::Settings p = settings->settings();
        if(p.language == true){
            QMessageBox::critical(this, tr("Error"), serial->errorString());
        }
        else{
            QMessageBox::critical(this, tr("Lỗi"), serial->errorString());
        }
    }
}

void MainWindow::closeSerialPort()
{
    serial->close();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionSetting->setEnabled(true);
    ui->plotButton->setEnabled(false);
}

void MainWindow::graphClick(QCPAbstractPlottable *plottable){
    if((plottable->name()!="Graph 1")&&(plottable->name()!="Graph 2"))
          ui->statusBar->showMessage(QString("Illuminance: %1.").arg(plottable->name()),2000);
}

void MainWindow::about(){
          QMessageBox infoBox;
          SettingsDialog::Settings p = settings->settings();
          if(p.language == true){
            infoBox.about(this,tr("About"),tr("Graph Plot 1.1.0\nCopyright 2014 GPLed Co. All rights reserved"));
          }
          else{
            infoBox.about(this,tr("Thông tin"),tr("Graph Plot 1.1.0\nBản quyền thuộc về công ty GPLed.\nCấm sao chép và sự dụng dưới mọi hình thức."));
          }
          infoBox.setIconPixmap(QPixmap(":/img/48x48/Help.png"));
}

void MainWindow::mousePosition(QMouseEvent *event){
          int x = ui->customPlot->xAxis->pixelToCoord((double)event->pos().x())*100;
          int y  = ui->customPlot->yAxis->pixelToCoord((double)event->pos().y())*100;
          SettingsDialog::Settings p = settings->settings();
          if(p.language == true){
              ui->label_4->setText(QString("Height: %1 m, Distance: %2 m.").arg((float)y/100).arg((float)x/100));
          }
          else{
              ui->label_4->setText(QString("Độ cao: %1 m, Khoảng cách: %2 m.").arg((float)y/100).arg((float)x/100));
          }
}

void MainWindow::addPoint(double a, double d, double h, int v){
    if(list.contains(v)){
        if(listGraph.at(list.indexOf(v))->data()->count()==1){
            listGraph.at(list.indexOf(v))->setScatterStyle(QCPScatterStyle::ssNone);
            QPen graphPen;
            graphPen.setWidth(1);
            listGraph.at(list.indexOf(v))->setPen(graphPen);
        }
        listGraph.at(list.indexOf(v))->addData(a, d, h);
    }
    else{
        int r,g,b;
        double p;
        if((v<=400)&&(v>=75)){
            p = (double)v/400;
            r=255*p;
            g=0;
            b=255*(1-p);
        }
        else if(v<75){
            p = (double)v/75;
            r=0;
            g=0;
            b=255*p;
        }
        else{
            p = (double)(v-400)/600;
            r=255;
            g=255*p;
            b=255*p;
        }
        QCPCurve *newcurve = new QCPCurve(ui->customPlot->xAxis,ui->customPlot->yAxis);
        if (ui->customPlot->addPlottable(newcurve))
        {
            newcurve->setName(QString::number(v)+" lux");
            QPen dotPen;
            dotPen.setWidth(2);
            dotPen.setColor(QColor(r,g,b));
            newcurve->setPen(dotPen);
            newcurve->setScatterStyle(QCPScatterStyle::ssDisc);
            if (QCPCurve *graph = qobject_cast<QCPCurve*>(newcurve)) listGraph.append(graph);
            list.append(v);
            listGraph.at(list.indexOf(v))->addData(a, d, h);
        }
        else{
            delete newcurve;
        }
    }
}

void MainWindow::on_plotButton_clicked()
{
    value-=value%5;
          if(value>=0){
            double distance = (double)ui->horizontalSlider->value()/100;
            double height = (double)-ui->verticalSlider->value()/100;
            double angle = atan2(-height, distance);
            addPoint(angle, distance, height, value);
            tempFile.setFileName(dir);
            if(!tempFile.open(QIODevice::ReadWrite|QIODevice::Append|QFile::Text)){
              qDebug()<< "Could not open file";
          }
            QTextStream out(&tempFile);
            out << value << " " << height << " " << distance << endl;
            tempFile.flush();
            tempFile.close();
          }
          ui->customPlot->replot();
}
