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

SparseMatrix operator*(const SparseMatrix<Q>&) const;

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

File: `main.cpp`.
