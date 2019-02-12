#ifndef SPARSE_MATRIX_H_
#define SPARSE_MATRIX_H_

#include <algorithm>  // std::swap
#include <cassert>    // assert
#include <cstddef>    // std::ptrdiff_t
#include <iostream>   // std::ostream
#include <iterator>   // std::forward_iterator_tag
#include <new>        // std::bad_alloc
#include <stdexcept>  // std::out_of_range

/**
 * Only the elements explicitly inserted (by the user) are physically stored.
 * @brief Sparse matrix templated class
 */
template <typename T>
class SparseMatrix {
 public:
  /**
   * Contains data about the element's position in the matrix and value.
   * @brief Matrix element struct
   */
  struct element {
    const size_t i;  ///< Index of element relative to matrix rows
    const size_t j;  ///< Index of element relative to matrix columns
    T value;         ///< Value of element

    /**
     * Create an element with i, j and value parameters (unsigned).
     * @brief Matrix element constructor
     * @param i     Index of element relative to matrix rows (unsigned)
     * @param j     Index of element relative to matrix columns (unsigned)
     * @param value Value of element
     */
    element(size_t i, size_t j, const T& value) : i(i), j(j), value(value) {}

    /**
     * Create an element with i, j and value parameters (signed).
     * @brief Matrix element constructor
     * @param i     Index of element relative to matrix rows (signed)
     * @param j     Index of element relative to matrix columns (signed)
     * @param value Value of element
     */
    element(int i, int j, const T& value)
        : i(static_cast<size_t>(i)), j(static_cast<size_t>(j)), value(value) {
      assert(i >= 0);
      assert(j >= 0);
    }

    /**
     * Overloading of operator<<.
     * @brief Matrix element ostream operator
     * @param  os Output stream
     * @param  e  Matrix element
     * @return Updated output stream
     */
    friend std::ostream& operator<<(std::ostream& os, const element& e) {
      return os << e.value;
    }
  };

 private:
  /**
   * Linked-list node implementation: key (element), next.
   * @brief List node struct
   */
  struct node {
    element key;  ///< Matrix element
    node* next;   ///< Next node in the list

    /**
     * Create an empty node.
     * @brief List node default constructor
     */
    node() : next(0) {}

    /**
     * Create a node with an element as key, and a pointer to the next node.
     * @brief List node secondary constructor
     * @param key  List node key
     * @param next Pointer to next node (default: 0) in list
     */
    node(const element& key, node* next = 0) : key(key), next(next) {}
  };

  size_t rows_;  ///< Matrix rows
  size_t cols_;  ///< Matrix cols
  T D_;          ///< Matrix default element's value

  size_t size_;  ///< Matrix size, number of stored elements

  node* head_;  ///< Pointer to head (node) of list

  /**
   * Prevents the class from being instantiated empty (no D_).
   * @brief Default constructor
   */
  SparseMatrix() {}

  /**
   * Delete each node in list making use of recursion.
   * @brief Helper for function clear
   * @param n List node to delete
   */
  void clear_helper(node* n) {
    node* tmp;
    while ((tmp = n)) {
      n = n->next;
      delete tmp;
    }
  }

  /**
   * Copy all nodes and data members from a SparseMatrix of different type Q.
   * @brief Helper for copy constructor and assignment
   * @param other Other SparseMatrix to copy
   */
  template <typename Q>
  void copy(const SparseMatrix<Q>& other) {
    size_t rows_bck = rows_;
    size_t cols_bck = cols_;
    T D_bck = D_;

    try {
      rows_ = other.rows();
      cols_ = other.cols();
      D_ = static_cast<T>(other.D());

      typename SparseMatrix<Q>::const_iterator it;

      for (it = other.begin(); it != other.end(); ++it) {
        element e(it->i, it->j, static_cast<T>(it->value));
        add(e);
      }
    } catch (...) {
      rows_ = rows_bck;
      cols_ = cols_bck;
      D_ = D_bck;

      clear();
      throw;
    }
  }

