#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "drawsymbols.h"
void DrawCrosshair(cv::InputOutputArray frame, cv::Point center,  const cv::Scalar& color,
                   int thickness);

VideoThread::VideoThread(QObject *parent) : QThread(parent), running(false) {}

VideoThread::~VideoThread() {
    stop();
}

void VideoThread::setPipeline(const std::string &pipeline) {
    gstPipeline = pipeline;
}

void VideoThread::run() {
    running = true;

    cap.open(gstPipeline, cv::CAP_GSTREAMER);
    if (!cap.isOpened()) {
        qDebug() << "Can not open the stream";
        emit frameReady(QImage());
        return;
    }

    cv::Mat frame;
    while (running) {
        cap >> frame; // Capture a frame
        if (frame.empty()) continue;

        // Calculate the center of the video frame
        int centerX = frame.cols / 2;
        int centerY = frame.rows / 2;
        cv::Point center = cv::Point(centerX,centerY);
        // Define the dimensions of the guidance marker rectangle
        int markerWidth = 100;
        int markerHeight = 100;

        // Create a rectangle centered in the frame
        cv::Rect2d markerRect(centerX - markerWidth / 2, centerY - markerHeight / 2, markerWidth, markerHeight);

        // Draw the guidance marker
        cv::drawGuidanceMarker(frame, markerRect, cv::Scalar(255, 255, 0), 2, 30, 1);

        // Draw a crosshair
        DrawCrosshair(frame, center, cv::Scalar(255, 255, 0), 2);

        // Add "Lorem Ipsum" to the top-left corner with line wrapping
        std::string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. In varius arcu vel ullamcorper iaculis. Proin mollis at felis a auctor. Vestibulum faucibus et dolor et aliquam. Maecenas tortor tellus, pharetra at augue eget, sollicitudin consectetur ex. In porta condimentum ante, et fermentum leo ultrices in. Fusce sit amet dolor porttitor, congue justo nec, elementum arcu. Phasellus in elit tincidunt, sodales lacus eget, fermentum ipsum.";
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.5; // Smaller font size
        int thickness = 1;
        int lineHeight = 15; // Height between lines
        int maxWidth = frame.cols - 30; // Maximum width for a line of text

        cv::Point textOrg(10, 20); // Starting position for the text
        int x = textOrg.x;
        int y = textOrg.y;

        // Split text into lines based on maxWidth
        std::stringstream ss(text);
        std::string word;
        std::string line;
        while (ss >> word) {
            std::string testLine = line + (line.empty() ? "" : " ") + word;
            int baseline = 0;
            cv::Size textSize = cv::getTextSize(testLine, fontFace, fontScale, thickness, &baseline);
            if (textSize.width > maxWidth) {
                // Draw the current line and start a new one
                cv::putText(frame, line, cv::Point(x, y), fontFace, fontScale, cv::Scalar(255, 255, 255), thickness);
                line = word;
                y += lineHeight; // Move to the next line
            } else {
                line = testLine;
            }
        }
        // Draw the last line
        if (!line.empty()) {
            cv::putText(frame, line, cv::Point(x, y), fontFace, fontScale, cv::Scalar(255, 255, 255), thickness);
        }

        // Convert BGR to RGB for display in Qt
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        // Convert the frame to a QImage
        QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

        // Emit the frameReady signal with the updated frame
        emit frameReady(image.copy());
    }

    cap.release();
}

void DrawCrosshair(cv::InputOutputArray frame, cv::Point center,  const cv::Scalar& color,
                   int thickness){
    int crossLength = 10;
    cv::line(frame, cv::Point(center.x - crossLength, center.y), cv::Point(center.x + crossLength, center.y),
             color, thickness);
    cv::line(frame, cv::Point(center.x, center.y - crossLength), cv::Point(center.x, center.y + crossLength),
             color, thickness);
}

void VideoThread::stop() {
    running = false;
    wait();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), videoThread(nullptr) {
    ui->setupUi(this);

    connect(ui->startButton1, &QPushButton::clicked, this, &MainWindow::startVideo1);
    connect(ui->startButton2, &QPushButton::clicked, this, &MainWindow::startVideo2);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopVideo);
}

MainWindow::~MainWindow() {
    stopVideo();
    delete ui;
}

void MainWindow::startVideo1() {
    stopVideo();

    videoThread = new VideoThread(this);

    videoThread->setPipeline("udpsrc port=5601 ! tsparse ! tsdemux ! h264parse ! avdec_h264 ! videoconvert ! video/x-raw, format=BGR ! appsink sync=false");

    connect(videoThread, &VideoThread::frameReady, this, &MainWindow::displayFrame);

    videoThread->start();
}

void MainWindow::startVideo2() {
    stopVideo();

    videoThread = new VideoThread(this);

    videoThread->setPipeline("udpsrc port=5600 ! tsparse ! tsdemux ! h264parse ! avdec_h264 ! videoconvert ! video/x-raw, format=BGR ! appsink sync=false");

    connect(videoThread, &VideoThread::frameReady, this, &MainWindow::displayFrame);

    videoThread->start();
}

void MainWindow::stopVideo() {
    if (videoThread) {
        videoThread->stop();
        videoThread->deleteLater();
        videoThread = nullptr;
    }
}

void MainWindow::displayFrame(const QImage &image) {
    if (!image.isNull()) {
        ui->videoLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio));
    }
}
