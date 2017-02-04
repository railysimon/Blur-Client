#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QUdpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QHostAddress>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QTime>
#include <QVector>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QColorDialog>
#include <QFontDialog>
#include <QTimer>
#include <QAction>
#include <QSystemTrayIcon>
#include <QFileDialog>
#include <QDir>
#include <QKeyEvent>
#include <ctime>

class UDPClient : public QDialog
{
    Q_OBJECT

public:
    UDPClient(QWidget *parent = 0);
    ~UDPClient();

private:
        QUdpSocket *client;
        QTextEdit *text;
        QLineEdit *line, *nickname;
        QListWidget *list;
        QPushButton *send, *okay;
        QVector<QListWidgetItem*> items;
        QTimer *timer;
        QAction *timestamp, *open;
        QSystemTrayIcon *tray;
        QDir dir;

private:
        void SystemMessage(QString message);
        void Layout();
        void MenuCreator();
        void Tray();
        QTextEdit *edit();
        QPushButton *button(QString name);

protected:
            virtual void closeEvent(QCloseEvent *event);
            virtual void keyPressEvent(QKeyEvent *event);

private slots:
            void ProcessData(); // Server - methods
            void SendDatagram();
            void Entering(); // Registration form
            void ServerError(); // server didnt respons
            void SendFile();

            void Colored(); // Message-text color
            void setFont(); // Message-text font
            void TrayWindowShow();
            void About();

signals:
            void Close();
};

#endif // UDPCLIENT_H
