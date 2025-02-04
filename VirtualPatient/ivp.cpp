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
int sim_time = 1440;            // simulation time of 300 minutes


/* --------- Meal Schedule (in minutes)---------*/
/* Breakfast: 7:30, Lunch: 12:30, Dinner: 19:30 */

const double meal_duration = 180.00;         // after ~3 hours carbs absorption will be finished

const double breakfast_time = 7.5 * 60;
const double breakfast_carbs = 42.00;

const double lunch_time = 12.5 * 60;
const double lunch_carbs = 66.00;

const double dinner_time = 19.5 * 60;
const double dinner_carbs = 51.00;


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
void compute_derivatives(struct PatientState *patient, double insulin_infusion, double RA, double dYdt[4]){
    // time derivative of subcutaneous insulin concentration
    dYdt[0] = ((-1 * patient->ISC)) / tau1 + ((insulin_infusion) / (tau1 * CI));

    // time derivative of plasma insulin concentration
    dYdt[1] = ((patient->ISC) / tau2) - ((patient->I) / tau2);

    // time derivative of insulin effect on blood glucose
    dYdt[2] = (-1 * p2 * (patient->IEFF)) + (p2 * SI * (patient->I));

    // time derivative of blood glucose concentration
    dYdt[3] = (-1) * (GEZI + (patient->IEFF)) * (patient->G) + EGP + RA;
}


// fills the fill_state structure with the calculations of the 4 differential equations 
void update_temp_state (struct PatientState *base_state, struct PatientState *fill_state, double time_step, double k[4]){
    fill_state -> ISC = base_state -> ISC + ((time_step) * k[0]);
    fill_state -> I = base_state -> I + ((time_step) * k[1]);
    fill_state -> IEFF = base_state -> IEFF + ((time_step) * k[2]);
    fill_state -> G = base_state -> G + ((time_step) * k[3]);
}

void update_patient_state_rk4(struct PatientState *patient, double insulin_infusion, double CH, double meal_timer){
    // simulate meal
    double RA = meal_intake(CH, meal_timer);

    // need 4 arrays of 4 components
    // each index in the array corresponds with one of the differential equations
    double k1[4], k2[4], k3[4], k4[4];

    // temporary patient state structure to hold intermediate values
    struct PatientState temp;

    // compute normal derivative to get k1
    compute_derivatives(patient, insulin_infusion, RA, k1);

    // k2 
    update_temp_state(patient, &temp, time_step / 2, k1);
    compute_derivatives(&temp, insulin_infusion, RA, k2);

    // k3
    update_temp_state(patient, &temp, time_step / 2, k2);
    compute_derivatives(&temp, insulin_infusion, RA, k3);

    // k4
    update_temp_state(patient, &temp, time_step, k3);
    compute_derivatives(&temp, insulin_infusion, RA, k4);

    // calculate the values for the 4D vector field at t = time + time_step
    patient -> ISC = (patient -> ISC) + ((time_step / 6) * (k1[0] + (2 * k2[0]) + (2 * k3[0]) + k4[0])); 
    patient -> I = (patient -> I) + ((time_step / 6) * (k1[1] + (2 * k2[1]) + (2 * k3[1]) + k4[1])); 
    patient -> IEFF = (patient -> IEFF) + ((time_step / 6) * (k1[2] + (2 * k2[2]) + (2 * k3[2]) + k4[2])); 
    patient -> G = (patient -> G) + ((time_step / 6) * (k1[3] + (2 * k2[3]) + (2 * k3[3]) + k4[3])); 
}


int main(){
    struct PatientState patient = {0, 0, 0, 100};    // initialize patient, with 100 mg/dL

    double insulin_infusion = 16667 ; // exmaple insulin infusion
    double meal_timer = 0;
    bool meal_active = false;

    double CH = 0;

    int num_calculated = 0;
    for (double time = 0; time < sim_time; time += time_step){

        // check to see if it is time for a meal
        if (std::fabs(time - breakfast_time) < time_step / 2){
            std::cout << "BREAKFAST TIME" << std::endl;
            meal_active = true;
            CH = breakfast_carbs;
        }
        else if (std::fabs(time - lunch_time) < time_step / 2){
            std::cout << "LUNCH TIME" << std::endl;
            meal_active = true;
            CH = lunch_carbs;
        }
        else if (std::fabs(time - dinner_time) < time_step / 2){
            std::cout << "DINNER TIME" << std::endl;
            meal_active = true;
            CH = dinner_carbs;
        }



        update_patient_state_rk4(&patient, insulin_infusion, CH, meal_timer);
        num_calculated++;
        if (num_calculated == 100) { // Print every 1 minute
            std::cout << "Time: " << time << " min, Glucose Level: " << patient.G << " mg/dL" << std::endl;
            num_calculated = 0;
        }


        // if meal is active, increment timer by set time step
        if (meal_active){
            meal_timer += time_step;
        }

        // when meal duration has passed, reset flag and timer
        if (meal_active && meal_timer >= meal_duration){
            meal_active = false;
            meal_timer = 0;
        }

    }

}

