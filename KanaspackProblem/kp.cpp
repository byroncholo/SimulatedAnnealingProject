/*
This program solve the Kanapsack problem using the proposed Simulated Annealing Algorithm.

Dataset:
knapPI_1_500_1000_1_items.csv

Goal: Maximize the profit

Author: Byron Cholo
Date: 4/30/2026
*/

#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <functional>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

using namespace std;

// Structure for one item
struct Item{
    int id;
    double profit;
    double weight;
};

// Output structure
struct SAResults{
    vector<int> best_solution;
    int best_profit;
    int best_weight;
    int iterations;
};

// Functions prototypes
vector<Item> read_items_from_cvs(const string& filename);
int compute_profit(const vector<int>& solution, const vector<Item>& items);
int compute_weight(const vector<int>& solution, const vector<Item>& items);
double objective_function(const vector<int>& solution, const vector<Item>& items, int capacity);
double acceptance_probability( double delta, double T);

// Repair Function
void repair_solution(
    vector<int>& solution,
    const vector<Item>& items,
    int capacity,
    mt19937& gen
);

SAResults simulatedAnnealing(
    const vector<Item>& items,
    int capacity,
    double T0,
    double alpha,
    int N_steps
);

// Read the dataset
vector<Item> read_items_from_cvs(const string& filename){
    vector<Item> items;
    ifstream file(filename);
    if (file.is_open()){
        cout << "File opened successfully." << endl;
    }
    if (!file.is_open()){
        cerr << "Error opening file." << endl;
        exit(1);
    }
    string line;

    // Skip header: item, price, weight, sol
    getline(file, line);

    while(getline(file, line)){


        stringstream ss(line);
        string id_str, profit_str, weight_str, sol_str;

        getline(ss, id_str, ',');
        getline(ss, profit_str, ',');
        getline(ss, weight_str, ',');
        getline(ss, sol_str, ',');

        Item item;
        item.id = stoi(id_str);
        item.profit = stoi(profit_str);
        item.weight = stoi(weight_str);

        items.push_back(item);
    }

    file.close();

    return items;
};

// compute total profit
int compute_profit(const vector<int>& solution, const vector<Item>& items){
    int total_profit = 0;

    for (int i = 0; i < solution.size(); i++){
        if (solution[i] == 1){
            total_profit += items[i].profit;
        }
    }
    return total_profit;
};

// compute total weight
int compute_weight(const vector<int>& solution, const vector<Item>& items){
    int total_weight  = 0;

    for (int i = 0; i < solution.size(); i++){
        if (solution[i] == 1){
            total_weight += items[i].weight;
        }
    }
    return total_weight;
};

// Objective function
double objective_function(
    const vector<int>& solution,
    const vector<Item>& items,
    int capacity
){
    int total_profit = compute_profit(solution, items);
    int total_weight = compute_weight(solution, items);

    if (total_weight <= capacity){
        return total_profit;
    }
    return 0.0;
}

// Acceptance probability
double acceptance_probability(double delta, double T){
    if (delta >= 0.0)
        return 1.0;
    else
        return exp(delta/T);
};

// Repair solution if it exeeds the capacity
// it randomly removes items until it does not exceed the capacity
void repair_solution(
    vector<int>& solution,
    const vector<Item>& items,
    int capacity,
    mt19937& gen
){
    int total_weight = compute_weight(solution, items);
    int n = solution.size();

    uniform_int_distribution<int> dist_item(0,n-1);

    while(total_weight > capacity){
        int index = dist_item(gen);

        if (solution[index] == 1){
            solution[index] = 0;
            total_weight -= items[index].weight;
        }
    }
}

// Define the objective functions

