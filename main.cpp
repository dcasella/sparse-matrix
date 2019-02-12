#include <string>
#include "sparsematrix.h"

struct pair {
  std::string a;
  std::string b;

  pair() : a("-"), b("-") {}

  pair(std::string a, std::string b) : a(a), b(b) {}
};

std::ostream& operator<<(std::ostream& os, const pair& t) {
  return os << "(" << t.a << ", " << t.b << ")";
}

template <typename T>
struct even {
  bool operator()(const typename SparseMatrix<T>::element& e) const {
    return e.value % 2 == 0;
  }
};

template <typename T>
struct positive {
  bool operator()(const typename SparseMatrix<T>::element& e) const {
    return e.value >= 0;
  }
};

struct length_g3 {
  bool operator()(const SparseMatrix<std::string>::element& e) const {
    return e.value.length() > 3;
  }
};

int main(int argc, const char* argv[]) {
  // element constructor
  SparseMatrix<float>::element e1(0, 0, 42);

  // element operator<<
  std::cout << "element e1 (" << e1.i << ", " << e1.j << "): " << e1;
  std::cout << std::endl << std::endl;

  // element copy constructor
  SparseMatrix<float>::element e2(e1);
  std::cout << "element e2 (" << e2.i << ", " << e2.j << ") copy1: " << e2;
  std::cout << std::endl << std::endl;

  // element copy constructor
  SparseMatrix<float>::element e3 = e2;
  std::cout << "element e3 (" << e3.i << ", " << e3.j << ") copy2: " << e3;
  std::cout << std::endl << std::endl;

  // SparseMatrix constructor
  SparseMatrix<int> m0(4, 2, 0);

  // SparseMatrix operator<<
  std::cout << "m0 (4 x 2):" << std::endl << m0;
  std::cout << std::endl << std::endl;

  // SparseMatrix test
  SparseMatrix<int> m1(0);
  m1.add(0, 2, 25);
  m1.add(0, 3, 14);
  m1.add(0, 4, 25);
  m1.add(1, 0, 22);
  m1.add(1, 1, 23);
  m1.add(1, 2, 15);
  m1.add(2, 4, 11);
  m1.add(3, 1, 5);
  m1.add(3, 2, 23);
  m1.add(4, 2, 4);
  std::cout << "m1 (5 x 5):" << std::endl << m1;
  std::cout << std::endl << std::endl;

  // SparseMatrix copy constructor
  SparseMatrix<int> m2(m1);
  std::cout << "m2 (5 x 5) copy1:" << std::endl << m2;
  std::cout << std::endl << std::endl;

  // SparseMatrix generic copy constructor
  SparseMatrix<float> m3(m1);
  std::cout << "m3 (5 x 5) copy2:" << std::endl << m3;
  std::cout << std::endl << std::endl;

  // SparseMatrix operator=
  m2 = m1;
  std::cout << "m2 (5 x 5) copy3:" << std::endl << m2;
  std::cout << std::endl << std::endl;

  // SparseMatrix get rows, cols, size, D
  std::cout << "m0 rows: " << m0.rows();
  std::cout << std::endl << std::endl;
  std::cout << "m1 cols: " << m1.cols();
  std::cout << std::endl << std::endl;
  std::cout << "m2 size: " << m2.size();
  std::cout << std::endl << std::endl;
  std::cout << "m3 D:    " << m3.D();
  std::cout << std::endl << std::endl;

  // SparseMatrix add from element
  SparseMatrix<float>::element e4(0, 0, 0.42);
  std::cout << "element e4 (0, 0): " << e4;
  std::cout << std::endl << std::endl;
  m3.add(e4);
  std::cout << "m3 (5 x 5) add e4:" << std::endl << m3;
  std::cout << std::endl << std::endl;

  // SparseMatrix add element (overwrite)
  // (0, 0) | 42 over 0.42
  m3.add(e1);
  std::cout << "m3 (5 x 5) add e1:" << std::endl << m3;
  std::cout << std::endl << std::endl;

  // SparseMatrix operator()
  std::cout << "m2(4, 2): " << m2(4, 2);
  std::cout << std::endl << std::endl;

  // SparseMatrix operator*
  SparseMatrix<int> m4(0);
  m4.add(0, 1, 4);
  m4.add(0, 2, -2);
  m4.add(1, 0, -4);
  m4.add(1, 1, -3);
  SparseMatrix<int> m5(3, 1, 0);
  m5.add(0, 1, 1);
  m5.add(1, 0, 1);
  m5.add(1, 1, -1);
  m5.add(2, 0, 2);
  m5.add(2, 1, 3);
  std::cout << "m4 (2 x 3):" << std::endl << m4;
  std::cout << std::endl << std::endl;
  std::cout << "m5 (3 x 2):" << std::endl << m5;
  std::cout << std::endl << std::endl;
  std::cout << "m4 * m5:" << std::endl << m4 * m5;
  std::cout << std::endl << std::endl;

  // SparseMatrix clear
  m2.clear();
  std::cout << "m2 (5 x 5) clear:" << std::endl << m2;
  std::cout << std::endl << std::endl;

  // prepare matrix m6 for evaluate(m6, length_g3<std::string>())
  SparseMatrix<std::string> m6("nil");
  m6.add(1, 1, "yes");
  m6.add(1, 2, "foobar");
  m6.add(2, 1, "hello");
  m6.add(2, 2, "rip");
  std::cout << "m6 (3 x 3):" << std::endl << m6;
  std::cout << std::endl << std::endl;

  // SparseMatrix function evaluate
  std::cout << "m3 even values:            " << evaluate(m1, even<int>());
  std::cout << std::endl << std::endl;
  std::cout << "m3 positive values:        " << evaluate(m3, positive<float>());
  std::cout << std::endl << std::endl;
  std::cout << "m6 values with length > 3: " << evaluate(m6, length_g3());
  std::cout << std::endl << std::endl;

  // Test custom type
  pair pD;
  pair p0("0", "0");
  pair p1("1", "1");
  pair p2("2", "2");
  pair p3("3", "3");
  SparseMatrix<pair> m7(pD);
  m7.add(0, 0, p0);
  m7.add(1, 1, p1);
  m7.add(2, 2, p2);
  m7.add(3, 3, p3);
  std::cout << "m7 (4 x 4):" << std::endl << m7;
  std::cout << std::endl << std::endl;

  // Test out_of_range exception from get function
  try {
    std::cout << "m7(4, 2): " << m5(4, 2);
    std::cout << std::endl << std::endl;
  } catch (const std::out_of_range& e) {
    std::cerr << "Out of range exception: " << e.what();
    std::cout << std::endl << std::endl;
  }

  return 0;
}
