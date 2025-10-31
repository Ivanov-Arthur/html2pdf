#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextDocument>
#include <QPrinter>
#include <QFile>
#include <QTextStream>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPageSize>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setAcceptDrops(true);
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget;
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QLabel *titleLabel = new QLabel("HTML2PDF");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    mainLayout->addWidget(titleLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout;

    QPushButton *loadBtn = new QPushButton("Load HTML");
    QPushButton *convertBtn = new QPushButton("Convert to PDF");
    QPushButton *clearBtn = new QPushButton("Clear");

    connect(loadBtn, &QPushButton::clicked, this, [this]() { loadHtmlFile(); });
    connect(convertBtn, &QPushButton::clicked, this, [this]() { convertToPdf(); });
    connect(clearBtn, &QPushButton::clicked, this, [this]() { clearContent(); });

    buttonLayout->addWidget(loadBtn);
    buttonLayout->addWidget(convertBtn);
    buttonLayout->addWidget(clearBtn);
    mainLayout->addLayout(buttonLayout);

    htmlEdit = new QTextEdit;
    mainLayout->addWidget(htmlEdit);

    QLabel *dragLabel = new QLabel("Drag and drop HTML files here");
    dragLabel->setAlignment(Qt::AlignCenter);
    dragLabel->setStyleSheet("color: #666; margin: 10px;");
    mainLayout->addWidget(dragLabel);

    setWindowTitle("HTML2PDF Converter");
    setMinimumSize(800, 600);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QUrl url = event->mimeData()->urls().first();
        if (url.isLocalFile())
            loadHtmlFile(url.toLocalFile());
    }
}

void MainWindow::loadHtmlFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open HTML File", "", "HTML Files (*.html *.htm)");
    if (!filePath.isEmpty())
        loadHtmlFile(filePath);
}

void MainWindow::loadHtmlFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Cannot open file: " + filePath);
        return;
    }

    QTextStream in(&file);
    htmlEdit->setPlainText(in.readAll());
    file.close();
    setWindowTitle("HTML to PDF - " + QFileInfo(filePath).fileName());
}

void MainWindow::convertToPdf()
{
    if (htmlEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Warning", "No HTML content to convert!");
        return;
    }

    QString pdfPath = QFileDialog::getSaveFileName(this, "Save PDF", "out.pdf", "PDF Files (*.pdf)");
    if (pdfPath.isEmpty()) return;

    QTextDocument document;
    document.setHtml(htmlEdit->toPlainText());

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(pdfPath);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    document.setPageSize(printer.pageRect(QPrinter::Point).size());
    document.print(&printer);

    QMessageBox::information(this, "Success", "PDF created: " + pdfPath);
}

void MainWindow::clearContent()
{
    htmlEdit->clear();
    setWindowTitle("HTML to PDF Converter");
}
