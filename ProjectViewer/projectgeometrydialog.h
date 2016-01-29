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
    void setProjectGeometry( ProjectGeometry);
    void setProject(std::shared_ptr<AVOProject>);

private slots:
    void updateOkButton();

    void on_pbExtract_clicked();

private:
    Ui::ProjectGeometryDialog *ui;

    std::shared_ptr<AVOProject> m_project;
};

#endif // PROJECTGEOMETRYDIALOG_H
