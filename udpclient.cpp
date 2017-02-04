#include "udpclient.h"

UDPClient::UDPClient(QWidget *parent) : QDialog(parent)
{
    Layout();

    client = new QUdpSocket(this);
    client->bind(2424);
    connect(client, SIGNAL(readyRead()), this, SLOT(ProcessData()));

}

UDPClient::~UDPClient()
{

}

void UDPClient::SystemMessage(QString message)
{
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    QString str = message;
    out << nickname->text() << str;
    client->writeDatagram(datagram, QHostAddress::LocalHost, 2323);
}

void UDPClient::Layout()
{
    QPalette pal;
    pal.setColor(this->backgroundRole(), QColor(90, 90, 90));

    text = edit();
    line = new QLineEdit();
    line->setEnabled(false);
    line->setFont(QFont("Ubuntu", 10, QFont::Bold));
    line->setStyleSheet("background-color: rgb(137, 197, 212)");

    list = new QListWidget;
    list->setStyleSheet("background-color: rgb(137, 197, 212)");
    list->setIconSize(QSize(30, 30));
    list->setFont(QFont("Ubuntu", 10, QFont::Bold));

    nickname = new QLineEdit("NickName");
    nickname->setStyleSheet("background-color: rgb(137, 197, 212)");
    nickname->setFont(QFont("Ubuntu", 10, QFont::Bold));

    send = button("Send");
    send->setEnabled(false);
    connect(send, SIGNAL(clicked(bool)), this, SLOT(SendDatagram()));

    okay = button("Ok");
    connect(okay, SIGNAL(clicked(bool)), this, SLOT(Entering()));

    QHBoxLayout *center = new QHBoxLayout;
    center->addWidget(text);
    center->addWidget(list);

    QHBoxLayout *top = new QHBoxLayout;
    top->addWidget(new QLabel("<b><font color=white>Chat:</font></b>"));
    top->addSpacing(180);
    top->addWidget(new QLabel("<b><font color=white>People:</font></b>"));

    QVBoxLayout *bottom_messages = new QVBoxLayout;
    bottom_messages->addWidget(line);
    bottom_messages->addWidget(send);

    QGroupBox *messages = new QGroupBox("Send message");
    messages->setFont(QFont("Ubuntu", 9, QFont::Decorative));
    messages->setLayout(bottom_messages);

    QVBoxLayout *bottom_entering = new QVBoxLayout;
    bottom_entering->addWidget(nickname);
    bottom_entering->addWidget(okay);

    QGroupBox *entering = new QGroupBox("Please enter");
    entering->setLayout(bottom_entering);
    entering->setFont(QFont("Ubuntu", 9, QFont::Decorative));

    QHBoxLayout *bottom = new QHBoxLayout;
    bottom->addWidget(messages);
    bottom->addWidget(entering);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addSpacing(20);
    layout->addLayout(top);
    layout->addLayout(center);
    layout->addLayout(bottom);

    MenuCreator();
    Tray();

    this->setWindowTitle("Blur");
    this->setLayout(layout);
    this->setPalette(pal);
    this->setWindowIcon(QPixmap("icon.png"));
    this->setFixedSize(550, 400);
}

void UDPClient::MenuCreator()
{
    QMenuBar *bar = new QMenuBar(this);
    bar->setFont(QFont("Ubuntu", 9, QFont::Normal));
    bar->setMaximumHeight(20);
    bar->setMinimumWidth(170);
    bar->setCursor(Qt::PointingHandCursor);

    QMenu *file = new QMenu("&File");

    open = new QAction("Open and send...");
    open->setEnabled(false);
    connect(open, SIGNAL(triggered(bool)), this, SLOT(SendFile()));
    file->addAction(open);
    file->addAction("Hide", this, SLOT(hide()), Qt::Key_Escape);
    file->addSeparator();
    file->addAction("Quit", this, SLOT(close()), Qt::CTRL + Qt::Key_Q);
    file->setCursor(Qt::PointingHandCursor);

    QMenu *edit = new QMenu("&Edit");
    timestamp = new QAction("Timestamp");
    timestamp->setCheckable(true);
    timestamp->setChecked(true);
    edit->addAction(timestamp);
    edit->addAction("Clear chat", text, SLOT(clear()), Qt::ALT + Qt::Key_C);
    edit->setCursor(Qt::PointingHandCursor);

    QMenu *view = new QMenu("&View");
    view->addAction("Color", this, SLOT(Colored()));
    view->addAction("Font", this, SLOT(setFont()));

    bar->addMenu(file);
    bar->addMenu(edit);
    bar->addMenu(view);
    bar->addAction("About", this, SLOT(About()));

}

void UDPClient::Tray()
{
    tray = new QSystemTrayIcon(this);
    tray->setIcon(QPixmap("icon.png"));

    QMenu *context = new QMenu;
    context->addAction("Show/Hide", this, SLOT(TrayWindowShow()));
    context->addAction("Quit", this, SLOT(close()));
    tray->setContextMenu(context);
    tray->show();
}

QTextEdit *UDPClient::edit()
{
    QTextEdit *txt = new QTextEdit;
    txt->setReadOnly(true);
    txt->setFrameStyle(2);
    txt->setMinimumWidth(350);

    txt->setStyleSheet("color: white; background-color: rgb(37, 37, 37)");
    txt->setFont(QFont("Ubuntu", 8, QFont::Bold));

    return txt;
}

QPushButton *UDPClient::button(QString name)
{
    QPushButton *btn = new QPushButton(name);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFont(QFont("Ubuntu", 10, QFont::Bold));
    btn->setStyleSheet("QPushButton {color: white; background-color: rgb(96, 91, 161)} "
                       "QPushButton::hover {background-color: rgb(85, 202, 128)}");

    return btn;
}