// solve the Knapsack problem
SAResults simulatedAnnealing(
    const vector<Item>& items,
    int capacity,
    double T0,
    double alpha,
    int N_steps
){
    mt19937 gen(random_device{}());

    int n = items.size();

    uniform_int_distribution<int> dist_binary(0,1);
    uniform_int_distribution<int> dist_item(0,n-1);
    uniform_real_distribution<double> dist_pro(0.0, 1.0);

    // initial solution
    vector<int> current_solution(n);

    for (int i=0; i<n; i++){
        current_solution[i] = dist_binary(gen);
    }

    // Repair the solution if it exceeds the capacity
    repair_solution(current_solution, items, capacity, gen);

    double f_current = objective_function(current_solution, items, capacity);

    // best solution
    vector<int> best_solution = current_solution;
    double best_value = f_current;

    cout << fixed << setprecision(6);

    cout << "Initial solution: " << f_current << endl;
    cout << "Initial weight: " << compute_weight(current_solution, items) << endl;

    double T = T0;
    int iterations = 0;

    for (int i = 0; i<N_steps; i++){
        // Generate a neighbor solution (candidate solution from the current solution)
        vector<int> candidate_solution = current_solution;
        // Neigborhood generation
        //This next line is the most clever line
        // Flip one random item
        int index = dist_item(gen);

        if (candidate_solution[index] == 0)
            candidate_solution[index] = 1;
        else
            candidate_solution[index] = 0;


        // Repair candidate if infeasible
        repair_solution(candidate_solution, items, capacity, gen);

        double f_candidate = objective_function(candidate_solution, items, capacity);

        double delta = f_candidate - f_current;

        // Acceptance probability
        double u = dist_pro(gen);

        if (u < acceptance_probability(delta, T)){
            current_solution = candidate_solution;
            f_current = f_candidate;
        }
        // Update best solution
        if (f_current > best_value){
            best_solution = current_solution;
            best_value = f_current;
        }
        //

        T = T*alpha;
        iterations++;
        // Other stopoing criterion
        // if (T < 1e-12)
        //     break;
        }

        int best_profit =  compute_profit(best_solution, items);
        int best_weight = compute_weight(best_solution, items);

        return {best_solution, best_profit, best_weight, iterations};
    };

// program control

int main(){
    // Problem setup
    // import the dataset

    //string filename = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\KanaspackProblem\\dataset\\knapsack_original_500_items.csv";
    //string filename = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\KanaspackProblem\\dataset\\knapsack_generic_1000_items.csv";
    //string filename = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\KanaspackProblem\\dataset\\knapsack_generic_2000_items.csv";
    // string filename = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\KanaspackProblem\\dataset\\knapsack_generic_3000_items.csv";
    // string filename = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\KanaspackProblem\\dataset\\knapsack_generic_5000_items.csv";
     string filename = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\KanaspackProblem\\dataset\\knapsack_generic_6000_items.csv";


    vector<Item> items = read_items_from_cvs(filename);

    // From the dataset name:
    // knapPI_1_500_1000_1_items.csv
    // 500 items

    int capacity = 2543;

    double T0 = 10000.0;
    double alpha = 0.995;
    int N_steps = 30000;
    
    cout << "Kanaspack capacity: " << capacity << endl;
    ////////////// measure the execution time of the algorithm /////////////
    auto startTime = chrono::high_resolution_clock::now();

    SAResults results = simulatedAnnealing(items, capacity, T0, alpha, N_steps);

    auto endTime = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsedTime = endTime- startTime;

    cout << endl;
  
    cout << "Number of iterations: " << results.iterations << endl;
    cout << "Best profit found: " << results.best_profit << endl;
    cout << "Best weight found: " << results.best_weight << endl;

    cout << "Execution time: " << elapsedTime.count() << " seconds" << endl;

    cout << endl;
    // cout << "Selected items: " << endl;

    // for (int i = 0; i< results.best_solution.size(); i++){
    //     if ( results.best_solution[i] == 1){
    //         cout << "Item " << items[i].id
    //             << " | Profit: " << items[i].profit
    //             << " | Weight: " << items[i].weight
    //             << endl;
    //     }
    // }
    return 0;
}