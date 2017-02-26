#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include <iostream>  // std::ostream
#include <cassert>   // assert
#include <algorithm> // std::swap
#include <new>       // std::bad_alloc
#include <iterator>  // std::forward_iterator_tag
#include <cstddef>   // std::ptrdiff_t
#include <stdexcept> // std::out_of_range


/**
 * Only the elements explicitly inserted (by the user) are physically stored.
 * @brief Sparse matrix templated class
 */
template <typename T>
class SparseMatrix {
  
public:
  
  typedef unsigned int size_type; ///< Type used for dimensions and positions
  
  /**
   * Contains data about the element's position in the matrix and value.
   * @brief Matrix element struct
   */
  struct element {
    
    const size_type i; ///< Index of element relative to matrix rows
    const size_type j; ///< Index of element relative to matrix columns
    T value;           ///< Value of element
    
    /**
     * Create an element with i, j and value parameters (unsigned).
     * @brief Matrix element constructor
     * @param i     Index of element relative to matrix rows (unsigned)
     * @param j     Index of element relative to matrix columns (unsigned)
     * @param value Value of element
     */
    element(size_type i, size_type j, const T& value)
      : i(i), j(j), value(value) { }
    
    /**
     * Create an element with i, j and value parameters (signed).
     * @brief Matrix element constructor
     * @param i     Index of element relative to matrix rows (signed)
     * @param j     Index of element relative to matrix columns (signed)
     * @param value Value of element
     */
    element(int i, int j, const T& value)
      : i(static_cast<size_type>(i)), j(static_cast<size_type>(j)), value(value) {
      
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
    
    element key; ///< Matrix element
    node *next;  ///< Next node in the list
    
    /**
     * Create an empty node.
     * @brief List node default constructor
     */
    node()
      : next(0) { }
    
    /**
     * Create a node with an element as key, and a pointer to the next node.
     * @brief List node secondary constructor
     * @param key  List node key
     * @param next Pointer to next node (default: 0) in list
     */
    node(const element& key, node *next = 0)
      : key(key), next(next) { }
  };
  
  size_type _rows; ///< Matrix rows
  size_type _cols; ///< Matrix cols
  T _D;            ///< Matrix default element's value
  
  size_t _size;    ///< Matrix size, number of stored elements
  
  node *_head;     ///< Pointer to head (node) of list
  
  /**
   * Prevents the class from being instantiated empty (no _D).
   * @brief Default constructor
   */
  SparseMatrix() { }
  
