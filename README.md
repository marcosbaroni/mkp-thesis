# My PhD Thesis Implementation

Author: Marcos Daniel Baroni

Date: 2018-12-10

Several combinatorial problems involve the simultaneous optimization of multiple criteria, which are generally conflicting with each other.
These problems are called multiobjective and
do not have a single solution, but a set of solutions of interest.
This work proposes the acceleration of the resolution process of the multiobjective knapsack problem,
through the use of a _kdtree_ as a multidimensional index structure to assist the manipulation of solutions.

### The thesis

[thesis-mbaroni-final.pdf](doc/tese/thesis-mbaroni-final.pdf?raw=true) (pt-br)

#### Abstract

Several real problems involve the simultaneous optimization of multiple criteria, which are generally conflicting with each other.
These problems are called multiobjective and do not have a single solution, but a set of solutions of interest, called efficient solutions or non-dominated solutions.
One of the great challenges to be faced in solving this type of problem is the size of the solution set, which tends to grow rapidly given the size of the instance, degrading algorithms performance. Among the most studied multiobjective problems is the multiobjective knapsack problem, by which several real problems can be modeled.
This work proposes the acceleration of the resolution process of the multiobjective knapsack problem, through the use of a k-d tree as a multidimensional index structure to assist the manipulation of solutions.
The performance of the approach is analyzed through computational experiments, performed in the exact context using a state-of-the-art algorithm.
Tests are also performed in the heuristic context, using the adaptation of a metaheuristic for the problem in question, being also a contribution of the present work.
According to the results, the proposal was effective for the exact context, presenting a speedup up to 2.3 for bi-objective cases and 15.5 for 3-objective cases, but not effective in the heuristic context, presenting little impact on computational time.
In all cases, however,
there was a considerable reduction in the number of solutions evaluations.

**Keywords**: Multiobjective Knapsack Problem, Multidimensional Indexing, Metaheuristic,
Exact Algorithm.

#### Paper about the approach (en): [doc/paper.pdf](doc/paper.pdf?raw=true)

### Description of folders:

- **bin**: binaries and scripts
- **doc**: documentation and publications
- **exp**: experiments records
- **insts**: problem instances
- **mip**: mathematical integer programming descriptions
- **misc**: miscellaneous related files
- **script**: more scripts
- **src**: algorithms source code
  - **metahrs**: metaheuristics implementation
  - **models**: models (structured) of problems
  - **soplex**: soplex example
  - **tools**: tools implementation (main: [mokp.c](src/tools/mokp.c))
  - **utils**: useful data structures implementation

### Results summary

#### 2D Instances

![Number of comparison for 2d instances](doc/tese/src/tab/cmp-2d_2.png)

#### 2D Instances

![Number of comparison for 3d instances](doc/tese/src/tab/cmp-3d_2.png)
