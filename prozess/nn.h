#ifndef _NN_H
#define _NN_H

#include<vector>
#include<initializer_list>
#include<functional>
#include<algorithm>
#include<cmath>
#include<cstdlib>		// random
#include "matrix.h"
#include "nnfunc.h"



class NN{

public:

    NN();
    NN( std::initializer_list<size_t> sizes );
    NN( std::vector<size_t> sizes );

    size_t layer_count()const{
        return m_sizes.size();
    }

    size_t layer_size(size_t l)const{
        return m_sizes.at(l);
    }

    Matrix<double> weights( size_t l )const{
        return m_w.at(l);
    }

    Matrix<double> biases( size_t l )const{
        return m_b.at(l);
    }

    std::function<double(double)> sigma()const{
       return m_sigma;
   }

   std::function<double(double)> sigma_prime()const{
       return m_sigma_prime;
   }

   double eta()const{
       return m_eta;
   }


   void setWeights( size_t l, const Matrix<double>& w);
   void setBiases( size_t l, const Matrix<double>& b);
   void setSigma( std::function<double(double)> f);
   void setSigmaPrime( std::function<double(double)> f);
   void setEta(double e);


	// input is a row vector, i.e. 1 row
    Matrix<double> feed_forward( const Matrix<double>& x )const;

    // X: one row per dataset, columns=inputs
    // Y: one row per result
    void train( Matrix<double> X, Matrix<double> Y, size_t niterations=1000);

private:

    void initWeightsAndBiases();

    std::vector<size_t> m_sizes;          // neurons per layer
    std::vector< Matrix<double> > m_w;      // weights
    std::vector< Matrix<double> > m_b;      // biases
    std::function<double(double)> m_sigma=sigmoid;      // activation function
    std::function<double(double)> m_sigma_prime=dx_sigmoid; // first derivative of a function
    double m_eta=0.01;
};


double average_error( const NN& nn, Matrix<double> X, Matrix<double> Y);

void train( NN& nn, const Matrix<double>& X, const Matrix<double>& Y,
    const double training_input_ratio=0.5, const size_t max_epochs=1000,const size_t max_no_reduce=3, 
            std::function<void(size_t,double)> progress=[](size_t,double){return;});


void train_stochastic_gradient_descent( NN& nn, const Matrix<double>& X, const Matrix<double>& Y,
            const int mini_batch_size, const int max_epochs,
            std::function<void(size_t,double)> progress=[](size_t,double){return;});


#endif