void UDPClient::closeEvent(QCloseEvent *event)
{
    QMessageBox message(QMessageBox::Warning, "Warning", "Do you want leave the chat?", QMessageBox::Ok | QMessageBox::Cancel);
    int value = message.exec();

    if(value == QMessageBox::Cancel) event->ignore();
    if((value == QMessageBox::Ok) && send->isEnabled())
    {
        SystemMessage("%Disconnect%");
        event->accept();
    }
}

void UDPClient::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape) this->hide();
}

void UDPClient::ProcessData()
{
    QByteArray datagram;

    do
      {
            datagram.resize(client->pendingDatagramSize());
            client->readDatagram(datagram.data(), datagram.size());

      } while(client->hasPendingDatagrams());

    QString message, name;
    QDataStream in(&datagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_7);
    in >> name;
    in >> message;

    if(message == "%List%")
    {
        QString title;
        QListWidgetItem *item;

        list->clear();

        for(int i=0; i<name.toInt(); i++)
        {
            in >> title;
            item = new QListWidgetItem(title, list);
            item->setIcon(QPixmap("online.gif"));
            items.push_back(item);
        }
    }

    else if(message == " /FILE/ ")
    {
        QString txt, filter, file_text;
        QPixmap pix(500, 500);

        in >> txt;
        in >> pix;

        if(filter == "png") in >> pix;
        if(filter == "txt") in >> file_text;

        if(!dir.exists(QApplication::applicationDirPath() + "/Downloads"))
            dir.mkdir("Downloads");


        srand(time(NULL));
        if(filter == "png") pix.save(QApplication::applicationDirPath() + "/Downloads/" + QString::number(rand() % 1000), "PNG");

        if(filter == "txt")
        {
            QFile file(QApplication::applicationDirPath() + "/Downloads/" + QString::number(rand() % 1000));
            file.open(QIODevice::WriteOnly);
            QByteArray arr = file_text.toLatin1();
            file.write(arr);
            file.close();
        }

        text->append(name + ": " + txt + message);
        tray->showMessage("Information", "You received some file. Check the Downloads direcotry!", QSystemTrayIcon::Information, 3000);
    }

    else
    {
        if(timestamp->isChecked())
        {
            QTime time = QTime::currentTime();
            text->append("[" + time.toString("hh:mm:ss") + "] " + name + ": " + message);
        }
        else text->append(name + ": " + message);
    }


}

void UDPClient::SendDatagram()
{
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    out << nickname->text() << line->text();
    client->writeDatagram(datagram, QHostAddress::LocalHost, 2323);
    line->clear();
}

void UDPClient::Entering()
{
    if(!(nickname->text().isEmpty()) && (nickname->text() != "NickName"))
    {
        send->setEnabled(true);
        line->setEnabled(true);
        okay->setEnabled(false);
        nickname->setEnabled(false);
        open->setEnabled(true);
        SystemMessage("%New connection%");
    }

    timer = new QTimer(this);
    timer->setInterval(10000);
    connect(timer, SIGNAL(timeout()), this, SLOT(ServerError()));
    timer->start();
}

void UDPClient::Colored()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "Color Dialog");
    text->setStyleSheet("background-color: rgb(37, 37, 37); color: rgb(" + QString::number(color.red()) + ","
                        + QString::number(color.green()) + "," + QString::number(color.blue()) + ")");
}

void UDPClient::setFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Ubuntu", 8, QFont::Bold), this, "Font Dialog");

    if(ok) text->setFont(font);
}

void UDPClient::TrayWindowShow()
{
    this->setVisible(!(this->isVisible()));
}

void UDPClient::About()
{
    QMessageBox *about = new QMessageBox(QMessageBox::Information, "About", "Blur - simple network messanger.\n"
                                                                            "It based on C++/Qt and using a UDP protocol.\n"
                                                                            "You easy can send short messages to your friends \n"
                                                                            "and even small pictures or text files. (~30kB).\n"
                                                                            "It fit for small local networks.", QMessageBox::Ok, this);
    about->setStyleSheet("color: white; background-color: rgb(90, 90, 90)");
    about->setFont(QFont("Ubuntu", 10, QFont::Bold));
    about->setCursor(Qt::PointingHandCursor);

    int value = about->exec();

    if(value == QMessageBox::Ok || value == QMessageBox::Cancel)
        delete about;


}

void UDPClient::ServerError()
{
    if(items.isEmpty())
    {
        QMessageBox error(QMessageBox::Critical, "Error", "Server wasn't found. Try again.", QMessageBox::Ok);
        int value = error.exec();
        connect(this, SIGNAL(Close()), qApp, SLOT(quit()));
        if(value == QMessageBox::Ok || value == QMessageBox::Cancel) emit Close();
    }
}

void UDPClient::SendFile()
{
    QString path = QFileDialog::getOpenFileName(this, "Open dialog", QApplication::applicationDirPath(), "*.png;; *.jpg;; *.txt;;");

    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    QString descriptor = " /FILE/ ";
    QString filter;

    if(!path.isEmpty())
    {
        if(path.contains("png") || path.contains("jpg"))
        {
            QPixmap pix(path);
            filter = "png";
            out << nickname->text() << descriptor << line->text() << filter << pix;
        }

        if(path.contains("txt"))
        {
            QFile file(path);
            file.open(QIODevice::ReadOnly);
            QString file_text = file.readAll();
            file.close();

            filter = "txt";
            out << nickname->text() << descriptor << line->text() << filter << file_text;
        }

        client->writeDatagram(datagram, QHostAddress::LocalHost, 2323);
        tray->showMessage("Information", "Choosen file was sent!", QSystemTrayIcon::Information, 3000);
        line->clear();
        path.clear();
    }
}
