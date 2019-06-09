#include "multilogviewer.h"
#include "ui_multilogviewer.h"

namespace logviewer{


MultiLogViewer::MultiLogViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MultiLogViewer)
{
    ui->setupUi(this);
}

MultiLogViewer::~MultiLogViewer()
{
    delete ui;
}

}
