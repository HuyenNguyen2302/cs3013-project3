# cs3013-project3
Solving synchronization problems using 2 methods:
+ Semaphores
+ Mutexes & condition variables

# Installation & Usage
For semaphores: Navigate to the semaphore folder, run make, then run ./order_semaphore [number_of_orders]
For mutexes and condition variables: Navigate to the too_many_cooks, run make, then run ./mutex_cv [numorders] [shiptime] [cooktime]

# Installation
## Semaphore
## Mutexes & condition variables
  If you haven't compiled the program, compile with following commands:

    cd mutex
    make

# Usage
## Semaphore
### Mutexes & condition variables

  To run the program, using following commands:

    ./mutex_cv numorders shiptime cooktime

  **numorders**: number of orders

  **shiptime** (in nanoseconds): time interval for order to arrive

  **cooktime** (in nanoseconds): time interval for stages

  To abort the program, type in **q** and hit **Enter**.

# Test inputs
### Mutexes & condition variables
  - Order arrives much faster than processing speed

        ./mutex_cv 200 1000 10000

  - Order arrives much slower than the processing speed

        ./mutex_cv 200 100000 100

  - Order arrives roughly the same as the processing speed

        ./mutex_cv 200 10000 1000

# Authors
Huyen Nguyen & Yang Liu
# License
GPL
