#ifndef IVP_H
#define IVP_H

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
const double time_step = 1.00 / (1000); // time step of 10^-3 min for RK4 method

struct PatientState {
    double ISC;                 // subcutaneous insulin concentration (μU/mL)
    double I;                   // plasma insulin concentration (μU/mL)
    double IEFF;                // insulin effect on blood glucose (min^-1)
    double G;                   // blood glucose concentration (mg/dL)
};

double meal_intake(double CH, double time);
void compute_derivatives(struct PatientState *patient, double insulin_infusion, double RA, double dYdt[4]);
void update_temp_state (struct PatientState *base_state, struct PatientState *fill_state, double time_step, double k[4]);
void update_patient_state_rk4(struct PatientState *patient, double insulin_infusion, double CH, double meal_timer);
double adjust_insulin(double G, double target_G, double current_insulin);



#endif