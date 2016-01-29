#ifndef ORIENTATIONDIALOG_H
#define ORIENTATIONDIALOG_H

#include <QDialog>

#include "axxisorientation.h"


namespace Ui {
class OrientationDialog;
}

class OrientationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OrientationDialog(QWidget *parent = 0);
    ~OrientationDialog();

    AxxisOrientation inlineOrientation()const;
    AxxisDirection inlineDirection()const;
    AxxisDirection crosslineDirection()const;

public slots:

    void setInlineOrientation( AxxisOrientation);
    void setInlineDirection( AxxisDirection);
    void setCrosslineDirection( AxxisDirection);
    void setOrientationParams( AxxisOrientation ilOrientation, AxxisDirection ilDirection, AxxisDirection xlDirection);

signals:

    void inlineOrientationChanged( AxxisOrientation);
    void inlineDirectionChanged( AxxisDirection);
    void crosslineDirectionChanged( AxxisDirection);

private slots:
    void on_cbInlineOrientation_currentIndexChanged(int index);

    void on_cbInlineDirection_currentIndexChanged(int index);

    void on_cbCrosslineDirection_currentIndexChanged(int index);

private:
    Ui::OrientationDialog *ui;
};

#endif // ORIENTATIONDIALOG_H
