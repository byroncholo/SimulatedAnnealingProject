/*
This program solves the traveling salesman problem with the help of the simulated 
annealing algorithm.

Dataset: att48.tsp

Format:
  EDGE_WEIGHT_TYPE : ATT
  NODE_COORD_SECTION

Author: Byron Cholo
Date: 4/30/2026
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <chrono> // for measuring execution time


using namespace std;

// structure for a 2D city coordinate
struct Point{
  double x_1;
  double x_2;
};

struct SAResults{
  vector<int> bestTour; // store the path
  double bestValue; // store distance best path
  int iterations; // store number of iterations to reach the solution
};
//////////////////////////////////////////////////////////
////////////// Function prototypes ///////////////////////
/////////////////////////////////////////////////////////
vector<Point> readTSPFile(const string& filePath);

vector<vector<int>> DistanceMatrix(const vector<Point>& cities); //
int attDistance(const Point& a, const Point& b);


double objectiveFunction(const vector<int>& tour, const vector<vector<int>>& distMatrix
);

double acceptanceProbability(double delta, double T);

SAResults simulatedAnnealing(const vector<vector<int>>& distMatrix, 
                            double T0, double alpha, int NSteps, int startCity);

/////////////////////////////////////////////////////////////////
// Read coordinates from att48.tsp
vector<Point> readFile(const string& filePath){

  ifstream file(filePath);

  if (!file.is_open()){
    cerr << "Error: file could not be opened" << endl;
    exit(1);
  }

  vector<Point> cities;
  string line;
  bool readingCoordinates = false;

  while (getline(file, line)){
      if (line == "NODE_COORD_SECTION"){
        readingCoordinates = true;
        continue;
      }

      if (line == "EOF")
        break;

      if (readingCoordinates){
        stringstream ss(line);

        int cityID;
        double x, y;

        ss >> cityID >> x >> y;

        if (!ss.fail()){
          Point p;
          p.x_1 = x;
          p.x_2 = y;

          cities.push_back(p);
      }
    }
  }

  file.close();

  return cities;
}

/////////////////////////////////
// ATT pseudo-Euclidean distance 
// EDGE_WEIGHT_TYPE: ATT is define as integer weight 
int attDistance(const Point& a, const Point& b){

  double dx = a.x_1 - b.x_1;
  double dy = a.x_2 - b.x_2;

  double rij = sqrt((dx*dx + dy*dy)/10.0);

  int tij =  static_cast<int>(round(rij));

  if (tij < rij)
    return tij + 1;
  else
    return tij;
}

// Construct symetric distance matrix
vector<vector<int>> buildDistanceMatrix(const vector<Point>& cities){
  int n = cities.size();

  vector<vector<int>> distMatrix(n, vector<int>(n, 0));

  for (int i = 0; i < n; i++){
    for (int j = 0; j < n; j++){
      if (i==j)
        distMatrix[i][j] = 0;
      else
        distMatrix[i][j] = attDistance(cities[i], cities[j]);
    }
  }

  return distMatrix;
};

// Objective function: total length of a (closed) TSP tour
double objectiveFunction(const vector<int>& tour,
                        const vector<vector<int>>& distMatrix){
    double totalDistance = 0.0;
    int n = tour.size();

    for (int i=0; i<n-1; i++){
      totalDistance += distMatrix[tour[i]][tour[i+1]];
    }

    // return from last city to first city
    totalDistance += distMatrix[tour[n-1]][tour[0]];
    
    return totalDistance;
}

// Acceptance probability (for minimization)
double acceptanceProbability(double delta, double T){
  if (delta <= 0.0)
    return 1.0;
  else  
    return exp(-delta/T);
}

////////////////////////////////////

SAResults simulatedAnnealing(const vector<vector<int>>& distMatrix,
  double T0, double alpha, int NSteps, int startCity
)
{
mt19937 gen(random_device{}()); // carefully with this!

int n = distMatrix.size();

// convert real city number into the enviroment
startCity = startCity - 1;

// construct initial tour with the start city at the beginning
vector<int> currentTour;
// add the start city
currentTour.push_back(startCity);

// add the rest of the cities
for (int i = 0; i<n; i++){
  if (i != startCity)
    currentTour.push_back(i);
}

// make the trivial solution a bit random
shuffle(currentTour.begin() + 1, currentTour.end(), gen);
//cout << " checkpoint 2" << endl;
double fCurrent = objectiveFunction(currentTour, distMatrix);

// best solution
vector<int> bestTour = currentTour;
double bestValue = fCurrent;

cout << fixed << setprecision(6);
cout << "Initial tour length: " << fCurrent << endl;

// random distribution
uniform_real_distribution<double> dist_pro(0.0, 1.0);
// nota: index 0 is fixed, so only choose positions from 1 to n-1.
uniform_int_distribution<int> dist_index(1, n-1);

// initial temperature
double T = T0;

// iterations
int iterations = 0;

// Print the initial solution tour
  for (int city: currentTour){
    cout << city + 1 << " ";
  }

// close the tour by returning to the first city
cout << currentTour[0] + 1 << endl;



for (int i = 0; i< NSteps; i++){
  iterations++;
  // candidate to solution
  vector<int> candidateTour = currentTour;

  // generate two random positions (check this!)
  int a =  dist_index(gen);
  int b =  dist_index(gen);

  if (a>b)
    swap(a,b);
  
  if (a==b)
    continue;
  
  // Generating neighbor solution by reversing a section of the tour

  reverse(candidateTour.begin() + a, candidateTour.begin() + b + 1);

  double fCandidate = objectiveFunction(candidateTour, distMatrix);

  // Nota: delta <= 0, accept
  //       delta > 0, accept with probability e^(-delta/T)

  double delta = fCandidate - fCurrent;

  double u = dist_pro(gen);

  if (u < acceptanceProbability(delta, T)){
    currentTour = candidateTour;
    fCurrent = fCandidate;
  } 

  // update best solution
  if (fCandidate < bestValue){
    bestTour = currentTour;
    bestValue = fCandidate;
  }

  // cooling schedule
  T = T*alpha;

  //iterations++;
  // improvement tool
  // if (T < 1e-12)
  //   break;
   }

  return {bestTour, bestValue, iterations};
}

int main(){
  //  cout << " checkpoint 00" << endl;
  // File path
  //string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\TravelingSalesmanProblem\\dataset\\d2\\att48.tsp";
  // string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\TravelingSalesmanProblem\\dataset\\d2\\att_random_100.tsp";
  // string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\TravelingSalesmanProblem\\dataset\\d2\\att_random_300.tsp";
  //string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\TravelingSalesmanProblem\\dataset\\d2\\att_random_600.tsp";
   // string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\TravelingSalesmanProblem\\dataset\\d2\\att_random_800.tsp";
   //string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\TravelingSalesmanProblem\\dataset\\d2\\att_random_1200.tsp";
   string filePath = "E:\\1_Semester_I_2026\\Algorithms\\SimulatingAnnealingProyect\\Presentation II\\Code\\TravelingSalesmanProblem\\dataset\\d2\\att_random_1800.tsp";


  //cout << " checkpoint 0" << endl;

  // read cities from file
  vector<Point> cities = readFile(filePath);

  cout << "Number of cities: " << cities.size() << endl;

  if (cities.empty()){
    cerr << "Error: no cities read form file " << endl;
    return 1;
   }

  // Build distance matrix using ATT distance
  vector<vector<int>> distMatrix = buildDistanceMatrix(cities);

  // parameters
  double T0 = 10000.0;
  double alpha = 0.995;
  int NSteps = 100000;

  // starting city
  int startCity = 1;

  //cout << " checkpoint 1" << endl;

  // measure execution time
  auto start = chrono::high_resolution_clock::now();

  // SA call
  SAResults results = simulatedAnnealing(distMatrix, T0, alpha, NSteps, startCity);

  auto end = chrono::high_resolution_clock::now();

  chrono::duration<double> elapsed = end - start;


  cout << endl;
  cout << "Number of iterations: " << results.iterations << endl;
  cout << "Best tour distance found: " << results.bestValue << endl;
  cout << "Execution time: " << elapsed.count() << endl;

  cout << "Best tour starting from city " << startCity << ": "<< endl;

  for (int city: results.bestTour){
    cout << city + 1 << " "; // recall that in cpp, positions start at : 0,1,2,3,.... city = 0 -> real city = 1
  } 
  // close the tour(by returning to the first city)
  cout << results.bestTour[0] + 1 << endl;

  return 0;
}
















