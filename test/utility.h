#include <iostream>
#include <string>

template <typename T>
inline void PRINT_ELEMENTS(const T &coll,
  const std::string &optstr="")
{
  std::cout << optstr;
  typedef typename T::const_iterator TIT;
  for(TIT it = coll.begin(); it != coll.end(); ++it){
    std::cout << *it << " ";
  }
  std::cout << std::endl;
}
