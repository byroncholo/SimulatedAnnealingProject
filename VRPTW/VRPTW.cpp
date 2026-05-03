/*
This program implements the Simulated Annealing Algorithm to solve the VRPTW problem.

Dataset: Solomon Dataset. 

Author = Byron Cholo
Date = 4/30/2026
*/

//////////////////////////////


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>
#include <string>
#include <regex>
#include <cmath>
#include <limits>
#include <chrono>


using namespace std;


// custumer structure
struct Customer{
    int id;
    double x;
    double y;
    double demand;
    double readyTime;
    double dueDate;
    double serviceTime;
};

// route structure
struct Route{
    vector<int> customers; // customers ids, excluding depot
    double distance;
    double load;
    double finishTime; // == service time
};

// solution structure
struct Solution{
    vector<int> permutation; // sequence of customers
    vector<Route> routes;
    double totalDistance;
    double cost;
    bool feasible;

};

// SA result structure
struct SAResult{
    Solution bestSolution;
    int iterations;
};
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// function prototypes ////////////////////////////////////

vector<Customer> readCSV(const string& filename);
vector<double> extractNumbers(const string& line);

double euclideanDistance(const Customer& a, const Customer& b);
double acceptanceProbability(double delta, double T);

Route buildRouteFromCustomers(
    const vector<Customer>& custumers,
    const vector<int>& routeCustumers // was modified 11:23 am 5/1/2026
);

bool canAddCustomer(
    const vector<Customer>& customers,
    const Route& route,
    int customerID,
    double vehicleCapacity
);

Solution decodePermutation(
    const vector<Customer>& custumers,
    const vector<int>& permutation,
    double vehicleCapacity
);

Solution generateNeighbor(
    const vector<Customer>& customers,
    const Solution& current,
    double vehicleCapacity,
    mt19937& gen
);

SAResult simulatedAnnealingVRPTW(
    const vector<Customer>& customers,
    double vehicleCapacity,
    double T0,
    double alpha,
    int NSteps
);

void printSolution(const Solution& sol);
///////////////////////////////////////////////////////////////////////////////////////////////

// Extract numerical values from a line
vector<double> extractNumbers(const string& line){

    vector<double> numbers;

    regex pattern("[-+]?[0-9]+\\.?[0-9]+"); // detect decimals with a decimal point

    auto begin = sregex_iterator(line.begin(), line.end(), pattern);
    auto end = sregex_iterator ();

    for (auto it = begin; it != end; ++it){
        numbers.push_back(stod(it->str()));
    }

    return numbers;
}

// Read Dataset
// numerical order
// id x, y, demand, ready_time, due_date, service_time

vector<Customer> readCSV(const string& filename){

    vector<Customer> customers;
    ifstream file(filename);

    if (!file.is_open()){
        cerr << "Error: could not open file. "<< endl;
        cerr << "Path:" << filename << endl;
        exit(1);
    }

    string line;
    // for debugging
    int totalLines = 0;
    int acceptedLines = 0;

    // Skip header
    getline(file, line);

    while (getline(file, line)){
        totalLines++;

        stringstream ss(line);
        string value;
        vector<string> tokens; // look this

        while (getline(ss, value, ',')){
            tokens.push_back(value);
        }

        if (tokens.size() == 7){
            
            Customer c;

            c.id = stoi(tokens[0]);
            c.x = stod(tokens[1]);
            c.y = stod(tokens[2]);
            c.demand = stod(tokens[3]);
            c.readyTime = stod(tokens[4]);
            c.dueDate = stod(tokens[5]);
            c.serviceTime = stod(tokens[6]);

            customers.push_back(c);  
            acceptedLines++;
        }
    }

    file.close();
    // Here is messing some details

    // For debugging pourpuses
    cout << "Total lines read: " << totalLines << endl;
    cout << "Accepted lines: " << acceptedLines << endl;

    if (!customers.empty()){
        cout << "First customer: " << customers.front().id << endl;
        cout << "Last customer: " << customers.back().id << endl;
    }

    return customers;
}

// Euclidean Distance
double euclideanDistance(const Customer& a, const Customer& b){
    double dx = a.x -b.x;
    double dy = a.y - b.y;

    return sqrt(dx*dx + dy*dy);
}

// Acceptance Probability (for minimization)
// delta = candidate_cost - current_cost

// If delta <= 0 accept
// If delta > 0 , the candidate is worse, accept with probability e^(-delta/T)

double acceptanceProbability(double delta, double T){
    if (delta <=0.0)
        return 1.0;
    else
        return exp(-delta/T);
}