  /**
   * Return the element at the given coordinates.
   * @brief Matrix get element
   * @param  i Index of element relative to matrix rows, unsigned value
   * @param  j Index of element relative to matrix columns, unsigned value
   * @return Matrix element
   * @throw  out_of_range Indices i or j are equal or greater than rows or cols
   */
  const T get(size_t i, size_t j) const {
    if (i >= rows_ || j >= cols_)
      throw std::out_of_range("i or j out of bounds");

    if (!head_) return D_;

    node *n = head_, *prev = head_;

    while (n && (n->key.i < i || (n->key.i == i && n->key.j <= j))) {
      prev = n;
      n = n->next;
    }

    if (prev && i == prev->key.i && j == prev->key.j) {
      return prev->key.value;
    }

    return D_;
  }

 public:
  /**
   * Create a sparse matrix with D parameter.
   * @brief Secondary constructor
   * @param D Matrix default element's value
   */
  explicit SparseMatrix(const T& D)
      : rows_(0), cols_(0), D_(D), size_(0), head_(0) {
#ifndef NDEBUG
    std::cout << "SparseMatrix::SparseMatrix(const T&)" << std::endl;
#endif
  }

  /**
   * Create a sparse matrix with rows, cols and D parameters.
   * @brief Secondary constructor
   * @param rows Matrix rows, unsigned value
   * @param cols Matrix columns, unsigned value
   * @param D    Matrix default element's value
   */
  SparseMatrix(size_t rows, size_t cols, const T& D)
      : rows_(rows), cols_(cols), D_(D), size_(0), head_(0) {
#ifndef NDEBUG
    std::cout << "SparseMatrix::SparseMatrix(size_t, size_t, const T&)"
              << std::endl;
#endif

    assert(rows > 0);
    assert(cols > 0);
  }

  /**
   * Create a sparse matrix with rows, cols and D parameters.
   * @brief Secondary constructor
   * @param rows Matrix rows, signed value
   * @param cols Matrix columns, signed value
   * @param D    Matrix default element's value
   */
  SparseMatrix(int rows, int cols, const T& D)
      : rows_(static_cast<size_t>(rows)),
        cols_(static_cast<size_t>(cols)),
        D_(D),
        size_(0),
        head_(0) {
#ifndef NDEBUG
    std::cout << "SparseMatrix::SparseMatrix(int, int, const T&)" << std::endl;
#endif

    assert(rows > 0);
    assert(cols > 0);
  }

  /**
   * Create a sparse matrix from another SparseMatrix instance.
   * @brief Copy constructor
   * @param other Other SparseMatrix to copy
   */
  SparseMatrix(const SparseMatrix& other)
      : rows_(0), cols_(0), D_(0), size_(0), head_(0) {
#ifndef NDEBUG
    std::cout << "SparseMatrix::SparseMatrix(const SparseMatrix&)" << std::endl;
#endif

    copy(other);
  }

  /**
   * Create a sparse matrix from a SparseMatrix instance with different
   * element types.
   * @brief Templated copy constructor
   * @param other Other SparseMatrix to copy
   */
  template <typename Q>
  SparseMatrix(const SparseMatrix<Q>& other)
      : rows_(0), cols_(0), D_(0), size_(0), head_(0) {
#ifndef NDEBUG
    std::cout << "SparseMatrix::SparseMatrix(const SparseMatrix<Q>&)"
              << std::endl;
#endif

    copy(other);
  }

  /**
   * Copy the data from another SparseMatrix instance.
   * @brief Assignment operator
   * @param  other Other SparseMatrix to copy
   * @return Copied SparseMatrix
   */
  SparseMatrix& operator=(const SparseMatrix& other) {
#ifndef NDEBUG
    std::cout << "SparseMatrix::operator=(const SparseMatrix&)" << std::endl;
#endif

    if (this != &other) {
      SparseMatrix tmp(other);
      std::swap(rows_, tmp.rows_);
      std::swap(cols_, tmp.cols_);
      std::swap(D_, tmp.D_);
      std::swap(size_, tmp.size_);
      std::swap(head_, tmp.head_);
    }

    return *this;
  }

  /**
   * Clear the matrix.
   * @brief Destructor
   */
  ~SparseMatrix() {
#ifndef NDEBUG
    std::cout << "SparseMatrix::~SparseMatrix()" << std::endl;
#endif

    clear();
    rows_ = 0;
    cols_ = 0;
  }

  /**
   * Get matrix number of rows.
   * @brief Rows getter
   * @return Matrix rows
   */
  size_t rows() const { return rows_; }

