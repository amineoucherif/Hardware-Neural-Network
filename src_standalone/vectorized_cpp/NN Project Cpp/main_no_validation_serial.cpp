//
//  main.cpp
//  NN Project Cpp
//
//  Created by Paul-Edouard on 16/10/16.
//  Copyright © 2016 NUS EE4218. All rights reserved.
//

#include <iostream>
#include <math.h>
#include "Dense"

#include "data.h"

#define INPUT_SIZE			13
#define HIDDEN_SIZE			20
#define NUM_LABELS			3

#define WEIGHTS_INIT_EPS	0.12

#define ITERATIONS			5000
#define LEARNING_RATE		0.0005

typedef uint32_t u32;
typedef uint8_t u8;

u32 DATA_SIZE = 178;

using namespace Eigen;
typedef Matrix<u8, Dynamic, Dynamic, Eigen::RowMajor> Data_T;

Matrix<float, HIDDEN_SIZE, INPUT_SIZE+1> Theta1;
Matrix<float, NUM_LABELS, HIDDEN_SIZE+1>  Theta2;

float Sigmoid(float x);

int main()
{
    printf("Creating matrices...\n\r");
    
    Data_T Data(DATA_SIZE, INPUT_SIZE+1);
    
    Data = Eigen::Map<Data_T>( data, DATA_SIZE, INPUT_SIZE+1);
    PermutationMatrix<Dynamic, Dynamic> Perm(DATA_SIZE);
    Perm.setIdentity();
    std::random_shuffle(Perm.indices().data(), Perm.indices().data()+Perm.indices().size());
    Data = Perm * Data;
    
    
    Theta1.setRandom();
    Theta2.setRandom();
    Theta1 *= WEIGHTS_INIT_EPS;
    Theta2 *= WEIGHTS_INIT_EPS;
    

    Matrix<u8, Dynamic, Dynamic> a_1(DATA_SIZE, INPUT_SIZE+1);
    MatrixXf a_2(DATA_SIZE, HIDDEN_SIZE+1);
    MatrixXf z_1(DATA_SIZE, HIDDEN_SIZE);
    MatrixXf z_2(DATA_SIZE, NUM_LABELS);
    MatrixXf h(DATA_SIZE, NUM_LABELS);
    Matrix<u8, Dynamic, Dynamic> y_logic(DATA_SIZE, NUM_LABELS);
    MatrixXf delta_2(DATA_SIZE, HIDDEN_SIZE);
    MatrixXf delta_3(DATA_SIZE, NUM_LABELS);
    u32 i;
    float cost;
    

    printf("Starting initialization before traning......\n\r");
    a_1 = Data;
    a_1.col(0).setOnes();
    a_2.col(0).setOnes();
    
    y_logic.setZero();
    for(i = 0; i < DATA_SIZE; i++)
        y_logic(i,Data(i,0)-1) = 1;
    
    printf("Starting traning...\n\r");
    
    for(i = 0; i < ITERATIONS; i++)
    {
        // Feedforward
        z_1 = a_1.cast<float>() * Theta1.transpose();
        a_2.block(0,1, DATA_SIZE, HIDDEN_SIZE) = z_1.unaryExpr(&Sigmoid);
        z_2 = a_2 * Theta2.transpose();
        h = z_2.unaryExpr(&Sigmoid);
        
        // Gradient
        delta_3 = h - y_logic.cast<float>();
        delta_2 = delta_3 * Theta2.block<NUM_LABELS,HIDDEN_SIZE>(0,1);
        delta_2 = delta_2.cwiseProduct(a_2.block(0,1,DATA_SIZE,HIDDEN_SIZE));
        delta_2 = delta_2.cwiseProduct(MatrixXf::Ones(DATA_SIZE,HIDDEN_SIZE) - a_2.block(0,1,DATA_SIZE,HIDDEN_SIZE));
        
        Theta1 = Theta1 - LEARNING_RATE * delta_2.transpose()*a_1.cast<float>();
        Theta2 = Theta2 - LEARNING_RATE * delta_3.transpose()*a_2.cast<float>();
        
        cost = delta_3.array().pow(2).sum();
        printf("Iteration %u\t Cost: %0.4f\n", i, cost);
    }
    
    Matrix<u8, Dynamic, 1> y_pred(DATA_SIZE);
    float accuracy = 0;
    for(i = 0; i < DATA_SIZE; i++)
    {
        h.row(i).maxCoeff( &y_pred(i,0) );
        accuracy += ((y_pred(i,0)+1) == Data(i,0));
    }
    accuracy *= 100./DATA_SIZE;
    printf("\nTraining Set Accuracy: %f\n", accuracy);
    
    
    printf("Exiting...\n\r");
    return 0;
}

//MatrixXf Predict(

float Sigmoid(float x)
{
    return 1/(1 + std::exp(-x) );
}