// Construct a route from a list of customers
Route buildRouteFromCustomers(
    const vector<Customer>& customers,
    const vector<int>& routeCustomers
){
    Route route;

    route.customers = routeCustomers;
    route.distance = 0.0;
    route.load = 0.0; // load =  carga
    route.finishTime = 0.0; // service time

    int depot = 0;
    int previous = depot;
    double currentTime= 0.0;

    for (int customerID : routeCustomers){
        const Customer& prev = customers[previous];
        const Customer& curr = customers[customerID];

        double travelTime = euclideanDistance(prev, curr);// distance and speed are assumed equivalent

        route.distance += travelTime;// accumulate total distance of the route
        currentTime += travelTime;
        // readyTime =  earliest time service can start; dueTime = latest time service can start
        if (currentTime < curr.readyTime){ // if the vehicle arrives too early, it waits until readyTime
            currentTime = curr.readyTime;
        }

        currentTime += curr.serviceTime;// after arriving the vehicle spends serviceTime
        route.load += curr.demand;// accumulate total demand(load) carried by the vehicle

        previous = customerID; // update previous customer
    }
    // after visiting the last customer, return to the depot
    route.distance += euclideanDistance(customers[previous], customers[depot]);
    currentTime += euclideanDistance(customers[previous], customers[depot]);

    route.finishTime = currentTime;
    return route;
}

// Check whether a customer can be added to the current route
// Constraints checked: vehicle capacity, customer time window, depod return time
bool canAddCustomer(
    const vector<Customer>& customers,
    const Route& route, // current route being constructed
    int customerID, // the customer you want to be added
    double vehicleCapacity // vehicle capacity(which is the same for all vehicles)
){
    int depot = 0; // this is so by assumption

    if (route.load + customers[customerID].demand  > vehicleCapacity)
        return false;
    
    int previous;
    // determines from where the vehicle would travel to the new customer
    if (route.customers.empty())
        previous = depot; // the vehicle is still at the depot
    else
        previous = route.customers.back();// previous location of the last customer
    
    double currentTime;
    // nota that finish time contains the distance from the last customer to the depot
    if (route.customers.empty())
        currentTime = 0.0;
    else{ // Check this!
        currentTime = route.finishTime; // Finish time =time after completing all services + return to the depot
        int last = route.customers.back();
        currentTime -= euclideanDistance(customers[last], customers[depot]);
    }

    double arrivalTime = currentTime + euclideanDistance(customers[previous], customers[customerID]);
    
    double serviceStart =  max(arrivalTime, customers[customerID].readyTime);

    if (serviceStart > customers[customerID].dueDate)
        return false;
    // departure time = hora de salida
    double departureTime = serviceStart + customers[customerID].serviceTime;

    double returnTime = departureTime + euclideanDistance(customers[customerID], customers[depot]);

    if (returnTime > customers[depot].dueDate)
        return false;
    
    return true;
};

// Decode a permutation into feasible routes
// take a sequence of customers and split into feasible routes
Solution decodePermutation(
    const vector<Customer>& customers, // contains all the customers
    const vector<int>& permutation, // Nota: is like a sequence (preserves order). Order in which the customers will be visited
    double vehicleCapacity
){
    Solution sol; // create a solution object

    sol.permutation = permutation; // save the orignal permutation. This is good for applying 2-opt to the permuation
    sol.routes.clear();
    sol.totalDistance = 0.0;
    sol.cost = 0.0;

    Route currentRoute; // create the route being built

    currentRoute.customers.clear();// constains only customers ID, no the depot
    currentRoute.distance = 0.0;
    currentRoute.load = 0.0;
    currentRoute.finishTime = 0.0;

    for (int customerID : permutation){// loop over the permutation
        if (canAddCustomer(customers, currentRoute, customerID, vehicleCapacity)){ // try to add the customer to the current route
            currentRoute.customers.push_back(customerID);// add customer to the current route

            currentRoute = buildRouteFromCustomers(customers, currentRoute.customers);// rebuild the route
        }
        else{ // not feasible then close current route
            if (!currentRoute.customers.empty()) // prevent adding empty route
                sol.routes.push_back(currentRoute);
            // reset current route to start a new route in the next iteration
            currentRoute.customers.clear();
            currentRoute.distance = 0.0;
            currentRoute.load = 0.0;
            currentRoute.finishTime = 0.0;
            // add the customer to the new route
            currentRoute.customers.push_back(customerID);
            // computes the distance, load and finish time of the new route.
            currentRoute = buildRouteFromCustomers(customers, currentRoute.customers);
        }
    }
    // note that the last route is not added to the solution. So we add here.
    if (!currentRoute.customers.empty())
        sol.routes.push_back(currentRoute);
    for (const Route& route : sol.routes)// goes through all constructed routed and sums their distances
        sol.totalDistance += route.distance;
    
    // objective
    sol.cost = sol.totalDistance; //Nota: we are minimizing the distance
    return sol;
}


// generate a neigborhood solution

