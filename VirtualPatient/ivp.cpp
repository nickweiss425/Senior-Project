#include <iostream>
#include <cmath>
#include <vector>

/* Constant system parameters defined by paper under Materials/Methods */
const double tau1 = 49.00;      // pharmacokinetic time constants (min)
const double tau2 = 47.00;      // pharmacokinetic time constants (min)
const double CI = 2010.00;      // insulin clearance (mL/min)
const double p2 = 1.06e-2;      // time constant for insulin action (min^-1)
const double SI = 8.11e-4;      // insulin sensitivity (mL / μU / min)
const double GEZI = 2.20e-3;    // Glucose effectiveness at zero insulin (min^-1)
const double EGP = 1.33;        // endogenous glucose production (mg/dL/min)
const double taum = 40.50;      // the peak time of meal glucose appearance (min)
const double VG = 253.00;       // glucose distribution volume (dL)

const double time_step = 10e-3; // time step of 10^-3 min for RK4 method
int sim_time = 300;             // simulation time of 300 minutes


struct PatientState {
    double ISC;                 // subcutaneous insulin concentration (μU/mL)
    double I;                   // plasma insulin concentration (μU/mL)
    double IEFF;                // insulin effect on blood glucose (min^-1)
    double G;                   // blood glucose concentration (mg/dL)
};


// helper function to return RA, the rate of glucose appearance (mg/dL/min)
// simulates increase in blood glucose due to food intake
double meal_intake(double CH, double time){
    // equation (2) in paper
    return (CH * time / (VG * taum * taum)) * exp(-1 * time / taum);
}


// calculate time derivatives of 4D vector, given in equation (1)
void compute_derivatives(struct PatientState *patient, double insulin_infusion, double time, double RA, double dYdt[4]){
    // time derivative of subcutaneous insulin concentration
    dYdt[0] = ((-1 * patient->ISC)) / tau1 + ((insulin_infusion) / (tau1 * CI));

    // time derivative of plasma insulin concentration
    dYdt[1] = ((patient->ISC) / tau2) - ((patient->I) / tau2);

    // time derivative of insulin effect on blood glucose
    dYdt[2] = (-1 * p2 * (patient->IEFF)) + (p2 * SI * (patient->I));

    // time derivative of blood glucose concentration
    dYdt[3] = (-1) * (GEZI + (patient->IEFF)) * (patient->G) + EGP + RA;
}


void update_patient_state_rk4(struct PatientState *patient, double insulin_infusion, double CH, double time){
    double k1[4], k2[4], k3[4], k4[4];

    // 
    compute_derivatives(patient, insulin_infusion, CH, time, k1);





}


int main(){
    struct PatientState patient = {0, 0, 0, 100};    // initialize patient, with 100 mg/dL

    double insulin_infusion = 1.0; // exmaple insulin infusion
    double CH  = 45;

    double t;
    for (t = 0; t < sim_time; t += time_step){
        

    }

}

