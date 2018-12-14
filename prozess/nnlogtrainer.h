#ifndef NNLOGTRAINER_H
#define NNLOGTRAINER_H

#include <avoproject.h>
#include <table.h>
#include <QDialog>

#include"simplemlp.h"


namespace Ui {
class NNLogTrainer;
}

class NNLogTrainer : public QDialog
{
    Q_OBJECT

public:
    explicit NNLogTrainer(QWidget *parent = 0);
    ~NNLogTrainer();

public slots:
    void setProject(AVOProject*);

private slots:
    void setRunning(bool);
    void setProgress(size_t,double);
    void setValidNN(bool);
    void invalidateNN();
    void on_pbRun_clicked();
    void on_pbSave_clicked();
    void refreshScene();

    void on_pbStop_clicked();
    void updateLogs(QString well);

private:

    void buildNN();
    void prepareTraining();
    void runTraining();
    void getParamsFromControls();
    void error(QString msg);

    Ui::NNLogTrainer *ui;

    AVOProject* m_project=nullptr;

    QString m_well;
    QString m_predictedName;
    QStringList m_inputNames;
    unsigned m_hiddenNeurons;
    unsigned m_trainingEpochs;
    double m_learningRate;
    std::shared_ptr<Log> m_predicted;
    std::vector<std::shared_ptr<Log>> m_inputs;

    Matrix<double> m_X;
    Matrix<double> m_Y;
    SimpleMLP m_nn;
    bool m_validNN=false;
    QVector<double> m_errors;
    bool m_running=false;
};

#endif // NNLOGTRAINER_H
