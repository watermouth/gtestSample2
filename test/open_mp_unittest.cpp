#include "gtest/gtest.h"
#include <omp.h>
#include <vector>
#include <numeric>
#include <iostream>
#include "utility.h"

using namespace std;

TEST(OpenMPTest, print){
  omp_set_num_threads(2);
  #pragma omp parallel
  {
    cout << "Hello!" << endl;
    cout << "thread_num : " << omp_get_num_threads() << ".." << endl;
  } 
}

TEST(OpenMPTest, forloop){
  size_t size = 1000000;
  vector<double> a(size);
  omp_set_num_threads(1);
  cout << "max threads : " << omp_get_max_threads() << endl;
  #pragma omp parallel for 
  for (size_t i=0; i<size; ++i) {
    a[i] = (double)i;
  }
  cout << accumulate(a.begin(),a.end(),0) << endl; 
}

TEST(OpenMPTest, forloop2){
  size_t size = 1000000;
  vector<double> a(size);
  omp_set_num_threads(2);
  cout << "max threads : " << omp_get_max_threads() << endl;
  #pragma omp parallel for 
  for (size_t i=0; i<size; ++i) {
    a[i] = (double)i;
  }
  cout << accumulate(a.begin(),a.end(),0) << endl; 
}

