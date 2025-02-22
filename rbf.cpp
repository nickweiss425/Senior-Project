#include <iostream>
#include <vector>
#include <cmath> 
#include <cstdlib> 
#include <ctime>   

#include "rbf.h"

#define WEIGHT_BOUND 30000.0 // ϑ = 3 × 10^4

// preset centers and widths for each of the hidden neurons
double centers[HIDDEN_NEURONS] = {-25.0, -10.0, -8.0, -5.0, -2.0, 0.0, 5.0, 10.0, 20.0, 40.0, 100.0};
double widths[HIDDEN_NEURONS] = {20.0, 15.0, 10.0, 8.0, 5.0, 5.0, 5.0, 20.0, 40.0, 100.0, 200.0};
const double time_step = 1.00 / (1000); // time step of 10^-3 min for RK4 method

// gaussian activation function for each neuron in the network
double gaussian_activation(double center, double width, double control_error){
    return exp(-0.5 * (pow(((control_error - center) / width), 2)));
}

// calculate the results of the activation function for each neuron
void forward_pass(double control_error, double *activations){
    for (int i = 0; i < HIDDEN_NEURONS; i++){
        activations[i] = gaussian_activation(centers[i], widths[i], control_error);
    }
}


// output of the rbf is a weighted sum of all the activation results
double compute_rbf_output(double *weights, double *activations){
    double sum = 0;
    for (int i = 0; i < HIDDEN_NEURONS; i++){
        // add the product of weight and activation output to the sum
        sum += weights[i] * activations[i];
    }

    return sum;
}


// using current error and activation function outputs, update the weights for each neuron
void update_weights(double learning_rate, double control_error, double *activations, double *weights){
    double euclidean_norm = compute_euclidean_norm(weights);
    double dot_product = compute_dot_product(weights, activations);
    double outer[HIDDEN_NEURONS][HIDDEN_NEURONS];

    
    // projection algorithm to ensure that the weight vector always remains within safe range
    // equation 13 in the paper
    if ((euclidean_norm < WEIGHT_BOUND) || (euclidean_norm == WEIGHT_BOUND && (learning_rate * control_error * dot_product < 0))){
        // cycle through all neurons and update weights
        for (int i = 0; i < HIDDEN_NEURONS; i++){
            weights[i] += learning_rate * control_error * activations[i] * time_step;
        }
    }
    else{
        // product of weight vector transpose and weight vector 
        double wTw_dot = compute_dot_product(weights, weights);

        // calculate outer product matrix divded by product of weight vector transpose and weight vector
        compute_scaled_outer_product(weights, outer, wTw_dot);

        // compute projection matrix by subtracting scaled outer matrix from identity matrix
        double projection[HIDDEN_NEURONS][HIDDEN_NEURONS];
        compute_projection_matrix(projection, outer);

        // Compute the unprojected update: delta = learning_rate * control_error * activations
        double delta[HIDDEN_NEURONS];
        for (int i = 0; i < HIDDEN_NEURONS; i++){
            delta[i] = learning_rate * control_error * activations[i];
        }


        // weight deltas initialized to 0
        double weight_deltas[HIDDEN_NEURONS] = {0};

        // compute weight deltas
        compute_weight_deltas(weight_deltas, projection, delta);

        // add deltas to weights
        for (int i = 0; i < HIDDEN_NEURONS; i++){
            weights[i] += weight_deltas[i] * time_step;
        }
    }
}

void compute_weight_deltas(double weight_deltas[HIDDEN_NEURONS], double projection[HIDDEN_NEURONS][HIDDEN_NEURONS], double delta[HIDDEN_NEURONS]){
    for (int i = 0; i < HIDDEN_NEURONS; i++){
        for (int j = 0; j < HIDDEN_NEURONS; j++){
            weight_deltas[i] += projection[i][j] * delta[j];
        }
    }
}

// subtract scaled outer matrix from identity matrix
void compute_projection_matrix(double projection[HIDDEN_NEURONS][HIDDEN_NEURONS], double scaled_outer[HIDDEN_NEURONS][HIDDEN_NEURONS]){
    for (int i = 0; i < HIDDEN_NEURONS; i++){
        for (int j = 0; j < HIDDEN_NEURONS; j++){
            if (i == j){
                projection[i][j] = 1.0 - scaled_outer[i][j];
            }
            else{
                projection[i][j] = -1.0 * scaled_outer[i][j];
            }
        }
    }
}


// matrix multiplication of weight vector times transpose of weight vector
void compute_scaled_outer_product(double *weights, double outer[HIDDEN_NEURONS][HIDDEN_NEURONS], double scale){
    if (scale == 0.0) scale = 1e-6; 
    for (int i = 0; i < HIDDEN_NEURONS; i++){
        for (int j = 0; j < HIDDEN_NEURONS; j++){
            outer[i][j] = (weights[i] * weights[j]) / scale;
        }
    }
}


// calculate the insulin infusion rate
double control_law(double control_error, double target_glycemia, double disturbance, double control_gain, double f_hat, double b_hat){
    return (1.0 / b_hat) * ((-1.0 * f_hat) - (control_gain * control_error) + disturbance);
}



// calculate the euclidean norm of all the current weights for use in the weight update function
double compute_euclidean_norm(double *weights){
    double sum = 0.0;
    for (int i = 0; i < HIDDEN_NEURONS; i++){
        sum += weights[i] * weights[i];
    }

    return sqrt(sum);
}


// function to compute dot product between weight and activation vectors
double compute_dot_product(double *weights, double *activations){
    double sum = 0;
    for (int i = 0; i < HIDDEN_NEURONS; i++){
        sum += weights[i] * activations[i];
    }
    return sum;
}