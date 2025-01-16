#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QImage>
#include <QProcess>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Клас потоку для захоплення відео
class VideoThread : public QThread {
    Q_OBJECT

public:
    explicit VideoThread(QObject *parent = nullptr);
    ~VideoThread();

    void setPipeline(const std::string &pipeline);
    void run() override;
    void stop();

signals:
    void frameReady(const QImage &image);

private:
    bool running;
    cv::VideoCapture cap;
    std::string gstPipeline;
};

// Головне вікно
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startVideo1();
    void startVideo2();
    void stopVideo();
    void displayFrame(const QImage &image);

private:
    Ui::MainWindow *ui;
    VideoThread *videoThread;
};

#endif // MAINWINDOW_H