  /**
   * Get matrix number of columns.
   * @brief Columns getter
   * @return Matrix columns
   */
  size_t cols() const { return cols_; }

  /**
   * Get the number of elements.
   * @brief Size getter
   * @return Matrix size
   */
  size_t size() const { return size_; }

  /**
   * Get the default element.
   * @brief Default element getter
   * @return Matrix default element's value
   */
  const T D() const { return D_; }

  /**
   * Insert element into matrix (overwrite if necessary).
   * @brief Matrix add element
   * @param elem Matrix element to add
   */
  void add(const element& elem) {
    node* current = new node(elem);

    size_t rows = elem.i + 1;
    size_t cols = elem.j + 1;

    if (rows > rows_) rows_ = rows;

    if (cols > cols_) cols_ = cols;

    // create head = element
    if (!head_) {
      head_ = current;
      ++size_;

      return;
    }

    node *n = head_, *prev = head_;

    // search element or free position
    while (n &&
           (n->key.i < elem.i || (n->key.i == elem.i && n->key.j < elem.j))) {
      prev = n;
      n = n->next;
    }

    // replace element
    if (n && n->key.i == elem.i && n->key.j == elem.j) {
      current->next = n->next;

      if (n == head_) {
        head_ = current;
      } else {
        prev->next = current;
      }

      delete n;
      n = 0;

      return;
    }

    ++size_;

    // add element in front of head
    if (n == head_) {
      current->next = head_;
      head_ = current;

      return;
    }

    // add element in list
    current->next = prev->next;
    prev->next = current;
  }

  /**
   * Insert element into matrix (overwrite if necessary).
   * @brief Matrix add element
   * @param i     Index of element relative to matrix rows, unsigned value
   * @param j     Index of element relative to matrix columns, unsigned value
   * @param value Value of element
   */
  template <typename pos_type>
  void add(pos_type i, pos_type j, const T& value) {
    element e(i, j, value);
    add(e);
  }

  /**
   * Return the element at the given coordinates.
   * @brief Matrix get element
   * @param  i Index of element relative to matrix rows, unsigned value
   * @param  j Index of element relative to matrix columns, unsigned value
   * @return Matrix element
   */
  const T operator()(size_t i, size_t j) const { return get(i, j); }

  /**
   * Return the element at the given coordinates.
   * @brief Matrix get element
   * @param  i Index of element relative to matrix rows, signed value
   * @param  j Index of element relative to matrix columns, signed value
   * @return Matrix element
   */
  const T operator()(int i, int j) const {
    assert(i >= 0);
    assert(j >= 0);

    return get(static_cast<size_t>(i), static_cast<size_t>(j));
  }

  /**
   * Perform matrix multiplication between *this and other of generic type Q,
   * and return the result
   * @brief Matrix multiplication operator
   * @param  other Other matrix
   * @return Matrix representing the matrix multiplication
   */
  template <typename Q>
  SparseMatrix operator*(const SparseMatrix<Q>& other) const {
#ifndef NDEBUG
    std::cout
        << "SparseMatrix SparseMatrix::operator*(const SparseMatrix<Q>&) const"
        << std::endl;
#endif

    if (this->cols() != other.rows())
      throw std::out_of_range("m1.cols() != m2.rows()");

    SparseMatrix result(this->rows(), other.cols(), this->D());

    const_iterator m1_it;
    typename SparseMatrix<Q>::const_iterator m2_it;

    for (m1_it = this->begin(); m1_it != this->end(); ++m1_it) {
      for (m2_it = other.begin(); m2_it != other.end(); ++m2_it) {
        // only process m1[i, N] * m2[N, j]
        if (m1_it->j != m2_it->i) continue;

        // sum (m1[i, N] * m2[N, j]) to result[i, j]
        result.add(m1_it->i, m2_it->j,
                   result(m1_it->i, m2_it->j) + m1_it->value * m2_it->value);
      }
    }

    return result;
  }

  /**
   * Clear the Matrix.
   * @brief Matrix clear
   */
  void clear() {
    clear_helper(head_);
    size_ = 0;
    head_ = 0;
  }

  // Iterators

  class const_iterator;

