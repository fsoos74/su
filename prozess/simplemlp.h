#include<cmath>
#include<limits>
#include<vector>
#include<functional>    
#include"matrix.h"

inline double sigmoid(double x){
    return 1./(1.+std::exp(-x));
}

// expects sigmoid has already been applied to x!!!!
inline double dx_sigmoid(double x){
    return x*(1.-x);
}

inline double tanh(double x){
    auto f=std::exp(2*x);
    return (f-1.)/(f+1.);
}

// expects tanh already been applied to x
inline double dx_tanh(double x){
    return (1.+x)/(1.-x);
}

// two hidden layers MLP 
class SimpleMLP{

public:

    // nin: number of input, nh1: neurons hidden layer1
    // nh2: neurons hidden layer2, nout number of output neurons 
    SimpleMLP( size_t nin, size_t nh1, size_t nh2, size_t nout):
        m_nin(nin),m_nh1(nh1),m_nh2(nh2),m_nout(nout), 
        m_mm(nin,std::make_pair(0.,1.)){
        initWeights();
    }

    // set activation function and derivative
    // derivative must accept input with activation already applied
    void setSigma(std::function<double(double)> sigma,
                    std::function<double(double)> dx_sigma){
        m_sigma=sigma;
        m_dx_sigma=dx_sigma;
    }

    // nomen est omen
    Matrix<double> predict(Matrix<double> X)const{
        auto XX=prepareInput(X);
        auto y=calOut(XX);
        return y;
    }

    // adjust mlp to best fit the given (training) data.
    // this sets the scaling parameters and then runs the training
    // report training iteration,error via callback
    void fit(Matrix<double> X, Matrix<double> Y,  
                double eta=0.75, size_t  maxiter=200, 
                std::function<void(size_t,double)> 
                    progress=[](size_t,double){return;}){
        adjustScaling(X);
        auto XX=prepareInput(X);    // uses adjusted scaling parameters
        train(XX, Y, eta, maxiter, progress);
    }

protected:

    void adjustScaling(Matrix<double> X){
        for( size_t j=0; j<m_mm.size(); j++){
            auto itmm = std::minmax_element( 
                            X.column_begin(j), X.column_end(j));
            m_mm[j]=std::make_pair(*itmm.first,*itmm.second);
        }
    }

    // scale to 0-1, add bias neuron
    Matrix<double> prepareInput(Matrix<double> X)const{
        //scale input 
        for( size_t i=0; i<X.rows(); i++){
            for( size_t j=0; j<X.columns(); j++){
                X(i,j)=(X(i,j)-m_mm[j].first)/(m_mm[j].second-m_mm[j].first);
            }
        }
        auto XX=addColumn(X,1.);                        // add bias 
        return XX;
    }

    // used internally, expects bias neuron in X
    Matrix<double> calOut(Matrix<double> X)const{
        auto O1=dot(m_w[0],X.transposed());
        O1=O1.apply(m_sigma);
        auto O2=dot(m_w[1],O1);
        O2=O2.apply(m_sigma);
        auto y=dot(m_w[2],O2);  // linear activation
        return y.transposed();
    }

    // used internally, expects bias neuron in X, and X scaled to 0-1
    void train(Matrix<double> X, Matrix<double> Y, 
                double eta, size_t maxiter,
                std::function<void(size_t,double)> progress){
        
        std::vector<Matrix<double>> dW;
        for( size_t i=0; i<m_w.size(); i++){
            dW.push_back(Matrix<double>(m_w[i].rows(), m_w[i].columns(), 0.));
        }

        std::vector<size_t> idx(X.rows());
        for( size_t i=0; i<X.rows(); i++) idx[i]=i;
        double minError=std::numeric_limits<double>::max();
        auto minW=m_w;
        for( size_t iter=0; iter<maxiter; iter++){

            // update weights for all training examples separately
            // use different order of examples in each iteration
            std::random_shuffle(idx.begin(),idx.end()); 
            for( size_t k=0; k<idx.size(); k++){
                auto i=idx[k];
                auto x=X.row(i);
                auto O1=dot(m_w[0],x.transposed());
                O1.apply(m_sigma);
                auto dO1=O1;
                dO1.apply(m_dx_sigma);
                auto O2=dot(m_w[1],O1);
                O2.apply(m_sigma);
                auto dO2=O2;
                dO2.apply(m_dx_sigma);
                auto temp=m_w[2];
                for( size_t j=0; j<temp.columns(); j++){
                    temp(0,j)*=dO2(j,0);
                }
                dW[2]=O2.transposed();  // mean hack
                dW[1]=dot(temp.transposed(),O1.transposed());
                dW[0]=dot((dO1*(dot(temp,m_w[1]).transposed())),x); // was x.T
                auto yp=calOut(x);
                double yfactor=0.;
                for( size_t j=0; j<yp.columns(); j++) yfactor+=Y(i,j)-yp(0,j);
                // finally update weights
                for( size_t j=0; j<m_w.size(); j++){
                    m_w[j]+=(eta*yfactor)*dW[j];
                }
                //std::cout<<x<<" -> "<<yp<<std::endl;
            }

            auto yp=calOut(X);
            double meanE=0.;
            for( size_t j=0; j<yp.size(); j++) meanE+=std::pow(Y[j]-yp[j],2);
            meanE/=yp.size();
            if( meanE<minError){
                minError=meanE;
                minW=m_w;
            }
            progress(iter,meanE);   // report progress
        }

        m_w=minW;
    }

    void initWeights(){
        m_w.push_back(Matrix<double>(m_nh1,m_nin+1));   // +1 for bias
        m_w.push_back(Matrix<double>(m_nh2,m_nh1));
        m_w.push_back(Matrix<double>(m_nout,m_nh2));
        for( auto& w: m_w[0]) w=double(std::rand())/RAND_MAX - 0.5;
        for( auto& w: m_w[1]) w=double(std::rand())/RAND_MAX - 0.5;
        for( auto& w: m_w[2]) w=double(std::rand())/RAND_MAX - 0.5;
    }

private:
    size_t m_nin;
    size_t m_nh1;
    size_t m_nh2;
    size_t m_nout;
    std::vector<std::pair<double,double>> m_mm; // min max value per input value
    std::vector<Matrix<double>> m_w;            // weight matrices for layers
    std::function<double(double)> m_sigma=sigmoid;  // activation function
    std::function<double(double)> m_dx_sigma=dx_sigmoid; // derivative of activation function for input which already has activation function applied!!! 
};


