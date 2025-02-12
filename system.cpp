#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

#include "write_csv.h"
#include "ivp.h"
#include "rbf.h"


const double time_step = 1.00 / (1000); // time step of 10^-3 min for RK4 method
int sim_time = 1440;            // simulation time of 1440 minutes

/* --------- Meal Schedule (in minutes)---------*/
/* Breakfast: 7:30, Lunch: 12:30, Dinner: 19:30 */

const double meal_duration = 180.00;         // after ~3 hours carbs absorption will be finished

const double breakfast_time = 7.5 * 60;
const double breakfast_carbs = 42.00;

const double lunch_time = 12.5 * 60;
const double lunch_carbs = 66.00;

const double dinner_time = 19.5 * 60;
const double dinner_carbs = 51.00;





int main(int argc, char *argv[]){

    // setup csv file to hold data
    std::string filename = "glucose_data.csv";
    if (argc > 1){
        std::string filename = argv[1];
    }
    // write the column names to file
    writeHeaders(filename);


    /* Information for virtual patient */
    struct PatientState patient = {0, 0, 0, 100};    // initialize patient, with 100 mg/dL
    double insulin_infusion = 0 ; // exmaple insulin infusion
    double meal_timer = 0;
    bool meal_active = false;
    double CH = 0;
    double target_glycemia = 110;  // target glucose = 110 mg/dL

    /* Information for RBF and control law */
    double activations[HIDDEN_NEURONS];
    double weights[HIDDEN_NEURONS] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // start all weights at 0

    int num_calculated = 0;
    for (double time = 0; time < sim_time; time += time_step){

        // check to see if it is time for a meal
        if (std::fabs(time - breakfast_time) < time_step / 2){
            std::cout << "BREAKFAST TIME" << std::endl;
            meal_active = true;
            // carbohydrates in mg
            CH = breakfast_carbs * 1000;
        }
        else if (std::fabs(time - lunch_time) < time_step / 2){
            std::cout << "LUNCH TIME" << std::endl;
            meal_active = true;
            // carbohydrates in mg
            CH = lunch_carbs * 1000;
        }
        else if (std::fabs(time - dinner_time) < time_step / 2){
            std::cout << "DINNER TIME" << std::endl;
            meal_active = true;
            CH = dinner_carbs * 1000;
        }


        // update patient parameters for next time step
        update_patient_state_rk4(&patient, insulin_infusion, CH, meal_timer);
        num_calculated++;
        if (num_calculated == 100) { // Print every 1 minute
            std::cout << "Time: " << time << " min, Glucose Level: " << patient.G << " mg/dL" << std::endl;
            num_calculated = 0;
        }

        appendData(filename, time, patient.G, insulin_infusion);


        // if meal is active, increment timer by set time step
        if (meal_active){
            meal_timer += time_step;
        }

        // when meal duration has passed, reset flag and timer
        if (meal_active && meal_timer >= meal_duration){
            meal_active = false;
            meal_timer = 0;
            CH = 0;
        }

        // get the control error based on current and target glucose
        double control_error = patient.G - target_glycemia;
        forward_pass(control_error, &activations[0]);
        double disturbance = compute_rbf_output(&weights[0], &activations[0]);

        insulin_infusion = control_law(control_error, target_glycemia, disturbance, 1, 0, 1);

    }

    std::string command = "python3 plot_data.py " + filename;
    system(command.c_str());

}