  /**
   * Delete each node in list making use of recursion.
   * @brief Helper for function clear
   * @param n List node to delete
   */
  void clear_helper(node *n) {
    
    node *tmp;
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
    
    size_type rows_bck = _rows;
    size_type cols_bck = _cols;
    T D_bck = _D;
    
    try {
      _rows = other.rows();
      _cols = other.cols();
      _D = static_cast<T>(other.D());
      
      typename SparseMatrix<Q>::const_iterator it, it_e;
      
      for (it = other.begin() ; it != other.end() ; ++it) {
        SparseMatrix<T>::element e(it->i, it->j, static_cast<T>(it->value));
        add(e);
      }
    }
    catch (...) {
      _rows = rows_bck;
      _cols = cols_bck;
      _D = D_bck;
      
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
  const T get(size_type i, size_type j) const {
    
    if (i >= _rows || j >= _cols)
      throw std::out_of_range("i or j out of bounds");
    
    if (!_head)
      return _D;
    
    node *n = _head, *prev = _head;
    
    while (n && (n->key.i < i || (n->key.i == i && n->key.j <= j))) {
      prev = n;
      n = n->next;
    }
    
    if (prev && i == prev->key.i && j == prev->key.j) {
      return prev->key.value;
    }
    
    return _D;
  }
  
public:
  
  /**
   * Create a sparse matrix with D parameter.
   * @brief Secondary constructor
   * @param D    Matrix default element's value
   */
  explicit SparseMatrix(const T& D)
    : _rows(0), _cols(0), _D(D), _size(0), _head(0) {
    
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
  SparseMatrix(size_type rows, size_type cols, const T& D)
    : _rows(rows), _cols(cols), _D(D), _size(0), _head(0) {
    
    #ifndef NDEBUG
    std::cout << "SparseMatrix::SparseMatrix(size_type, size_type, const T&)" << std::endl;
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
    : _rows(static_cast<size_type>(rows)), _cols(static_cast<size_type>(cols)),
      _D(D), _size(0), _head(0) {
    
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
    : _rows(0), _cols(0), _D(0), _size(0), _head(0) {
    
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
    : _rows(0), _cols(0), _D(0), _size(0), _head(0) {
    
    #ifndef NDEBUG
    std::cout << "SparseMatrix::SparseMatrix(const SparseMatrix<Q>&)" << std::endl;
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
      std::swap(_rows, tmp._rows);
      std::swap(_cols, tmp._cols);
      std::swap(_D, tmp._D);
      std::swap(_size, tmp._size);
      std::swap(_head, tmp._head);
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
    _rows = 0;
    _cols = 0;
  }
  
  /**
   * Get matrix number of rows.
   * @brief Rows getter
   * @return Matrix rows
   */
  size_type rows() const { return _rows; }
  
  /**
   * Get matrix number of columns.
   * @brief Columns getter
   * @return Matrix columns
   */
  size_type cols() const { return _cols; }
  
  /**
   * Get the number of elements.
   * @brief Size getter
   * @return Matrix size
   */
  size_t size() const { return _size; }
  
  /**
   * Get the default element.
   * @brief Default element getter
   * @return Matrix default element's value
   */
  const T D() const { return _D; }
  
  /**
   * Set the default element.
   * @brief Default element setter
   * @return Matrix default element
   */
  T& D() { return _D; }
  
  /**
   * Insert element into matrix (overwrite if necessary).
   * @brief Matrix add element
   * @param elem Matrix element to add
   */
  void add(const element& elem) {
    
    node *current = new node(elem);
    
    size_type rows = elem.i + 1;
    size_type cols = elem.j + 1;
    
    if (rows > _rows)
      _rows = rows;
    
    if (cols > _cols)
      _cols = cols;
    
    // create head = element
    if (!_head) {
      _head = current;
      ++_size;
      
      return;
    }
    
    node *n = _head, *prev = _head;
    
    // search element or free position
    while (n && (n->key.i < elem.i
      || (n->key.i == elem.i && n->key.j < elem.j))) {
      
      prev = n;
      n = n->next;
    }
    
    // replace element
    if (n && n->key.i == elem.i && n->key.j == elem.j) {
      current->next = n->next;
      
      if (n == _head) {
        _head = current;
      }
      else {
        prev->next = current;
      }
      
      delete n;
      n = 0;
      
      return;
    }
    
    ++_size;
    
    // add element in front of head
    if (n == _head) {
      current->next = _head;
      _head = current;
      
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
  const T operator()(size_type i, size_type j) const {
    
    return get(i, j);
  }
  
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
    
    return get(static_cast<size_type>(i), static_cast<size_type>(j));
  }
  
  /**
   * Clear the Matrix.
   * @brief Matrix clear
   */
  void clear() {
    
    clear_helper(_head);
    _size = 0;
    _head = 0;
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
    typedef element                   value_type;
    typedef ptrdiff_t                 difference_type;
    typedef element*                  pointer;
    typedef element&                  reference;
    
    iterator()
      : n(0) { }
    
    iterator(const iterator &other)
      : n(other.n) { }
    
    iterator& operator=(const iterator &other) {
      
      n = other.n;
      
      return *this;
    }
    
    ~iterator() { }
    
    reference operator*() const {
      
      return n->key;
    }
    
    pointer operator->() const {
      
      return &(n->key);
    }
    
    iterator operator++(int) {
      
      iterator tmp(*this);
      n = n->next;
      
      return tmp;
    }
    
    iterator& operator++() {
      
      n = n->next;
      
      return *this;
    }
    
    bool operator==(const iterator &other) const {
      
      return n == other.n;
    }
    
    bool operator!=(const iterator &other) const {
      
      return n != other.n;
    }
    
    friend class const_iterator;
    
    bool operator==(const const_iterator &other) const {
      
      return n == other.n;
    }
    
    bool operator!=(const const_iterator &other) const {
      
      return n != other.n;
    }
    
  private:
    
    node *n;
    
    friend class SparseMatrix;
    
    iterator(node *p)
      : n(p) { }
    
  };
  
  /**
   * Return begin iterator.
   * @brief Iterator begin
   * @return Iterator pointing to matrix's first element
   */
  iterator begin() {
    
    return iterator(_head);
  }
  
  /**
   * Return end iterator.
   * @brief Iterator end
   * @return Iterator pointing to null
   */
  iterator end() {
    
    return iterator(0);
  }
  
  /**
   * Iterates through matrix's stored elements.
   * @brief Const iterator class
   */
  class const_iterator {
    
  public:
    
    typedef std::forward_iterator_tag iterator_category;
    typedef element                   value_type;
    typedef ptrdiff_t                 difference_type;
    typedef const element*            pointer;
    typedef const element&            reference;
    
    const_iterator()
      : n(0) { }
    
    const_iterator(const const_iterator &other)
      : n(other.n) { }
    
    const_iterator& operator=(const const_iterator &other) {
      
      n = other.n;
      
      return *this;
    }
    
    ~const_iterator() { }
    
    reference operator*() const {
      
      return n->key;
    }
    
    pointer operator->() const {
      
      return &(n->key);
    }
    
    const_iterator operator++(int) {
      
      const_iterator tmp(*this);
      n = n->next;
      
      return tmp;
    }
    
    const_iterator& operator++() {
      
      n = n->next;
      
      return *this;
    }
    
    bool operator==(const const_iterator &other) const {
      
      return n == other.n;
    }
    
    bool operator!=(const const_iterator &other) const {
      
      return n != other.n;
    }
    
    friend class iterator;
    
    bool operator==(const iterator &other) const {
      
      return n == other.n;
    }
    
    bool operator!=(const iterator &other) const {
      
      return n != other.n;
    }
    
  private:
    
    node *n;
    
    friend class SparseMatrix;
    
    const_iterator(node *p)
      : n(p) { }
  };
  
  /**
   * Return begin const iterator.
   * @brief Const iterator begin
   * @return Const iterator pointing to matrix's first element
   */
  const_iterator begin() const {
    
    return const_iterator(_head);
  }
  
  /**
   * Return end const iterator.
   * @brief Const iterator end
   * @return Const iterator pointing to null
   */
  const_iterator end() const {
    
    return const_iterator(0);
  }
  
  /**
   * Overloading of operator<<.
   * @brief Matrix ostream operator
   * @param  os Output stream
   * @param  m  Matrix
   * @return Updated output stream
   */
  friend std::ostream& operator<<(std::ostream& os, const SparseMatrix<T>& m) {
    
    os << "[";
    
    for (size_type i = 0; i < m._rows; ++i) {
      if (i > 0)
        os << ",\n ";
      
      os << "[";
      
      for (size_type j = 0; j < m._cols; ++j) {
        if (j > 0)
          os << ",\t";
        
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
  
  for (typename SparseMatrix<T>::size_type i = 0; i < m.rows(); ++i) {
    for (typename SparseMatrix<T>::size_type j = 0; j < m.cols(); ++j) {
      typename SparseMatrix<T>::element e1(i, j, m(i, j));
      
      if (p(e1)) ++count;
      
      else if (e1.value == m.D()) {
        typename SparseMatrix<T>::element e2(i, j, m.D());
        
        if (p(e2)) ++count;
      }
    }
  }
  
  return count;
}

#endif
