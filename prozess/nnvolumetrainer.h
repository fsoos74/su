#ifndef NNVOLUMETRAINER_H
#define NNVOLUMETRAINER_H

#include <avoproject.h>
#include <table.h>
#include <volumereader2.h>
#include <QDialog>

#include"simplemlp.h"


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
    void setProgress(size_t,double);
    void setValidNN(bool);
    void invalidateNN();
    void on_pbRun_clicked();
    void on_pbSave_clicked();
    void refreshScene();

    void on_pbStop_clicked();

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
    std::shared_ptr<Table> m_matching;
    std::shared_ptr<Table> m_nonMatching;
    std::vector<VolumeReader2*> m_inputVolumeReaders;

    Matrix<double> m_X;
    Matrix<double> m_Y;
    SimpleMLP m_nn;
    bool m_validNN=false;
    QVector<double> m_errors;
    bool m_running=false;
    int m_ilAperture=1;     // number of inlines
    int m_xlAperture=1;     // number of xlines
};

#endif // NNVOLUMETRAINER_H
