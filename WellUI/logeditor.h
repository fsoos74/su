#ifndef LOGEDITOR_H
#define LOGEDITOR_H

#include <QMainWindow>
#include <QLabel>
#include <avoproject.h>
#include <log.h>
#include <memory>
#include <functional>


namespace Ui {
class LogEditor;
}

class LogEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit LogEditor(QWidget *parent = 0);
    ~LogEditor();

    bool isDirty()const{
        return m_dirty;
    }

public slots:
    void setProject( AVOProject*);
    void setUWI(const QString&);
    void setLog( std::shared_ptr<Log> );
    void openLog();
    void openLog(QString);


protected:
    void closeEvent(QCloseEvent*);

private slots:

    void on_action_Open_Log_triggered();
    void on_action_Save_Log_As_triggered();

    void on_action_Median_triggered();
    void on_actionMean_Filter_triggered();
    void on_actionClip_triggered();
    void on_action_Invert_triggered();
    void on_actionAdd_Constant_triggered();
    void on_actionMultiply_Constant_triggered();
    void on_actionAdd_Log_triggered();
    void on_actionSubtract_Log_triggered();
    void on_actionMultiply_Log_triggered();
    void on_actionDivide_Log_triggered();

    void on_actionEdit_Name_triggered();
    void on_actionEdit_Unit_triggered();
    void on_actionEdit_Description_triggered();

    void on_actionHistogram_triggered();

    void onXCursorChanged(qreal);
    void onZCursorChanged(qreal);

    void on_actionEdit_Sampling_triggered();

    void on_actionRunning_Average_triggered();

    void on_actionDepth_to_Time_triggered();

private:

    bool logOp( QString opName, QString opAbbr, std::function<double(double)> op);
    bool logValueOp(QString opName, QString opAbbr, std::function<double(double, double)> op );
    bool logLogOp(QString opName, QString opAbbr, std::function<double(double, double)> op );
    void setDirty(bool);
    bool askModifiedProceed( QString title );

    Ui::LogEditor *ui;

    AVOProject* m_project=nullptr;
    QString m_uwi;
    std::shared_ptr<Log> m_log;
    bool m_dirty=false;
};

#endif // LOGEDITOR_H
