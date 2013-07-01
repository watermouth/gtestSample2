#include "add.h"

using namespace std;

int add (int x, int y){
  return x + y;
}

template<typename T>
std::vector<T> add (const std::vector<T> &x, const std::vector<T> &y){
  typedef typename std::vector<T>::iterator VecIter;
  std::vector<T> result;
  if (x.size() != y.size()){
    return result;
  }
  for(VecIter it = x.begin(), it_y = y.begin(); it != x.end(); ++it, ++it_y){
    result.push_back(!it + !it_y);
  }
  return result;
}


