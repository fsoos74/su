#ifndef NNVOLUMETRAINER_H
#define NNVOLUMETRAINER_H

#include <avoproject.h>
#include <table.h>
#include <volumereader2.h>
#include <QDialog>

#include"nn.h"

namespace Ui {
class NNVolumeTrainer;
}

class NNVolumeTrainer : public QDialog
{
    Q_OBJECT

public:
    explicit NNVolumeTrainer(QWidget *parent = 0);
    ~NNVolumeTrainer();

public slots:
    void setProject(AVOProject*);

private slots:
    void setRunning(bool);
    void setValidNN(bool);
    void invalidateNN();
    void on_pbRun_clicked();
    void on_pbSave_clicked();
    void on_pbStop_clicked();
    void refreshScene();

private:

    void buildNN();
    void prepareTraining();
    void runTraining();
    void getParamsFromControls();
    void error(QString msg);

    Ui::NNVolumeTrainer *ui;

    AVOProject* m_project=nullptr;

    QString m_matchingName;
    QString m_nonMatchingName;
    QStringList m_inputNames;
    unsigned m_hiddenNeurons;
    unsigned m_trainingEpochs;
    double m_learningRate;
    unsigned m_apertureLines=1;
    unsigned m_maxNoDecrease=3;
    unsigned m_miniBatchSize=100;
    std::shared_ptr<Table> m_matching;
    std::shared_ptr<Table> m_nonMatching;
    std::vector<VolumeReader2*> m_inputVolumeReaders;

    Matrix<double> m_X;
    Matrix<double> m_Y;
    std::vector< std::pair<double,double> > m_Xmm;
    std::pair<double,double> m_Ymm;
    NN m_nn;
    bool m_running=false;
    bool m_validNN=false;
    QVector<double> m_errors;
};

#endif // NNVOLUMETRAINER_H
