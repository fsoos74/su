#ifndef NNLOGTRAINER_H
#define NNLOGTRAINER_H

#include <avoproject.h>

#include<QDialog>

#include"nn.h"

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
    void updateInputAndPredicted(QString well);
    void setRunning(bool);
    void setValidNN(bool);
    void invalidateNN();
    void on_pbRun_clicked();
    void on_pbSave_clicked();
    void on_pbStop_clicked();
    void on_pbTest_clicked();

private:

    void buildNN();
    void prepareTraining();
    void runTraining();
    void prepareTest();
    void runTest();
    void getParamsFromControls();
    void error(QString msg);

    Ui::NNLogTrainer *ui;

    AVOProject* m_project=nullptr;

    QString m_well;
    QStringList m_inputNames;
    std::vector<std::shared_ptr<Log>> m_inputLogs;
    QString m_predictedName;
    unsigned m_hiddenNeurons;
    unsigned m_trainingEpochs;
    double m_trainingRatio;
    double m_learningRate;
    unsigned m_maxNoDecrease=3;
    unsigned m_filterlen=0;

    Matrix<double> m_X;
    Matrix<double> m_Y;
    std::vector< std::pair<double,double> > m_Xmm;
    std::pair<double,double> m_Ymm;
    NN m_nn;
    bool m_running=false;
    bool m_validNN=false;
    QString m_outputLogName;
    std::shared_ptr<Log> m_outputLog;
};

#endif // NNLOGTRAINER_H
