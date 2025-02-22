#ifndef RBF_H
#define RBF_H

#define HIDDEN_NEURONS 11
void forward_pass(double control_error, double *activations);
double compute_rbf_output(double *weights, double *activations);
double gaussian_activation(double center, double width, double control_error);
void update_weights(double learning_rate, double control_error, double *activations, double *weights);
double control_law(double control_error, double target_glycemia, double disturbance, double control_gain, double f_hat, double b_hat);
double compute_euclidean_norm(double *weights);
double compute_dot_product(double *weights, double *activations);
void compute_scaled_outer_product(double *weights, double outer[HIDDEN_NEURONS][HIDDEN_NEURONS], double scale);
void compute_projection_matrix(double projection[HIDDEN_NEURONS][HIDDEN_NEURONS], double scaled_outer[HIDDEN_NEURONS][HIDDEN_NEURONS]);
void compute_weight_deltas(double weight_deltas[HIDDEN_NEURONS], double projection[HIDDEN_NEURONS][HIDDEN_NEURONS], double delta[HIDDEN_NEURONS]);

#endif