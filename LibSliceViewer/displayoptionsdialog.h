#ifndef DISPLAYOPTIONSDIALOG_H
#define DISPLAYOPTIONSDIALOG_H

#include <QDialog>
#include "displayoptions.h"

namespace sliceviewer {


namespace Ui {
class DisplayOptionsDialog;
}

class DisplayOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayOptionsDialog(QWidget *parent = 0);
    ~DisplayOptionsDialog();

    DisplayOptions displayOptions();
    void setDisplayOptions(const DisplayOptions&);


private slots:
    void on_cbLastViewedColor_clicked();

private:
    Ui::DisplayOptionsDialog *ui;
};

}

#endif // DISPLAYOPTIONSDIALOG_H
