#ifndef PROJECTGEOMETRYDIALOG_H
#define PROJECTGEOMETRYDIALOG_H

#include <QDialog>

#include<avoproject.h>
#include<memory>
#include<projectgeometry.h>

namespace Ui {
class ProjectGeometryDialog;
}

class ProjectGeometryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectGeometryDialog(QWidget *parent = 0);
    ~ProjectGeometryDialog();

    ProjectGeometry projectGeometry();
    bool isUpdateAxes();
    void setProjectGeometry( ProjectGeometry);
    void setProject(AVOProject*);

private slots:
    void updateOkButton();

    void on_pbExtract_clicked();

private:
    Ui::ProjectGeometryDialog *ui;

    AVOProject* m_project=nullptr;
};

#endif // PROJECTGEOMETRYDIALOG_H
