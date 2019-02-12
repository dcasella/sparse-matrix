# Sparse Matrix

> Project's specifications are taken from the 2017 February C++ exam of the course "Programmazione e Amministrazione di Sistema"

A 2D sparse matrix is a matrix in which only the elements explicitly inserted  are physically stored.  
In this implementation we use a ordered linked-list, where the order of the elements depends on their coordinates in the matrix.

Time complexity:  
Empty matrix creation `ϴ(1)`.  
Element insertion `O(size)`, where size is the number of stored elements.  
Matrix inspection `O(size)`.  
Matrix clear `ϴ(size)`.

Space complexity:  
Matrix `ϴ(size)`.

Since `size` is always less than `n * m`, the class is clearly more efficient space-wise than time-wise.

## List of contents
 
- [Interface](#interface)
- [Examples](#examples)

## Interface

### Template

`T`: type of the values stored in the matrix.

### Member classes

#### Element

```cpp
element(size_t, size_t, const T&);

std::ostream& operator<<(std::ostream&, const element&);
```

### Member functions

```cpp
SparseMatrix(const T&);

SparseMatrix(size_t, size_t, const T&);

SparseMatrix(const SparseMatrix&);

SparseMatrix(const SparseMatrix<Q>&);

~SparseMatrix();

SparseMatrix& operator=(const SparseMatrix&);

size_t rows() const;

size_t cols() const;

size_t size() const;

const T D() const;

void add(const element&);

void add(size_t, size_t, const T&);

const T operator()(size_t, size_t);

void clear();
```

### Iterators

```cpp
iterator begin();

iterator end();

const_iterator begin() const;

const_iterator end() const;
```

### Non member functions

```cpp
std::ostream& operator<<(std::ostream&, const SparseMatrix<T>);

int evaluate(const SparseMatrix<T>, P);
```

## Examples

```cpp
main.cpp:

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

// SparseMatrix clear
m2.clear();
std::cout << "m2 (5 x 5) clear:" << std::endl << m2;
std::cout << std::endl << std::endl;

// prepare matrix m4 for evaluate(m4, length_g3<std::string>())
SparseMatrix<std::string> m4("nil");
m4.add(1, 1, "yes");
m4.add(1, 2, "foobar");
m4.add(2, 1, "hello");
m4.add(2, 2, "rip");
std::cout << "m4 (3 x 3):" << std::endl << m4;
std::cout << std::endl << std::endl;

// SparseMatrix function evaluate
std::cout << "m3 even values:            " << evaluate(m1, even<int>());
std::cout << std::endl << std::endl;
std::cout << "m3 positive values:        " << evaluate(m3, positive<float>());
std::cout << std::endl << std::endl;
std::cout << "m4 values with length > 3: " << evaluate(m4, length_g3());
std::cout << std::endl << std::endl;
```
