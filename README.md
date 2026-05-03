# Simulated Annealing Project

This repository contains several implementations of the **Simulated Annealing (SA)** metaheuristic applied to different optimization problems. The project is written in **C++**.

The implemented problems include:

1. Benchmark function optimization *
2. 0/1 Knapsack Problem
3. Traveling Salesman Problem (TSP)
4. Vehicle Routing Problem with Time Windows (VRPTW)

   
* This is not our focus but it put anyways :)
---

## Table of Contents

- [Project Description](#project-description)
- [Repository Structure](#repository-structure)
- [Simulated Annealing Overview](#simulated-annealing-overview)
- [Implemented Problems](#implemented-problems)
  - [1. Benchmark Function Optimization](#1-benchmark-function-optimization)
  - [2. Knapsack Problem](#2-knapsack-problem)
  - [3. Traveling Salesman Problem](#3-traveling-salesman-problem)
  - [4. Vehicle Routing Problem with Time Windows](#4-vehicle-routing-problem-with-time-windows)
- [Requirements](#requirements)
- [Compilation](#compilation)
- [Execution](#execution)
- [Datasets](#datasets)
- [Output](#output)
- [Author](#author)

---

## Project Description

The goal of this project is to study and implement the **Simulated Annealing algorithm** as a general-purpose optimization method. Simulated Annealing is inspired by the physical annealing process, where a material is heated and then slowly cooled to reach a stable low-energy configuration.

In optimization, the algorithm explores the solution space by accepting better solutions and, with a certain probability, also accepting worse solutions. This allows the algorithm to escape local optima and continue searching for better global solutions.

This project applies Simulated Annealing to both:

- **Maximization problems**, such as the Knapsack Problem and benchmark-function optimization.
- **Minimization problems**, such as the Traveling Salesman Problem and the Vehicle Routing Problem with Time Windows.

---

## Repository Structure

```text
SimulatedAnnealingProject/
│
├── EvaluatingBechmarkFunctions/
│   ├── SAmain.cpp
│   └── SAmain.exe
│
├── KanaspackProblem/
│   ├── dataset/
│   ├── kp.cpp
│   ├── kp.exe
│   ├── objective_history.csv
│   ├── objective_weight_history.csv
│   └── value_weight_history.csv
│
├── TravelingSalesmanProblem/
│   ├── dataset/
│   │   └── d2/
│   ├── TSP.cpp
│   ├── TSP.exe
│   ├── TSPTesting.cpp
│   └── TSPTesting.exe
│
├── VRPTW/
│   ├── dataset/
│   ├── VRPTW.cpp
│   └── VRPTW.exe
│
└── README.md
