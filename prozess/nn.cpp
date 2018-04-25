#include "nn.h"



#include<vector>
#include<initializer_list>
#include<algorithm>
#include<cmath>
#include<cstdlib>		// random
#include "matrix.h"
#include "nnfunc.h"

NN::NN():m_sizes(1,1){
    initWeightsAndBiases();
}

NN::NN( std::initializer_list<size_t> sizes ): m_sizes(sizes){
    initWeightsAndBiases();
}

NN::NN( std::vector<size_t> sizes ): m_sizes(sizes){
    initWeightsAndBiases();
}

void NN::setWeights( size_t l, const Matrix<double>& w){
       //if( l>layer_count() ) throw std::range_error("invalid layer");
    m_w[l]=w;
}

void NN::setBiases( size_t l, const Matrix<double>& b){
    m_b[l]=b;
}

void NN::setSigma( std::function<double(double)> f){
    m_sigma=f;
}

void NN::setSigmaPrime( std::function<double(double)> f){
    m_sigma_prime=f;
}

void NN::setEta(double e){
    m_eta=e;
}


// input is a row vector, i.e. 1 row
Matrix<double> NN::feed_forward( const Matrix<double>& x )const{

    #ifdef DEBUG
    std::cout<<"x="<<x<<std::endl;
    #endif

    Matrix<double> a=x;

    for( size_t l = 1; l<m_sizes.size(); l++){
        auto z = dot(a,m_w[l]) + m_b[l];
        a=z.apply(m_sigma);

        #ifdef DEBUG
        std::cout<<"layer "<<l<<std::endl<<"z="<<z<<std::endl<<"a="<<a<<std::endl;
        #endif
    }

    return a;
}

// X: one row per dataset, columns=inputs
// Y: one row per result
void NN::train( Matrix<double> X, Matrix<double> Y, size_t niterations){

    int nlayers=static_cast<int>(layer_count());    // need int in back loop
    std::vector<Matrix<double>> z(nlayers);
    std::vector<Matrix<double>> a(nlayers);
    std::vector<Matrix<double>> l_error(nlayers);
    std::vector<Matrix<double>> l_delta(nlayers);

    for( size_t iter = 0; iter<niterations; iter++){

        // forward propagation
        a[0]=z[0]=X;
        for( int l = 1; l<nlayers; l++){
            z[l] = dot(a[l-1],m_w[l]) + m_b[l];
            //a[l]=sigmoid(z[l]);
            a[l]=z[l].apply(m_sigma);
        }

        // backward propagation
        l_error[nlayers-1]=Y-a[nlayers-1];
        //l_delta[nlayers-1]=l_error[nlayers-1]*dx_sigmoid(a[nlayers-1]);
        l_delta[nlayers-1]=l_error[nlayers-1]*a[nlayers-1].apply(m_sigma_prime);

        for( int l=nlayers-2; l>0; l--){
                l_error[l]=dot(l_delta[l+1],m_w[l+1].transposed() );
                //l_delta[l]=l_error[l] * dx_sigmoid(a[l]);
                l_delta[l]=l_error[l] * a[l].apply( m_sigma_prime);
        }

        // update weights and biases
        for( size_t l=1; l<static_cast<size_t>(nlayers); l++){
            m_w[l]+=m_eta*dot( a[l-1].transposed(), l_delta[l]);
            m_b[l]+=m_eta*columnAverage(l_delta[l]);
        }
    }
}

void NN::initWeightsAndBiases(){

    m_w.push_back( Matrix<double>( 1, m_sizes[0], 1 ) ); // not used, easier indices
    m_b.push_back( Matrix<double>( 1, m_sizes[0], 1 ) ); // not used, easier indices

    for( size_t i=1; i<m_sizes.size(); i++){

        Matrix<double> m( m_sizes[i-1] , m_sizes[i] );
        for( size_t idx=0; idx<m.size(); idx++){
            m[idx]=2*double(std::rand())/RAND_MAX-1;	// between -1 and 1
        }
        std::cout<<"i="<<i<<" rows="<<m.rows()<<" cols="<<m.columns()<<std::endl<<std::flush;
        m_w.push_back(m);

        Matrix<double> b( 1 , m_sizes[i] );
        for( size_t idx=0; idx<b.size(); idx++){
            b[idx]=2*double(std::rand())/RAND_MAX-1;	// between -1 and 1
        }
        m_b.push_back(b);
    }

}

double average_error( const NN& nn, Matrix<double> X, Matrix<double> Y){

    auto a = nn.feed_forward(X);
    auto err=a-Y;
    double abssum=0.;
    for( auto x : err ){
        abssum+=fabs(x);
    }
    return abssum/err.size();
}



void train( NN& nn, const Matrix<double>& X, const Matrix<double>& Y,
    const double training_input_ratio, const size_t max_epochs,
    const size_t max_no_reduce, std::function<void(size_t,double)> progress){

    // split training data into input and test data
    std::vector<size_t> indices;
    indices.reserve(X.rows());
    for( size_t i=0; i<X.rows(); i++){
        indices.push_back(i);
    }
    std::random_shuffle(indices.begin(), indices.end());

    size_t ntrain=std::min(X.rows(),static_cast<size_t>(training_input_ratio*X.rows()) );
    size_t ntest=X.rows()-ntrain;

    // extract training data
    Matrix<double> Xtrain(ntrain, X.columns());
    Matrix<double> Ytrain(ntrain, Y.columns());
    for( size_t i=0; i<Xtrain.rows(); i++){
        auto irow=indices[i];
        for( size_t j=0; j<X.columns(); j++){
            Xtrain(i,j)=X(irow,j);
        }
        for( size_t j=0; j<Y.columns(); j++){
            Ytrain(i,j)=Y(irow,j);
        }
    }

    // extract test data
    Matrix<double> Xtest(ntest, X.columns());
    Matrix<double> Ytest(ntest, Y.columns());
    for( size_t i=0; i<Xtest.rows(); i++){
        auto irow=indices[i+ntrain];
        for( size_t j=0; j<X.columns(); j++){
            Xtest(i,j)=X(irow,j);
        }
        for( size_t j=0; j<Y.columns(); j++){
            Ytest(i,j)=Y(irow,j);
        }
    }

    double last_error=std::numeric_limits<double>::max();
    double err=last_error;
    size_t epoch=0;
    size_t n_inc_error=0;
    while(epoch<max_epochs){
        nn.train(Xtrain,Ytrain,1);
        last_error=err;
        err=average_error(nn, Xtest,Ytest);
        epoch++;
        std::cout<<"epoch: "<<epoch<<" err: "<<err<<std::endl<<std::flush;
        if( err>=last_error ){
            n_inc_error++;
            if( n_inc_error>max_no_reduce ) break;
        }
        else{
            n_inc_error=0;
        }

        progress(epoch,err);
    }
}
