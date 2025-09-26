<div align="center">
  <h1>Dataframe</h1>
  <h3>C++17 implementation of <code>pandas.DataFrame</code></h3>
</div>

<p align="center">
  <img alt="C++17" src="https://img.shields.io/badge/C++-17-blue.svg?style=flat-square"/>
  <img alt="Windows" src="https://img.shields.io/badge/Platform-Windows/Linux/Mac-green?style=flat-square"/>
  <img alt="License" src="https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square"/>
</p>

---

## Overview
A **lightweight single-header** C++17 dataframe implementation inspired by **pandas.DataFrame**.

Think of **Dataframe** as a **spreadsheet** in C++:
- **Columns** are named **labels**
- **Rows** are **indexed records**

Each column can hold **different types** (`int`, `float`, `double`, `string`), so you can **load CSVs**, manipulate data, and perform analytics with just a few lines of code — bringing pandas-like power to C++.

---

## Requirements
- **C++17** or newer  
  Uses features like `std::variant`, `std::optional`, and `std::from_chars`.
- **Windows console** *(only for colored logging via `c_logger`)*  

> **Note for Linux/Mac:**  
> The dataframe core works on any platform.  
> `c_logger` uses **Windows-specific APIs** for colored output.  
> To run on Linux/Mac:
> - Replace `c_logger` with a simple `printf`/`std::cout` logger, or  
> - Use ANSI escape codes for colors, or  
> - Stub out logging completely.

---

## Installation
1. Add the single header to your project:
   ```
   dataframe.h
   ```
2. Include it in your code:
   ```cpp
   #include "dataframe.h"
   ```
3. Compile with a C++17 compatible compiler.

---

## Bash Build Instructions

### MSVC (Windows Developer Command Prompt)
```bash
cl /std:c++17 /O2 main.cpp
```

### MinGW (Windows)
```bash
g++ -std=gnu++17 -O2 main.cpp -o demo.exe
```

### Linux / Mac (portable core; replace c_logger)
```bash
g++ -std=c++17 -O2 main.cpp -o demo
```

---

## Features
- **Mixed types per column**: `using value_t = std::variant<double, std::string>;`
- **CSV I/O**: `from_csv(path, header)` and `to_csv(path, header)`
- **Column operations**:
  - Add: `add_column(name, values)`
  - Rename: `rename_column(old_name, new_name)`
  - Remove: `remove_column(name)`
  - Shape: `shape() -> {rows, columns}`
- **Row operations**:
  - `drop(index)`
  - `dropf()` – drop first row
  - `dropb()` – drop last row
- **Cleaning**:
  - `dropna()` – remove rows with NaN
  - `dropinf()` – remove rows with Inf
  - `dropemp()` – remove rows with empty string cells
- **Statistics**:
  - `sum`, `prod`, `mean`, `var`, `std`, `min`, `max`
- **Relationships**:
  - `cov` – covariance
  - `corr` – correlation
- **Higher moments**:
  - `skew` – skewness
  - `kurt` – excess kurtosis
- **Transforms**:
  - `diff` – first difference
  - `pct_change` – percentage change
  - `log_change` – log returns
  - `cumsum` – cumulative sum
  - `cumprod` – cumulative product
- **Selection**:
  - `at("col") -> std::vector<value_t>&` (mutable reference to column)
  - `at<T>("col") -> std::vector<T>` typed extraction with automatic parsing
  - `at<T>({col1, col2, ...}) -> std::vector<std::vector<T>>` multiple columns
- **Display**:
  - `print(n)` – pretty console output
  - `head(n)` – first *n* rows
  - `tail(n)` – last *n* rows

---

## Example Usage

```cpp
#include "dataframe.h"
#include <iostream>

int main() {
    c_dataframe df;

    // Add some columns
    df.add_column("Name", std::vector<std::string>{"Alice", "Bob", "Charlie"});
    df.add_column("Age", std::vector<int>{25, 30, 28});
    df.add_column("Salary", std::vector<double>{50000, 60000, 55000});

    // Print the dataframe
    df.print();

    // Access statistics
    double avg_salary = df.mean("Salary");
    double salary_std = df.std("Salary");
    std::cout << "Average salary: " << avg_salary << std::endl;
    std::cout << "Salary std dev: " << salary_std << std::endl;

    // Save to CSV
    df.to_csv("employees.csv", true);

    // Load from CSV
    c_dataframe df2("employees.csv", true);
    df2.print();

    return 0;
}
```

**Example Console Output:**
```
        Name                       Age                        Salary
-----------------------------------------------------------------------
0       Alice                      25                         50000
1       Bob                        30                         60000
2       Charlie                    28                         55000
```