Solution generateNeighbor(
    const vector<Customer>& customers,
    const Solution& current,
    double vehicleCapacity,
    mt19937& gen
){
    vector<int> newPermutation = current.permutation;

    // shuffle(newPermutation.begin(), newPermutation.end(), gen);   This is other alternative instead of using reversing

    if (newPermutation.size()< 2)
        return current;
    
    uniform_int_distribution<int> op_dist(1, 3); // 1:2-opt, 2:3-opt, 3:4-opt
    uniform_int_distribution<int> index_dist(0, static_cast<int>(newPermutation.size()) -1); // this is important

    int op = op_dist(gen);

    int i = index_dist(gen);
    int j = index_dist(gen);

    if (i>j)
        swap(i,j);
    if (op == 1)
        swap(newPermutation[i], newPermutation[j]);
    else if (op == 2)
        reverse(newPermutation.begin() + i, newPermutation.begin() + j +1);
    else{
        // relocate one customer
        if(i != j){
            int customer = newPermutation[i];

            newPermutation.erase(newPermutation.begin() + i);
            newPermutation.insert(newPermutation.begin() + j, customer);
        }
    }
    return decodePermutation(customers, newPermutation, vehicleCapacity);
}


// SA implementation for VRPTW

SAResult simulatedAnnealingVRPTW(
    const vector<Customer>& customers,
    double vehicleCapacity,
    double T0,
    double alpha,
    int NSteps
){
    mt19937 gen(random_device{}());

    uniform_real_distribution<double> prob_dist(0.0, 1.0);

    vector<int> initialPermutation;

    for (int i = 1; i < static_cast<int>(customers.size()); i++)
        initialPermutation.push_back(i);
    
    shuffle(initialPermutation.begin(), initialPermutation.end(), gen);

    Solution current = decodePermutation(customers, initialPermutation, vehicleCapacity);

    Solution best = current;

    double T  = T0;
    int iterations = 0;

    cout << fixed << setprecision(4);

    cout << "Initial solution: " << endl;
    cout << "Vehicles: " << current.routes.size() << endl;
    cout << "Distance: " << current.cost << endl;
    cout << "Cost: " << current.cost << endl;
    cout << endl;

    for (int k = 0; k < NSteps; k++){
        Solution candidate = generateNeighbor(customers, current, vehicleCapacity,gen);

        double delta = candidate.cost - current.cost;

        double u = prob_dist(gen);

        if (u < acceptanceProbability(delta, T))
            current = candidate;
        if (candidate.cost < best.cost)
            best = candidate;

        T = T*alpha;
        iterations++;

        // if (T<1e-12)
        //     break;
        // it could be printed the progress
    }

    return {best, iterations};
}

void printSolution(const Solution& sol){
    cout << fixed << setprecision(4);

    cout << endl;
    cout << "======== Solution ============" << endl;
    cout << "Vehicles: " << sol.routes.size() << endl;
    cout << "Total distance: " << sol.totalDistance << endl;
    cout << "Total cost: " << sol.cost << endl;
    cout << "==============================" << endl;
    // For debug
    // for (int r = 0; r < static_cast<int>(sol.routes.size()); r++){
    //     const Route& route = sol.routes[r];

    //     cout << "Route " << r + 1 << ": ";
    //     cout << "0 -> ";

    //     for (int customerID : route.customers)
    //         cout << customerID << "-> ";
        
    //     cout << "0";

    //     cout << " | Load: " << route.load;
    //     cout << " | Distance: " << route.distance;
    //     cout << " | Finish Time: " << route.finishTime;
    //     cout << endl;
    // }

}

// control

int main(){
    
    //string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\VRPTW\\dataset\\C101.csv";
    // string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\VRPTW\\dataset\\C101_synthetic_500.csv";
    // string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\VRPTW\\dataset\\C101_synthetic_1500.csv";
    // string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\VRPTW\\dataset\\C101_synthetic_2000.csv";
    // string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\VRPTW\\dataset\\C101_synthetic_2500.csv";
    // string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\VRPTW\\dataset\\C101_synthetic_3000.csv";
     string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\VRPTW\\dataset\\C101_synthetic_4000.csv";

    vector<Customer> customers = readCSV(filePath);

    cout << "Number of customers: " << customers.size() << endl;

    double vehicleCapacity = 2000.0;

    double T0 = 10000.0;
    double alpha = 0.995;
    int NSteps = 20000;

    // start measuring execution time
    auto start = chrono::high_resolution_clock::now();

    SAResult results = simulatedAnnealingVRPTW(customers, vehicleCapacity, T0, alpha, NSteps);

    auto end  = chrono::high_resolution_clock::now();

    chrono::duration<double> duration = end - start;

    cout << endl;
    cout << fixed << setprecision(4);
    cout << "Number of iterations: " << results.iterations << endl;
    cout << "Execution time: " << duration.count() << " seconds" << endl;

    printSolution(results.bestSolution);

    return 0;
}