  /**
   * Iterates through matrix's stored elements.
   * @brief Iterator class
   */
  class iterator {
   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef element value_type;
    typedef ptrdiff_t difference_type;
    typedef element* pointer;
    typedef element& reference;

    iterator() : n(0) {}

    iterator(const iterator& other) : n(other.n) {}

    iterator& operator=(const iterator& other) {
      n = other.n;

      return *this;
    }

    ~iterator() {}

    reference operator*() const { return n->key; }

    pointer operator->() const { return &(n->key); }

    iterator operator++(int) {
      iterator tmp(*this);
      n = n->next;

      return tmp;
    }

    iterator& operator++() {
      n = n->next;

      return *this;
    }

    bool operator==(const iterator& other) const { return n == other.n; }

    bool operator!=(const iterator& other) const { return n != other.n; }

    friend class const_iterator;

    bool operator==(const const_iterator& other) const { return n == other.n; }

    bool operator!=(const const_iterator& other) const { return n != other.n; }

   private:
    node* n;

    friend class SparseMatrix;

    iterator(node* p) : n(p) {}
  };

  /**
   * Return begin iterator.
   * @brief Iterator begin
   * @return Iterator pointing to matrix's first element
   */
  iterator begin() { return iterator(head_); }

  /**
   * Return end iterator.
   * @brief Iterator end
   * @return Iterator pointing to null
   */
  iterator end() { return iterator(0); }

  /**
   * Iterates through matrix's stored elements.
   * @brief Const iterator class
   */
  class const_iterator {
   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef element value_type;
    typedef ptrdiff_t difference_type;
    typedef const element* pointer;
    typedef const element& reference;

    const_iterator() : n(0) {}

    const_iterator(const const_iterator& other) : n(other.n) {}

    const_iterator& operator=(const const_iterator& other) {
      n = other.n;

      return *this;
    }

    ~const_iterator() {}

    reference operator*() const { return n->key; }

    pointer operator->() const { return &(n->key); }

    const_iterator operator++(int) {
      const_iterator tmp(*this);
      n = n->next;

      return tmp;
    }

    const_iterator& operator++() {
      n = n->next;

      return *this;
    }

    bool operator==(const const_iterator& other) const { return n == other.n; }

    bool operator!=(const const_iterator& other) const { return n != other.n; }

    friend class iterator;

    bool operator==(const iterator& other) const { return n == other.n; }

    bool operator!=(const iterator& other) const { return n != other.n; }

   private:
    node* n;

    friend class SparseMatrix;

    const_iterator(node* p) : n(p) {}
  };

  /**
   * Return begin const iterator.
   * @brief Const iterator begin
   * @return Const iterator pointing to matrix's first element
   */
  const_iterator begin() const { return const_iterator(head_); }

  /**
   * Return end const iterator.
   * @brief Const iterator end
   * @return Const iterator pointing to null
   */
  const_iterator end() const { return const_iterator(0); }

  /**
   * Overloading of operator<<.
   * @brief Matrix ostream operator
   * @param  os Output stream
   * @param  m  Matrix
   * @return Updated output stream
   */
  friend std::ostream& operator<<(std::ostream& os, const SparseMatrix<T>& m) {
    os << "[";

    for (size_t i = 0; i < m.rows_; ++i) {
      if (i > 0) os << ",\n ";

      os << "[";

      for (size_t j = 0; j < m.cols_; ++j) {
        if (j > 0) os << ",\t";

        os << m(i, j);
      }

      os << "]";
    }

    os << "]";

    return os;
  }
};

/**
 * Iterates through the whole matrix and counts the elements that, when tested,
 * returned true to the predicate.
 * @brief Matrix templated count element
 * @param  m Matrix
 * @param  p Predicate to test matrix elements with
 * @return Number of elements that satisfied the predicate
 */
template <typename T, typename P>
int evaluate(const SparseMatrix<T>& m, P p) {
  int count = 0;

  for (size_t i = 0; i < m.rows(); ++i) {
    for (size_t j = 0; j < m.cols(); ++j) {
      typename SparseMatrix<T>::element e1(i, j, m(i, j));

      if (p(e1))
        ++count;

      else if (e1.value == m.D()) {
        typename SparseMatrix<T>::element e2(i, j, m.D());

        if (p(e2)) ++count;
      }
    }
  }

  return count;
}

#endif
