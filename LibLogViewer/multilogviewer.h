#ifndef MULTILOGVIEWER_H
#define MULTILOGVIEWER_H

#include <QMainWindow>

namespace logviewer{


namespace Ui {
class MultiLogViewer;
}

class MultiLogViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MultiLogViewer(QWidget *parent = nullptr);
    ~MultiLogViewer();

private:
    Ui::MultiLogViewer *ui;
};

}


#endif // LOGVIEWER_H
