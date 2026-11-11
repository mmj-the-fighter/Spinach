/// \file matrix.hpp

/*  MIT License
 *
 *  Copyright (c) 2024 gc1905
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

#include <ostream>
#include <complex>
#include <vector>
#include <initializer_list> 
#include <limits>
#include <functional>
#include <algorithm>
#include <utility>

namespace Mtx {

template<typename T> class Matrix;

/** Colelction of various helper functions that allow for generalization of code for complex and real 
 *  datatypes.
 */
namespace Util {
  template<class T> struct is_complex : std::false_type {};
  template<class T> struct is_complex<std::complex<T>> : std::true_type {};

  /** \brief Complex conjugate helper.
   *
   *  Helper function to allow for generalization of code for complex and real types.
   * 
   *  For real numbers, this function returns the input argument unchanged. 
   *  For complex numbers, this function calls std::conj.
   */
  template<typename T, typename std::enable_if<!is_complex<T>::value,int>::type = 0>
  inline T cconj(T x) {
    return x;
  }

  template<typename T, typename std::enable_if<is_complex<T>::value,int>::type = 0>
  inline T cconj(T x) {
    return std::conj(x);
  }

  /** \brief Complex sign helper.
   *
   *  Helper function to allow for generalization of code for complex and real types.
   * 
   *  For real numbers, this function returns sign bit, i.e., 1 when the value is non-negative and -1 otherwise.
   *  For complex numbers, this function calculates \f$ e ^ {i \cdot arg(x)} \f$.
   */
  template<typename T, typename std::enable_if<!is_complex<T>::value,int>::type = 0>
  inline T csign(T x) {
    return (x > static_cast<T>(0)) ? static_cast<T>(1) : static_cast<T>(-1);
  }

  template<typename T, typename std::enable_if<is_complex<T>::value,int>::type = 0>
  inline T csign(T x) {
    auto x_arg = std::arg(x);
    T y(0, x_arg);
    return std::exp(y);
  }

  /** \brief Complex real part helper.
   *
   *  Helper function to allow for generalization of code for complex and real types.
   * 
   *  For real numbers, this function returns the input argument unchanged.
   *  For complex numbers, this function returns the real part.
   */
  template<typename T, typename std::enable_if<!is_complex<T>::value,int>::type = 0>
  inline T creal(std::complex<T> x) {
    return std::real(x);
  }

  template<typename T, typename std::enable_if<!is_complex<T>::value,int>::type = 0>
  inline T creal(T x) {
    return x;
  }
} // namespace Util

/** \brief Singular matrix exception.
 *
 *  This exception is thrown by functions like matrix inversion or factorization that are defined to operate on non-singular
 *  matrices when singular matrix is provided as an input.
 *
 *  @see std::domain_error
 */
class singular_matrix_exception : public std::domain_error {
  public:
    /** \brief Constructor.
     */
    singular_matrix_exception(const std::string& message) : std::domain_error(message) {}
};

/** \brief Result of LU decomposition.
 *
 *  This structure stores the result of LU decomposition, returned by \ref Mtx::lu() function.
 */
template<typename T>
struct LU_result {
  /**\brief Lower triangular matrix
   */
  Matrix<T> L;

  /**\brief Upper triangular matrix
   */
  Matrix<T> U;
};

/** \brief Result of LU decomposition with pivoting.
 *
 *  This structure stores the result of LU decomposition with pivoting, returned by \ref Mtx::lup() function.
 */
template<typename T>
struct LUP_result {
  /**\brief Lower triangular matrix
   */
  Matrix<T> L;

  /**\brief Upper triangular matrix
   */
  Matrix<T> U;

  /**\brief Vector with column permutation indices 
   */
  std::vector<unsigned> P;
};

/** \brief Matrix with pivoting.
 *
 *  This structure encapsulates a \ref Mtx::Matrix object together with related pivot vector. The pivot vector
 *  can represent row or column permutations depending on a specific function that returns this structure.
 */
template<typename T>
struct Matrix_with_pivot {
  /**\brief Matrix
   */
  Matrix<T> A;

  /**\brief Pivot vector with permutation indices
   */
  std::vector<unsigned> P;
};

/** \brief Result of QR decomposition.
 *
 *  This structure stores the result of QR decomposition, returned by, e.g., from \ref Mtx::qr() function.
 *  Note that the dimensions of \a Q and \a R matrices depends on the employed  variant of QR decomposition.
 */
template<typename T>
struct QR_result {
  /**\brief Orthogonal matrix
   */
  Matrix<T> Q;

  /**\brief Upper triangular matrix
   */
  Matrix<T> R;
};

/** \brief Result of Hessenberg decomposition.
 *
 *  This structure stores the result of the Hessenberg decomposition, returned by \ref Mtx::hessenberg() function.
 */
template<typename T>
struct Hessenberg_result {
  /**\brief Matrix with upper Hessenberg form
   */
  Matrix<T> H;

  /**\brief Orthogonal matrix
   */
  Matrix<T> Q;
};

/** \brief Result of LDL decomposition.
 *
 *  This structure stores the result of LDL decomposition, returned by \ref Mtx::ldl() function.
 */
template<typename T>
struct LDL_result {
  /**\brief Lower triangular matrix
   */
  Matrix<T> L;
  
  /**\brief Vector with diagonal elements of diagonal matrix \a D
   */
  std::vector<T> d;
};

/** \brief Result of eigenvalues.
 *
 *  This structure stores the result of matrix eigenvalue calculation, returned by \ref Mtx::eigenvalues() function.
 */
template<typename T>
struct Eigenvalues_result {
  /**\brief Vector of eigenvalues. 
   */
  std::vector<std::complex<T>> eig;
  
  /**\brief Indicates if the eigenvalue algorithm has converged to assumed precision.
   */
  bool converged;

  /**\brief Error of eigenvalue calculation after the last iteration.
   */
  T err;
};


/** \brief Matrix of zeros.
 *
 *  Create a matrix of size \a nrows x \a ncols and fill it with all elements set to 0.
 *  \param nrows number of rows (the first dimension)
 *  \param ncols number of columns (the second dimension)
 *  \return zeros matrix
 */
template<typename T>
inline Matrix<T> zeros(unsigned nrows, unsigned ncols) {
  return Matrix<T>(static_cast<T>(0), nrows, ncols);
}

/** \brief Square matrix of zeros.
 *
 *  Construct a square matrix of size \a n x \a n and fill it with all elements set to 0.
 *  \param n size of the square matrix (the first and the second dimension)
 *  \return zeros matrix
 */
template<typename T>
inline Matrix<T> zeros(unsigned n) {
  return zeros<T>(n,n);
}

/** \brief Matrix of ones.
 *
 *  Construct a matrix of size \a nrows x \a ncols and fill it with all elements set to 1.
 * 
 *  In case of complex data types, matrix is filled with \f$1 + 0i\f$.
 * 
 *  \param nrows number of rows (the first dimension)
 *  \param ncols number of columns (the second dimension)
 *  \return ones matrix
 */
template<typename T>
inline Matrix<T> ones(unsigned nrows, unsigned ncols) {
  return Matrix<T>(static_cast<T>(1), nrows, ncols);
}

/** \brief Square matrix of ones.
 *
 *  Construct a square matrix of size \a n x \a n and fill it with all elements set to 1.
 * 
 *  In case of complex datatype, matrix is filled with \f$1 + 0i\f$.
 * 
 *  \param n size of the square matrix (the first and the second dimension)
 *  \return zeros matrix
 */
template<typename T>
inline Matrix<T> ones(unsigned n) {
  return ones<T>(n,n);
}

/** \brief Identity matrix.
 *
 *  Construct a square identity matrix. In case of complex datatype, the diagonal elements are set to \f$1 + 0i\f$.
 * 
 *  \param n size of the square matrix (the first and the second dimension)
 *  \return zeros matrix
 */
template<typename T>
Matrix<T> eye(unsigned n) {
  Matrix<T> A(static_cast<T>(0), n, n);
  for (unsigned i = 0; i < n; i++)
    A(i,i) = static_cast<T>(1);
  return A;
}

/** \brief Diagonal matrix from array.
 *
 *  Constructs a diagonal matrix of size \a n x \a n, whose diagonal elements are set to the elements stored in the \a array.
 * 
 *  \param array pointer to the first element of the array where the diagonal elements are stored
 *  \param n size of the matrix to be constructed. Also, a number of elements stored in \a array
 *  \return diagonal matrix
 */
template<typename T>
Matrix<T> diag(const T* array, size_t n) {
  Matrix<T> A(static_cast<T>(0), n, n);
  for (unsigned i = 0; i < n; i++) {
    A(i,i) = array[i];
  }
  return A;
}

/** \brief Diagonal matrix from std::vector.
 *
 *  Constructs a diagonal matrix, whose diagonal elements are set to the elements stored in the std::vector \a v. Size of the 
 *  matrix is equal to the vector size.
 * 
 *  \param v vector of diagonal elements
 *  \return diagonal matrix
 */
template<typename T>
inline Matrix<T> diag(const std::vector<T>& v) {
  return diag(v.data(), v.size());
}

/** \brief Diagonal extraction.
 *
 *  Store diagonal elements of a square matrix in std::vector.
 * 
 *  \param A square matrix
 *  \return vector of diagonal elements
 * 
 *  \throws std::runtime_error when the input matrix is not square
 */
template<typename T>
std::vector<T> diag(const Matrix<T>& A) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  std::vector<T> v;
  v.resize(A.rows());

  for (unsigned i = 0; i < A.rows(); i++)
    v[i] = A(i,i);
  return v;
}

/** \brief Circulant matrix from array.
 *
 *  Constructs a circulant matrix of size \a n x \a n by taking the elements from \a array as the first column. 
 * 
 *  \param array pointer to the first element of the array where the elements of the first column are stored
 *  \param n size of the matrix to be constructed. Also, a number of elements stored in \a array
 *  \return circulant matrix
 */
template<typename T>
Matrix<T> circulant(const T* array, unsigned n) {
  Matrix<T> A(n, n);
  for (unsigned j = 0; j < n; j++)
    for (unsigned i = 0; i < n; i++)
      A((i+j) % n,j) = array[i];
  return A;
}

/** \brief Create complex matrix from real and imaginary matrices.
 *
 *  Constructs a matrix of std::complex type from real matrices providing real and imaginary parts.
 *  Input matrices \a Re and \a Im matrices must have the same dimensions.
 * 
 *  \param Re real part matrix
 *  \param Im imaginary part matrix
 *  \return complex matrix with real part set to \a Re and imaginary part to \a Im
 * 
 *  \throws std::runtime_error when \a Re and \a Im have different dimensions
 */
template<typename T>
Matrix<std::complex<T>> make_complex(const Matrix<T>& Re, const Matrix<T>& Im) {
  if (Re.rows() != Im.rows() || Re.cols() != Im.cols()) throw std::runtime_error("Size of input matrices does not match");

  Matrix<std::complex<T> > C(Re.rows(),Re.cols());
  for (unsigned n = 0; n < Re.numel(); n++) {
    C(n).real(Re(n));
    C(n).imag(Im(n)); 
  }

  return C;
}

/** \brief Create complex matrix from real matrix.
 *
 *  Constructs a matrix of std::complex type from real and imaginary matrices. 
 * 
 *  \param Re real part matrix
 *  \return complex matrix with real part set to \a Re and imaginary part to zero
 */
template<typename T>
Matrix<std::complex<T>> make_complex(const Matrix<T>& Re) {
  Matrix<std::complex<T>> C(Re.rows(),Re.cols());

  for (unsigned n = 0; n < Re.numel(); n++) {
    C(n).real(Re(n));
    C(n).imag(static_cast<T>(0)); 
  }

  return C;
}

/** \brief Get real part of complex matrix.
 *
 *  Constructs a matrix of real type from std::complex matrix by taking its real part.
 */
template<typename T>
Matrix<T> real(const Matrix<std::complex<T>>& C) {
  Matrix<T> Re(C.rows(),C.cols());

  for (unsigned n = 0; n < C.numel(); n++)
    Re(n) = C(n).real();

  return Re;
}

/** \brief Get imaginary part of complex matrix.
 *
 *  Constructs a matrix of real type from std::complex matrix by taking its imaginary part.
 */
template<typename T>
Matrix<T> imag(const Matrix<std::complex<T>>& C) {
  Matrix<T> Re(C.rows(),C.cols());

  for (unsigned n = 0; n < C.numel(); n++)
    Re(n) = C(n).imag();

  return Re;
}

/** \brief Circulant matrix from std::vector.
 *
 *  Constructs a circulant matrix, whose the elements of the first column are set to the elements stored in the 
 *  std::vector \a v. Size of the matrix is equal to the vector size.
 * 
 *  \param v vector with data
 *  \return circulant matrix
 */
template<typename T>
inline Matrix<T> circulant(const std::vector<T>& v) {
  return circulant(v.data(), v.size());
}

/** \brief Transpose a matrix.
 *  
 * Returns a matrix that is a transposition of an input matrix.
 */
template<typename T>
inline Matrix<T> transpose(const Matrix<T>& A) {
  return A.transpose();
}

/** \brief Transpose a complex matrix.
 *
 *  Returns a matrix that is a conjugate (Hermitian) transposition of an input matrix.
 *  Conjugate transpose applies a conjugate operation to all elements in addition to element transposition.
 */
template<typename T>
inline Matrix<T> ctranspose(const Matrix<T>& A) {
  return A.ctranspose();
}

/** \brief Circular shift.
 *  
 *  Returns a matrix that is created by shifting the columns and rows of an input matrix in a circular manner.
 * 
 *  If the specified shift factor is a positive value, columns of the matrix are shifted towards right or rows are 
 *  shifted towards the bottom. A negative value may be used to apply shifts in opposite directions.
 * 
 *  \param A matrix
 *  \param row_shift row shift factor
 *  \param col_shift column shift factor
 *  \return matrix inverse
 */
template<typename T>
Matrix<T> circshift(const Matrix<T>& A, int row_shift, int col_shift) {
  Matrix<T> B(A.rows(), A.cols());
  for (int i = 0; i < A.rows(); i++) {
    int ii = (i + row_shift) % A.rows();
    for (int j = 0; j < A.cols(); j++) {
      int jj = (j + col_shift) % A.cols();
      B(ii,jj) = A(i,j);
    }
  }
  return B;
}

/** \brief Repeat matrix.
 *  
 *  Form a block matrix of size \a m by \a n, with a copy of matrix A as each element.
 * 
 *  \param A input matrix to be repeated
 *  \param m number of times to repeat matrix A in vertical dimension (rows)
 *  \param n number of times to repeat matrix A in horizontal dimension (columns)
 */
template<typename T>
Matrix<T> repmat(const Matrix<T>& A, unsigned m, unsigned n) {
  Matrix<T> B(m * A.rows(), n * A.cols());

  for (unsigned cb = 0; cb < n; cb++)
    for (unsigned rb = 0; rb < m; rb++)
      for (unsigned c = 0; c < A.cols(); c++)
        for (unsigned r = 0; r < A.rows(); r++)
          B(rb*A.rows() + r, cb*A.cols() + c) = A(r, c);

  return B;
}

/** \brief Horizontal matrix concatenation.
 *  
 *  Concatenates two input matrices \a A and \a B horizontally to form a concatenated matrix \f$C = [A|B]\f$.
 * 
 *  \throws std::runtime_error when the number of rows in \a A and \a B is not equal.
 */
template<typename T>
Matrix<T> concatenate_horizontal(const Matrix<T>& A, const Matrix<T>& B) {
  if (A.rows() != B.rows()) throw std::runtime_error("Unmatching number of rows for horizontal concatenation");
  
  Matrix<T> C(A.rows(), A.cols() + B.cols());

  for (unsigned c = 0; c < A.cols(); c++)
    for (unsigned r = 0; r < A.rows(); r++)
      C(r,c) = A(r,c);

  for (unsigned c = 0; c < B.cols(); c++)
    for (unsigned r = 0; r < B.rows(); r++)
      C(r,c+A.cols()) = B(r,c);

  return C;
}

/** \brief Vertical matrix concatenation.
 *  
 *  Concatenates two input matrices \a A and \a B vertically to form a concatenated matrix \f$C = [A|B]^T\f$.
 * 
 *  \throws std::runtime_error when the number of columns in \a A and \a B is not equal.
 */
template<typename T>
Matrix<T> concatenate_vertical(const Matrix<T>& A, const Matrix<T>& B) {
  if (A.cols() != B.cols()) throw std::runtime_error("Unmatching number of columns for vertical concatenation");
  
  Matrix<T> C(A.rows() + B.rows(), A.cols());

  for (unsigned c = 0; c < A.cols(); c++)
    for (unsigned r = 0; r < A.rows(); r++)
      C(r,c) = A(r,c);

  for (unsigned c = 0; c < B.cols(); c++)
    for (unsigned r = 0; r < B.rows(); r++)
      C(r+A.rows(),c) = B(r,c);

  return C;
}

/** \brief Frobenius norm.
 *  
 *  Calculates Frobenius norm of a matrix.
 * 
 *  More information https://en.wikipedia.org/wiki/Matrix_norm#Frobenius_norm
 */
template<typename T>
double norm_fro(const Matrix<T>& A) {
  double sum = 0;

  for (unsigned i = 0; i < A.numel(); i++)
    sum += Util::creal(A(i) * Util::cconj(A(i)));

  return std::sqrt(sum);
}

/** \brief Matrix \f$p=1\f$ norm (column norm).
 *  
 *  Calculates \f$p=1\f$ norm \f$||A||_1\f$ of the input matrix. The \f$p=1\f$ norm is defined as the
 *  maximum absolute sum of elements of each column.
 */
template<typename T>
double norm_p1(const Matrix<T>& A) {
  double max_sum = 0.0;

  for (unsigned c = 0; c < A.cols(); c++) {
    double sum = 0.0;

    for (unsigned r = 0; r < A.rows(); r++)
      sum += std::abs(A(r,c));

    if (sum > max_sum)
      max_sum = sum;
  }

  return max_sum;
}

/** \brief Matrix \f$p=\infty\f$ norm (row norm).
 *  
 *  Calculates \f$p=\infty\f$ norm \f$||A||_\infty\f$ of the input matrix. The \f$p=\infty\f$ norm is defined as
 *  the maximum absolute sum of elements of each row.
 */
template<typename T>
double norm_inf(const Matrix<T>& A) {
  double max_sum = 0.0;

  for (unsigned r = 0; r < A.rows(); r++) {
    double sum = 0.0;

    for (unsigned c = 0; c < A.cols(); c++)
      sum += std::abs(A(r,c));

    if (sum > max_sum)
      max_sum = sum;
  }

  return max_sum;
}

/** \brief Extract triangular lower part.
 *  
 *  Return a new matrix formed by extracting the lower triangular part of the input matrix, and setting all other 
 *  elements to zero.
 */
template<typename T>
Matrix<T> tril(const Matrix<T>& A) {
  Matrix<T> B(A);

  for (unsigned row = 0; row < B.rows(); row++)
    for (unsigned col = row+1; col < B.cols(); col++)
      B(row,col) = static_cast<T>(0);

  return B;
}

/** \brief Extract triangular upper part.
 *  
 *  Return a new matrix formed by extracting the upper triangular part of the input matrix, and setting all other 
 *  elements to zero.
 */
template<typename T>
Matrix<T> triu(const Matrix<T>& A) {
  Matrix<T> B(A);

  for (unsigned col = 0; col < B.cols(); col++)
    for (unsigned row = col+1; row < B.rows(); row++)
      B(row,col) = static_cast<T>(0);

  return B;
}

/** \brief Lower triangular matrix check.
 *  
 *  Return true if A is a lower triangular matrix, i.e., when it has nonzero entries only on the main diagonal and 
 *  below. This function uses hard decision for equality check.
 */
template<typename T>
bool istril(const Matrix<T>& A) {
  for (unsigned row = 0; row < A.rows(); row++)
    for (unsigned col = row+1; col < A.cols(); col++)
      if (A(row,col) != static_cast<T>(0)) return false;
  return true;
}

/** \brief Lower triangular matrix check.
 *  
 *  Return true if A is a lower triangular matrix, i.e., when it has nonzero entries only on the main diagonal and 
 *  below. This function uses hard decision for equality check.
 */
template<typename T>
bool istriu(const Matrix<T>& A) {
  for (unsigned col = 0; col < A.cols(); col++)
    for (unsigned row = col+1; row < A.rows(); row++)
      if (A(row,col) != static_cast<T>(0)) return false;
  return true;
}

/** \brief Hessenberg matrix check.
 *  
 *  Return true if \a A is an upper Hessenberg matrix, i.e., it is square and has only zero entries below the first 
 *  subdiagonal. This function uses hard decision for equality check.
 */
template<typename T>
bool ishess(const Matrix<T>& A) {
  if (!A.issquare())
    return false;
  for (unsigned row = 2; row < A.rows(); row++)
    for (unsigned col = 0; col < row-2; col++)
      if (A(row,col) != static_cast<T>(0)) return false;
  return true;
}

/** \brief Applies custom function element-wise in-place. 
 *  
 *  Applies specified function \a func to all elements of the input matrix.
 * 
 *  This function applies operation to the elements in-place (zero-copy). In order to apply the function 
 *  to the copy of the matrix without modifying the input one, use \ref Mtx::foreach_elem_copy().
 * 
 *  \param A input matrix to be modified
 *  \param func function to be applied element-wise to \a A. It inputs one variable of template type T and returns 
 *         variable of the same type.
 */
template<typename T>
inline void foreach_elem(Matrix<T>& A, std::function<T(T)> func) {
  for (unsigned i = 0; i < A.numel(); i++)
    A(i) = func(A(i));
}

/** \brief Applies custom function element-wise with matrix copy. 
 *  
 *  Applies the specified function \a func to all elements of the input matrix.
 * 
 *  This function applies operation to the copy of the input matrix. 
 *  For in-place (zero-copy) operation, use \ref Mtx::foreach_elem().
 * 
 *  \param A input matrix
 *  \param func function to be applied element-wise to A. It inputs one variable of template type T and returns 
 *         variable of the same type
 *  \return output matrix whose elements were modified by the function \a func
 */
template<typename T>
inline Matrix<T> foreach_elem_copy(const Matrix<T>& A, std::function<T(T)> func) {
  Matrix<T> B(A);
  foreach_elem(B, func);
  return B;
}

/** \brief Permute rows of the matrix.
 *  
 *  Creates a copy of the matrix with permutation of rows specified as input parameter. Each row in the new matrix 
 *  is a copy of respective row from the input matrix indexed by permutation vector.
 *  The size of the output matrix is \a perm.size() x \a A.cols().
 * 
 *  \param A input matrix
 *  \param perm permutation vector with row indices
 *  \return output matrix created by row permutation of \a A
 * 
 *  \throws std::runtime_error when permutation vector is empty
 *  \throws std::out_of_range when any index in permutation vector is out of range
 *          Thrown only when MATRIX_STRICT_BOUNDS_CHECK is enabled during compilation.
 *
 */
template<typename T>
Matrix<T> permute_rows(const Matrix<T>& A, const std::vector<unsigned> perm) {
  if (perm.empty()) throw std::runtime_error("Permutation vector is empty");

#ifdef MATRIX_STRICT_BOUNDS_CHECK
  for (unsigned p = 0; p < perm.size(); p++)
    if (!(perm[p] < A.rows())) throw std::out_of_range("Index in permutation vector out of range");
#endif

  Matrix<T> B(perm.size(), A.cols());

  for (unsigned p = 0; p < perm.size(); p++)
    for (unsigned c = 0; c < A.cols(); c++)
      B(p,c) = A(perm[p],c);

  return B;
}

/** \brief Permute columns of the matrix.
 *  
 *  Creates a copy of the matrix with permutation of columns specified as input parameter. Each column in the new 
 *  matrix is a copy of respective column from the input matrix indexed by permutation vector.
 *  The size of the output matrix is \a A.rows() x \a perm.size().
 * 
 *  \param A input matrix
 *  \param perm permutation vector with column indices
 *  \return output matrix created by column permutation of \a A
 * 
 *  \throws std::runtime_error when permutation vector is empty
 *  \throws std::out_of_range when any index in permutation vector is out of range
 *          Thrown only when MATRIX_STRICT_BOUNDS_CHECK is enabled during compilation.
 */
template<typename T>
Matrix<T> permute_cols(const Matrix<T>& A, const std::vector<unsigned> perm) {
  if (perm.empty()) throw std::runtime_error("Permutation vector is empty");

#ifdef MATRIX_STRICT_BOUNDS_CHECK
  for (unsigned p = 0; p < perm.size(); p++)
    if (!(perm[p] < A.cols())) throw std::out_of_range("Index in permutation vector out of range");
#endif

  Matrix<T> B(A.rows(), perm.size());

  for (unsigned p = 0; p < perm.size(); p++)
    for (unsigned r = 0; r < A.rows(); r++)
      B(r,p) = A(r,perm[p]);

  return B;
}

/** \brief Permute both rows and columns of the matrix.
 *  
 *  Creates a copy of the matrix with permutation of rows and columns specified as input parameter. The result 
 *  of this function is equivalent to performing row and column permutations separately - see \ref Mtx::permute_rows() 
 *  and \ref Mtx::permute_cols().
 * 
 *  The size of the output matrix is \a perm_rows.size() x \a perm_cols.size().
 * 
 *  \param A input matrix
 *  \param perm_rows permutation vector with row indices
 *  \param perm_cols permutation vector with column indices
 *  \return output matrix created by row and column permutation of \a A
 * 
 *  \throws std::runtime_error when any of permutation vectors is empty
 *  \throws std::out_of_range when any index in permutation vector is out of range.
 *          Thrown only when MATRIX_STRICT_BOUNDS_CHECK is enabled during compilation.
 */
template<typename T>
Matrix<T> permute_rows_and_cols(const Matrix<T>& A, const std::vector<unsigned> perm_rows, const std::vector<unsigned> perm_cols) {
  if (perm_rows.empty()) throw std::runtime_error("Row permutation vector is empty");
  if (perm_cols.empty()) throw std::runtime_error("Column permutation vector is empty");

#ifdef MATRIX_STRICT_BOUNDS_CHECK
  for (unsigned pc = 0; pc < perm_cols.size(); pc++)
    if (!(perm_cols[pc] < A.cols())) throw std::out_of_range("Column index in permutation vector out of range");

  for (unsigned pr = 0; pr < perm_rows.size(); pr++)
    if (!(perm_rows[pr] < A.rows())) throw std::out_of_range("Row index in permutation vector out of range");
#endif

  Matrix<T> B(perm_rows.size(), perm_cols.size());

  for (unsigned pc = 0; pc < perm_cols.size(); pc++)
    for (unsigned pr = 0; pr < perm_rows.size(); pr++)
      B(pr,pc) = A(perm_rows[pr],perm_cols[pc]);

  return B;
}

/** \brief Matrix multiplication.
 *  
 *  Performs multiplication of two matrices.
 * 
 *  This function supports template parameterization of input matrix transposition, providing better efficiency
 *  than in case of using \ref Mtx::ctranspose() function due to zero-copy operation. In case of complex matrices, 
 *  conjugate (Hermitian) transpose is used.
 * 
 *  \tparam transpose_first if set to true, the left-side input matrix will be transposed during operation
 *  \tparam transpose_second if set to true, the right-side input matrix will be transposed during operation
 *  
 *  \param A left-side matrix of size \a N x \a M (after transposition)
 *  \param B right-side matrix of size \a M x \a K (after transposition)
 *  \return output matrix of size \a N x \a K
 */
template<typename T, bool transpose_first = false, bool transpose_second = false>
Matrix<T> mult(const Matrix<T>& A, const Matrix<T>& B) {
  // Adjust dimensions based on transpositions
  unsigned rows_A = transpose_first ? A.cols() : A.rows();
  unsigned cols_A = transpose_first ? A.rows() : A.cols();
  unsigned rows_B = transpose_second ? B.cols() : B.rows();
  unsigned cols_B = transpose_second ? B.rows() : B.cols();

  if (cols_A != rows_B) throw std::runtime_error("Unmatching matrix dimensions for mult");

  Matrix<T> C(static_cast<T>(0), rows_A, cols_B);

  for (unsigned i = 0; i < rows_A; i++)
    for (unsigned j = 0; j < cols_B; j++)
      for (unsigned k = 0; k < cols_A; k++)
      C(i,j) += (transpose_first  ? Util::cconj(A(k,i)) : A(i,k)) *
                (transpose_second ? Util::cconj(B(j,k)) : B(k,j));

  return C;
}

/** \brief Matrix multiplication with addition.
 *  
 *  Performs matrix multiplication and addition according to the formula \f$A \cdot B + C\f$.
 * 
 *  This function supports template parameterization of input matrix transposition, providing better efficiency
 *  than in case of using \ref Mtx::ctranspose() function due to zero-copy operation. In case of complex matrices, 
 *  conjugate (Hermitian) transpose is used.
 * 
 *  \tparam transpose_A if set to true, matrix \f$A\f$ shall be transposed during operation
 *  \tparam transpose_B if set to true, matrix \f$B\f$ shall be transposed during operation
 *  \tparam transpose_C if set to true, matrix \f$C\f$ shall be transposed during operation
 *  
 *  \param A left-side factor matrix of size \a N x \a M (after transposition)
 *  \param B right-side factor matrix of size \a M x \a K (after transposition)
 *  \param C matrix to be added to the result of multiplication of size \a N x \a K (after transposition)
 *  \return output matrix of size \a N x \a K
 */
template<typename T, bool transpose_A = false, bool transpose_B = false, bool transpose_C = false>
Matrix<T> mult_and_add(const Matrix<T>& A, const Matrix<T>& B, const Matrix<T>& C) {
  // Adjust dimensions based on transpositions
  unsigned rows_A = transpose_A ? A.cols() : A.rows();
  unsigned cols_A = transpose_A ? A.rows() : A.cols();
  unsigned rows_B = transpose_B ? B.cols() : B.rows();
  unsigned cols_B = transpose_B ? B.rows() : B.cols();
  unsigned rows_C = transpose_C ? C.cols() : C.rows();
  unsigned cols_C = transpose_C ? C.rows() : C.cols();

  if ((cols_A != rows_B) || (rows_A != rows_C) || (cols_B != cols_C)) 
    throw std::runtime_error("Unmatching matrix dimensions for mult_and_add");

  Matrix<T> D(rows_C, cols_C);

  for (unsigned i = 0; i < rows_A; i++) {
    for (unsigned j = 0; j < cols_B; j++) {
      D(i,j) = transpose_C ? Util::cconj(C(j,i)) : C(i,j);
      for (unsigned k = 0; k < cols_A; k++) {
        D(i,j) += (transpose_A ? Util::cconj(A(k,i)) : A(i,k)) *
                  (transpose_B ? Util::cconj(B(j,k)) : B(k,j));
      }
    }
  }

  return D;
}

/** \brief Matrix Hadamard (element-wise) multiplication.
 *  
 *  Performs Hadamard (element-wise) multiplication of two matrices.
 * 
 *  This function supports template parameterization of input matrix transposition, providing better efficiency
 *  than in case of using \ref Mtx::ctranspose() function due to zero-copy operation. In case of complex matrices, 
 *  conjugate (Hermitian) transpose is used.
 * 
 *  \tparam transpose_first if set to true, the left-side input matrix will be transposed during operation
 *  \tparam transpose_second if set to true, the right-side input matrix will be transposed during operation
 *  
 *  \param A left-side matrix of size \a N x \a M (after transposition)
 *  \param B right-side matrix of size \a N x \a M (after transposition)
 *  \return output matrix of size \a N x \a M
 */
template<typename T, bool transpose_first = false, bool transpose_second = false>
Matrix<T> mult_hadamard(const Matrix<T>& A, const Matrix<T>& B) {
  // Adjust dimensions based on transpositions
  unsigned rows_A = transpose_first ? A.cols() : A.rows();
  unsigned cols_A = transpose_first ? A.rows() : A.cols();
  unsigned rows_B = transpose_second ? B.cols() : B.rows();
  unsigned cols_B = transpose_second ? B.rows() : B.cols();

  if ((rows_A != rows_B) || (cols_A != cols_B)) throw std::runtime_error("Unmatching matrix dimensions for mult_hadamard");

  Matrix<T> C(static_cast<T>(0), rows_A, cols_A);

  for (unsigned i = 0; i < rows_A; i++)
    for (unsigned j = 0; j < cols_A; j++)
      C(i,j) += (transpose_first  ? Util::cconj(A(j,i)) : A(i,j)) *
                (transpose_second ? Util::cconj(B(j,i)) : B(i,j));

  return C;
}

/** \brief Matrix addition.
 *  
 *  Performs addition of two matrices.
 * 
 *  This function supports template parameterization of input matrix transposition, providing better efficiency
 *  than in case of using \ref Mtx::ctranspose() function due to zero-copy operation. In case of complex matrices, 
 *  conjugate (Hermitian) transpose is used.
 * 
 *  \tparam transpose_first if set to true, the left-side input matrix will be transposed during operation
 *  \tparam transpose_second if set to true, the right-side input matrix will be transposed during operation
 *  
 *  \param A left-side matrix of size \a N x \a M (after transposition)
 *  \param B right-side matrix of size \a N x \a M (after transposition)
 *  \return output matrix of size \a N x \a M
 */
template<typename T, bool transpose_first = false, bool transpose_second = false>
Matrix<T> add(const Matrix<T>& A, const Matrix<T>& B) {
  // Adjust dimensions based on transpositions
  unsigned rows_A = transpose_first ? A.cols() : A.rows();
  unsigned cols_A = transpose_first ? A.rows() : A.cols();
  unsigned rows_B = transpose_second ? B.cols() : B.rows();
  unsigned cols_B = transpose_second ? B.rows() : B.cols();

  if ((rows_A != rows_B) || (cols_A != cols_B)) throw std::runtime_error("Unmatching matrix dimensions for add");

  Matrix<T> C(static_cast<T>(0), rows_A, cols_A);

  for (unsigned i = 0; i < rows_A; i++)
    for (unsigned j = 0; j < cols_A; j++)
      C(i,j) += (transpose_first  ? Util::cconj(A(j,i)) : A(i,j)) +
                (transpose_second ? Util::cconj(B(j,i)) : B(i,j));

  return C;
}

/** \brief Matrix subtraction.
 *  
 *  Performs subtraction of two matrices.
 * 
 *  This function supports template parameterization of input matrix transposition, providing better efficiency 
 *  than in case of using \ref Mtx::ctranspose() function due to zero-copy operation. In case of complex matrices, 
 *  conjugate (Hermitian) transpose is used.
 * 
 *  \tparam transpose_first if set to true, the left-side input matrix will be transposed during operation
 *  \tparam transpose_second if set to true, the right-side input matrix will be transposed during operation
 *  
 *  \param A left-side matrix of size \a N x \a M (after transposition)
 *  \param B right-side matrix of size \a N x \a M (after transposition)
 *  \return output matrix of size \a N x \a M
 */
template<typename T, bool transpose_first = false, bool transpose_second = false>
Matrix<T> subtract(const Matrix<T>& A, const Matrix<T>& B) {
  // Adjust dimensions based on transpositions
  unsigned rows_A = transpose_first ? A.cols() : A.rows();
  unsigned cols_A = transpose_first ? A.rows() : A.cols();
  unsigned rows_B = transpose_second ? B.cols() : B.rows();
  unsigned cols_B = transpose_second ? B.rows() : B.cols();

  if ((rows_A != rows_B) || (cols_A != cols_B)) throw std::runtime_error("Unmatching matrix dimensions for subtract");

  Matrix<T> C(static_cast<T>(0), rows_A, cols_A);

  for (unsigned i = 0; i < rows_A; i++)
    for (unsigned j = 0; j < cols_A; j++)
      C(i,j) += (transpose_first  ? Util::cconj(A(j,i)) : A(i,j)) -
                (transpose_second ? Util::cconj(B(j,i)) : B(i,j));

  return C;
}

/** \brief Multiplication of matrix by std::vector
 *  
 *  Performs the right multiplication of a matrix with a column vector represented by std::vector. The result 
 *  of this operation is also a std::vector.
 *  
 *  This function supports template parameterization of input matrix transposition, providing better efficiency 
 *  than in case of using \ref Mtx::ctranspose() function due to zero-copy operation. In case of complex matrices, 
 *  conjugate (Hermitian) transpose is used.
 * 
 *  \tparam transpose_matrix if set to true, the matrix will be transposed during operation
 * 
 *  \param A input matrix of size \a N x \a M
 *  \param v std::vector of size \a M
 *  \return std::vector of size \a N being the result of multiplication
 */
template<typename T, bool transpose_matrix = false>
std::vector<T> mult(const Matrix<T>& A, const std::vector<T>& v) {
  // Adjust dimensions based on transpositions
  unsigned rows_A = transpose_matrix ? A.cols() : A.rows();
  unsigned cols_A = transpose_matrix ? A.rows() : A.cols();

  if (cols_A != v.size()) throw std::runtime_error("Unmatching matrix dimensions for mult");

  std::vector<T> u(rows_A, static_cast<T>(0));
  for (unsigned r = 0; r < rows_A; r++)
    for (unsigned c = 0; c < cols_A; c++)
      u[r] += v[c] * (transpose_matrix ? Util::cconj(A(c,r)) : A(r,c));

  return u;
}

/** \brief Multiplication of std::vector by matrix
 *  
 *  Performs the left multiplication of a std::vector with a matrix. The result of this operation is also a 
 *  std::vector.
 *  
 *  This function supports template parameterization of input matrix transposition, providing better efficiency 
 *  than in case of using \ref Mtx::ctranspose() function due to zero-copy operation. In case of complex matrices, 
 *  conjugate (Hermitian) transpose is used.
 * 
 *  \tparam transpose_matrix if set to true, the matrix will be transposed during operation
 * 
 *  \param v std::vector of size \a N
 *  \param A input matrix of size \a N x \a M
 *  \return std::vector of size \a M being the result of multiplication
 */
template<typename T, bool transpose_matrix = false>
std::vector<T> mult(const std::vector<T>& v, const Matrix<T>& A) {
  // Adjust dimensions based on transpositions
  unsigned rows_A = transpose_matrix ? A.cols() : A.rows();
  unsigned cols_A = transpose_matrix ? A.rows() : A.cols();

  if (rows_A != v.size()) throw std::runtime_error("Unmatching matrix dimensions for mult");

  std::vector<T> u(cols_A, static_cast<T>(0));
  for (unsigned c = 0; c < cols_A; c++)
    for (unsigned r = 0; r < rows_A; r++)
      u[c] += v[r] * (transpose_matrix ? Util::cconj(A(c,r)) : A(r,c));
  
  return u;
}

/** \brief Addition of scalar to matrix.
 *  
 *  Adds a scalar \f$s\f$ from each element of the input matrix. 
 *  This method does not modify the input matrix but creates a copy.
 */
template<typename T>
Matrix<T> add(const Matrix<T>& A, T s) {
  Matrix<T> B(A.rows(), A.cols());
  for (unsigned i = 0; i < A.numel(); i++)
    B(i) = A(i) + s;
  return B;
}

/** \brief Subtraction of scalar from matrix.
 *  
 *  Subtracts a scalar \f$s\f$ from each element of the input matrix. 
 *  This method does not modify the input matrix but creates a copy.
 */
template<typename T>
Matrix<T> subtract(const Matrix<T>& A, T s) {
  Matrix<T> B(A.rows(), A.cols());
  for (unsigned i = 0; i < A.numel(); i++)
    B(i) = A(i) - s;
  return B;
}

/** \brief Multiplication of matrix by scalar.
 *  
 *  Multiplies each element of the input matrix by a scalar \f$s\f$. 
 *  This method does not modify the input matrix but creates a copy.
 */
template<typename T>
Matrix<T> mult(const Matrix<T>& A, T s) {
  Matrix<T> B(A.rows(), A.cols());
  for (unsigned i = 0; i < A.numel(); i++)
    B(i) = A(i) * s;
  return B;
}

/** \brief Division of matrix by scalar.
 *  
 *  Divides each element of the input matrix by a scalar \f$s\f$. 
 *  This method does not modify the input matrix but creates a copy.
 */
template<typename T>
Matrix<T> div(const Matrix<T>& A, T s) {
  Matrix<T> B(A.rows(), A.cols());
  for (unsigned i = 0; i < A.numel(); i++)
    B(i) = A(i) / s;
  return B;
}

/** \brief Converts matrix to std::string.
 *  
 *  This function converts a matrix into a string representation in row-major order. Each element of the matrix 
 *  is converted to its string equivalent. Elements within the same row are separated by the \a col_separator character. 
 *  Rows are separated by the \a row_separator character.
 * 
 *  \param A inpur matrix to be converted
 *  \param col_separator character used to separate elements within the same row. The default character is the space
 *  \param row_separator character used to separate rows. The default character is the new line '\\n'
 *  \return std::string representation of the input matrix
 */
template<typename T>
std::string to_string(const Matrix<T>& A, char col_separator = ' ', char row_separator = '\n') {
  std::stringstream ss;
  for (unsigned row = 0; row < A.rows(); row ++) {
    for (unsigned col = 0; col < A.cols(); col ++)
      ss << A(row,col) << col_separator;
    if (row < static_cast<unsigned>(A.rows()-1)) ss << row_separator;
  }
  return ss.str();
}

/** \brief Matrix ostream operator.
 *  
 *  Formats a string incorporating elements of the matrix in row-major order. Elements within the same row are 
 *  separated by the space character. Different lines (rows) are separated by the endline delimiter std::endl.
 * 
 *  This function does not allow to control the default delimiter characters. Refer to \ref Mtx::to_string()
 *  if control of delimiter characters is needed.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& A) {
  for (unsigned row = 0; row < A.rows(); row ++) {
    for (unsigned col = 0; col < A.cols(); col ++)
      os << A(row,col) << " ";
    if (row < static_cast<unsigned>(A.rows()-1)) os << std::endl;
  }
  return os;
}

/** \brief Matrix sum.
 *  
 *  Calculates a sum of two matrices \f$A + B\f$. \f$A\f$ and \f$B\f$ must be the same size.
 */
template<typename T>
inline Matrix<T> operator+(const Matrix<T>& A, const Matrix<T>& B) {
  return add(A,B);
}

/** \brief Matrix subtraction.
 *  
 *  Calculates a subtraction of two matrices \f$A - B\f$. \f$A\f$ and \f$B\f$ must be the same size.
 */
template<typename T>
inline Matrix<T> operator-(const Matrix<T>& A, const Matrix<T>& B) {
  return subtract(A,B);
}

/** \brief Matrix Hadamard product.
 *  
 *  Calculates a Hadamard product of two matrices \f$A \otimes B\f$. \f$A\f$ and \f$B\f$ must be the same size. 
 *  Hadamard product is calculated as an element-wise multiplication between the matrices.
 */
template<typename T>
inline Matrix<T> operator^(const Matrix<T>& A, const Matrix<T>& B) {
  return mult_hadamard(A,B);
}

/** \brief Matrix product.
 *  
 *  Calculates matrix product of two matrices \f$A \cdot B\f$. \f$A\f$ and \f$B\f$ must be the same size.
 */
template<typename T>
inline Matrix<T> operator*(const Matrix<T>& A, const Matrix<T>& B) {
  return mult(A,B);
}

/** \brief Matrix and std::vector product.
 *  
 *  Calculates product between matrix and std::vector \f$A \cdot v\f$. The input vector is assumed to be 
 *  a column vector.
 */
template<typename T>
inline std::vector<T> operator*(const Matrix<T>& A, const std::vector<T>& v) {
  return mult(A,v);
}

/** \brief std::vector and matrix product.
 *  
 *  Calculates product between std::vector and matrix \f$v \cdot A\f$. The input vector is assumed to be 
 *  a row vector.
 */
template<typename T>
inline std::vector<T> operator*(const std::vector<T>& v, const Matrix<T>& A) {
  return mult(v,A);
}

/** \brief Matrix sum with scalar.
 *  
 *  Adds a scalar \a s to each element of the matrix.
 */
template<typename T>
inline Matrix<T> operator+(const Matrix<T>& A, T s) {
  return add(A,s);
}

/** \brief Matrix subtraction with scalar.
 *  
 *  Subtracts a scalar \f$s\f$ from each element of the matrix.
 */
template<typename T>
inline Matrix<T> operator-(const Matrix<T>& A, T s) {
  return subtract(A,s);
}

/** \brief Matrix product with scalar.
 *  
 *  Multiplies each element of the matrix by a scalar \f$s\f$.
 */
template<typename T>
inline Matrix<T> operator*(const Matrix<T>& A, T s) {
  return mult(A,s);
}

/** \brief Matrix division by scalar.
 *  
 *  Divides each element of the matrix by a scalar \f$s\f$.
 */
template<typename T>
inline Matrix<T> operator/(const Matrix<T>& A, T s) {
  return div(A,s);
}

/** Matrix sum with scalar.
 *  Adds a scalar \f$s\f$ to each element of the matrix.
 */
template<typename T>
inline Matrix<T> operator+(T s, const Matrix<T>& A) {
  return add(A,s);
}

/** \brief Matrix product with scalar.
 *  
 *  Multiplies each element of the matrix by a scalar \f$s\f$.
 */
template<typename T>
inline Matrix<T> operator*(T s, const Matrix<T>& A) {
  return mult(A,s);
}

/** \brief Matrix sum.
 *  
 *  Calculates a sum of two matrices \f$ A + B\f$. \f$A\f$ and \f$B\f$ must be the same size.
 */
template<typename T>
inline Matrix<T>& operator+=(Matrix<T>& A, const Matrix<T>& B) {
  return A.add(B);
}

/** \brief Matrix subtraction.
 *  
 *  Subtracts two matrices \f$ A - B\f$. \f$A\f$ and \f$B\f$ must be the same size.
 */
template<typename T>
inline Matrix<T>& operator-=(Matrix<T>& A, const Matrix<T>& B) {
  return A.subtract(B);
}

/** \brief Matrix product.
 *
 *  Calculates matrix product of two matrices \f$A \cdot B\f$. \f$A\f$ and \f$B\f$ must be the same size.
 */
template<typename T>
inline Matrix<T>& operator*=(Matrix<T>& A, const Matrix<T>& B) {
  A = mult(A,B);
  return A;
}

/** \brief Matrix Hadamard product.
 *
 *  Calculates a Hadamard product of two matrices \f$A \otimes B\f$. \f$A\f$ and \f$B\f$ must be the same size. 
 *  Hadamard product is calculated as an element-wise multiplication between the matrices.
 */
template<typename T>
inline Matrix<T>& operator^=(Matrix<T>& A, const Matrix<T>& B) {
  return A.mult_hadamard(B);
}

/** \brief Matrix sum with scalar.
 *
 *  Adds a scalar \f$s\f$ to each element of the matrix.
 */
template<typename T>
inline Matrix<T>& operator+=(Matrix<T>& A, T s) {
  return A.add(s);
}

/** \brief Matrix subtraction with scalar.
 *  
 *  Subtracts a scalar \f$s\f$ from each element of the matrix.
 */
template<typename T>
inline Matrix<T>& operator-=(Matrix<T>& A, T s) {
  return A.subtract(s);
}

/** \brief Matrix product with scalar.
 *  
 *  Multiplies each element of the matrix by a scalar \f$s\f$.
 */
template<typename T>
inline Matrix<T>& operator*=(Matrix<T>& A, T s) {
  return A.mult(s);
}

/** \brief Matrix division by scalar.
 *  
 *  Divides each element of the matrix by a scalar \f$s\f$.
 */
template<typename T>
inline Matrix<T>& operator/=(Matrix<T>& A, T s) {
  return A.div(s);
}

/** \brief Matrix equality check operator.
 *  
 *  Returns true, if both matrices are the same size and all of the element are equal value.
 */
template<typename T>
inline bool operator==(const Matrix<T>& A, const Matrix<T>& b) {
  return A.isequal(b);
}

/** \brief Matrix non-equality check operator.
 *  
 *  Returns true, if both matrices are not the same size or not all of the elements are equal value.
 */
template<typename T>
inline bool operator!=(const Matrix<T>& A, const Matrix<T>& b) {
  return !(A.isequal(b));
}

/** \brief Kronecker product.
 *  
 *  Form the Kronecker product of two matrices. Kronecker product is defined block by block as 
 *  \f$C = [ A(i,j) \cdot B ]\f$.
 * 
 *  More information: https://en.wikipedia.org/wiki/Kronecker_product
 */
template<typename T>
Matrix<T> kron(const Matrix<T>& A, const Matrix<T>& B) {
    const unsigned rows_A = A.rows();
    const unsigned cols_A = A.cols();
    const unsigned rows_B = B.rows();
    const unsigned cols_B = B.cols();

    unsigned rows_C = rows_A * rows_B;
    unsigned cols_C = cols_A * cols_B;

    Matrix<T> C(rows_C, cols_C);

    for (unsigned i = 0; i < rows_A; i++)
      for (unsigned j = 0; j < cols_A; j++)
        for (unsigned k = 0; k < rows_B; k++)
          for (unsigned l = 0; l < cols_B; l++)
            C(i*rows_B + k, j*cols_B + l) = A(i,j) * B(k,l);

    return C;
}

/** \brief Adjugate matrix.
 *  
 *  Calculates adjugate of the matrix being the transpose of its cofactor matrix.
 * 
 *  More information: https://en.wikipedia.org/wiki/Adjugate_matrix
 * 
 *  \throws std::runtime_error when the input matrix is not square
 */
template<typename T>
Matrix<T> adj(const Matrix<T>& A) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  Matrix<T> B(A.rows(), A.cols());
  if (A.rows() == 1) { 
    B(0) = static_cast<T>(1.0);
  } else {
    for (unsigned i = 0; i < A.rows(); i++) { 
      for (unsigned j = 0; j < A.cols(); j++) {
        T sgn = static_cast<T>(1.0)(((i + j) % 2 == 0) ? (1.0) : (-1.0));
        B(j,i) = sgn * det(cofactor(A,i,j)); 
      } 
    }
  }
  return B; 
}

/** \brief Cofactor matrix.
 *  
 *  Calculates first minor of the matrix by deleting row \a p and column \a q. Note that this function does not include 
 *  sign change required by cofactor calculation.
 * 
 *  More information: https://en.wikipedia.org/wiki/Cofactor_(linear_algebra)
 *  \param A input square matrix
 *  \param p row to be deleted in the output matrix
 *  \param q column to be deleted in the output matrix 
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws std::out_of_range when row index \a p or column index \a q are out of range
 *  \throws std::runtime_error when input matrix \a A has less than 2 rows
 */
template<typename T>
Matrix<T> cofactor(const Matrix<T>& A, unsigned p, unsigned q) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");
  if (!(p < A.rows())) throw std::out_of_range("Row index out of range");
  if (!(q < A.cols())) throw std::out_of_range("Column index out of range");
  if (A.cols() < 2) throw std::runtime_error("Cofactor calculation requested for matrix with less than 2 rows");

  Matrix<T> c(A.rows()-1,A.cols()-1);
  unsigned i = 0;
  unsigned j = 0;

  for (unsigned row = 0; row < A.rows(); row++) { 
    if (row != p) {
      for (unsigned col = 0; col < A.cols(); col++)
        if (col != q) c(i,j++) = A(row,col); 
      j = 0;
      i++;
    }
  }

  return c;
}

/** \brief Matrix determinant from on LU decomposition.
 *  
 *  Calculates the determinant of a matrix using LU decomposition with pivoting.
 * 
 *  Note that determinant is calculated as a product: \f$det(L) \cdot det(U) \cdot det(P)\f$, where 
 *  determinants of \a L and \a U are calculated as the product of their diagonal elements, when the 
 *  determinant of \a P is either 1 or -1 depending on the number of row swaps performed during the pivoting 
 *  process.
 * 
 *  More information: https://en.wikipedia.org/wiki/Determinant
 * 
 *  \throws std::runtime_error when the input matrix is not square
 */
template<typename T>
T det_lu(const Matrix<T>& A) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  // LU decomposition with pivoting
  auto res = lup(A);

  // Determinants of LU
  T detLU = static_cast<T>(1);

  for (unsigned i = 0; i < res.L.rows(); i++)
    detLU *= res.L(i,i) * res.U(i,i);

  // Determinant of P
  unsigned len = res.P.size();
  T detP = static_cast<T>(1);

  std::vector<unsigned> p(res.P);
  std::vector<unsigned> q;
  q.resize(len);

  for (unsigned i = 0; i < len; i++)
    q[p[i]] = i;

  for (unsigned i = 0; i < len; i++) {
    unsigned j = p[i];
    unsigned k = q[i];
    if (j != i) {
      p[k] = p[i];
      q[j] = q[i];
      detP = - detP;
    }
  }

  return detLU * detP;
}

/** \brief Matrix determinant.
 *  
 *  Calculates determinant of a square matrix. If the size of the matrix is smaller than 4, the determinant is 
 *  calculated using hard-coded formulas. For matrix sizes equal to 4 and more, determinant is calculated 
 *  recursively using Laplace expansion.
 * 
 *  More information: https://en.wikipedia.org/wiki/Determinant
 * 
 *  \throws std::runtime_error when the input matrix is not square
 */
template<typename T>
T det(const Matrix<T>& A) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  if (A.rows() == 1)
    return A(0,0);
  else if (A.rows() == 2)
    return A(0,0)*A(1,1) - A(0,1)*A(1,0);
  else if (A.rows() == 3)
    return A(0,0)*(A(1,1)*A(2,2) - A(1,2)*A(2,1)) -
           A(0,1)*(A(1,0)*A(2,2) - A(1,2)*A(2,0)) +
           A(0,2)*(A(1,0)*A(2,1) - A(1,1)*A(2,0));
  else
    return det_lu(A);
}

/** \brief LU decomposition.
 *  
 *  Performs LU factorization of the matrix into the the product of a lower triangular matrix \a L and 
 *  an upper triangular matrix \a U.
 * 
 *  This function implements LU factorization without pivoting. Use \ref Mtx::lup() if pivoting is required.
 * 
 *  More information: https://en.wikipedia.org/wiki/LU_decomposition
 *  \param A input square matrix to be decomposed 
 *  \return structure containing calculated \a L and \a U matrices
 */
template<typename T>
LU_result<T> lu(const Matrix<T>& A) {
  const unsigned M = A.rows();
  const unsigned N = A.cols();

  LU_result<T> res;
  res.L = eye<T>(M);
  res.U = Matrix<T>(A);

  // aliases
  auto& L = res.L;
  auto& U = res.U;

  if (A.numel() == 0) 
    return res;

  for (unsigned k = 0; k < M-1; k++) {
    for (unsigned i = k+1; i < M; i++) {
      L(i,k) = U(i,k) / U(k,k);
      for (unsigned l = k+1; l < N; l++) {
        U(i,l) -= L(i,k) * U(k,l);
      }
    }
  }

  for (unsigned col = 0; col < N; col++)
    for (unsigned row = col+1; row < M; row++)
      U(row,col) = 0;
  
  return res;
}

/** \brief LU decomposition with pivoting.
 *  
 *  Performs LU factorization with partial pivoting, employing column permutations.
 * 
 *  The input matrix can be re-created from \a L, \a U and \a P using permute_cols() accordingly:
 *  \code
 *  auto r = lup(A);
 *  auto A_rec = permute_cols(r.L * r.U, r.P);
 *  \endcode 
 * 
 *  More information: https://en.wikipedia.org/wiki/LU_decomposition#LU_factorization_with_partial_pivoting
 *  \param A input square matrix to be decomposed 
 *  \return structure containing \a L, \a U and \a P.
 */
template<typename T>
LUP_result<T> lup(const Matrix<T>& A) {
  const unsigned M = A.rows();
  const unsigned N = A.cols();

  // Initialize L, U, and PP
  LUP_result<T> res;

  if (A.numel() == 0) 
    return res;
  
  res.L = eye<T>(M);
  res.U = Matrix<T>(A);
  std::vector<unsigned> PP;

  // aliases
  auto& L = res.L;
  auto& U = res.U;

  PP.resize(N);
  for (unsigned i = 0; i < N; i++)
    PP[i] = i;

  for (unsigned k = 0; k < M-1; k++) {
    // Find the column with the largest absolute value in the current row
    auto max_col_value = std::abs(U(k,k)); 
    unsigned max_col_index = k;
    for (unsigned l = k+1; l < N; l++) {
      auto val = std::abs(U(k,l));
      if (val > max_col_value) {
        max_col_value = val;
        max_col_index = l;
      }
    }

    // Swap columns k and max_col_index in U and update P
    if (max_col_index != k) {
      U.swap_cols(k, max_col_index); // TODO: This could be reworked to avoid column swap in U during every iteration by:
                                     //       1. using PP[k] for column indexing across iterations
                                     //       2. doing just one permutation of U at the end
      std::swap(PP[k], PP[max_col_index]);
    }

    // Update L and U
    for (unsigned i = k+1; i < M; i++) {
      L(i,k) = U(i,k) / U(k,k);
      for (unsigned l = k+1; l < N; l++) {
        U(i,l) -= L(i,k) * U(k,l);
      }
    }
  }

  // Set elements in lower triangular part of U to zero
  for (unsigned col = 0; col < N; col++)
    for (unsigned row = col+1; row < M; row++)
      U(row,col) = 0;

  // Transpose indices in permutation vector
  res.P.resize(N);
  for (unsigned i = 0; i < N; i++)
    res.P[PP[i]] = i;

  return res;
}

/** \brief Reduced row echelon form (with pivot)
 *
 *  Computes the reduced row echelon form of a matrix using the Gauss-Jordan elimination method
 *  by applying a sequence of elementary row operations. Additionally, returns a vector of non-zero
 *  pivot columns on which elimination has been performed. If pivot vector is not required, use
 *  \ref Mtx::rref() function instead.
 *
 *  More information: https://en.wikipedia.org/wiki/Row_echelon_form
 *
 *  \param A input matrix to be reduced
 *  \param tol numerical precision tolerance to determine zero element, defaults to 0
 *  \return structure encapsulating reduced row echelon form of the input matrix and a vector of
 *          the nonzero pivot column indices.
 */
template<typename T>
Matrix_with_pivot<T> rrefp(const Matrix<T>& A, T tol = 0) {
  Matrix_with_pivot<T> BP;

  // aliases
  auto& B = BP.A;
  auto& P = BP.P;

  B = Matrix<T>(A);
  P.reserve(B.cols());

  unsigned row = 0;

  for (unsigned c = 0; c < B.cols(); c++) {
    // stop if already found pivots for all rows
    if (row >= B.rows())
      break;

    // find the pivot row
    T max_val = static_cast<T>(0);
    unsigned pivot_row = row;

    for (unsigned i = row; i < B.rows(); i++) {
      T x = static_cast<T>(std::abs(B(i,c)));
      if (Util::creal(x) > Util::creal(max_val)) {
        max_val = x;
        pivot_row = i;
      }
    }

    if (Util::creal(max_val) <= Util::creal(tol)) {
      // skip column c
      for (unsigned i = row; i < B.rows(); i++)
        B(i,c) = static_cast<T>(0);
    } else {
      P.push_back(c);

      // swap current row with the pivot row
      if (pivot_row != row)
        for (unsigned j = c; j < B.cols(); j++)
          std::swap(B(row,j), B(pivot_row,j));

      // normalize pivot row if not normalized
      if (B(row,c) != static_cast<T>(1)) {
        auto factor = static_cast<T>(1) / B(row,c);
        for (unsigned j = c; j < B.cols(); j++)
          B(row,j) *= factor;
      }

      // eliminate current column
      for (unsigned i = 0; i < B.rows(); i++) {
        if (i != row) {
          auto factor = B(i,c);
          for (unsigned j = c; j < B.cols(); j++)
            B(i,j) -= factor * B(row,j);
        }
      }

      row++;
    }
  }

  return BP;
}

/** \brief Reduced row echelon form
 *
 *  Computes the reduced row echelon form of a matrix using the Gauss-Jordan elimination method
 *  by applying a sequence of elementary row operations.
 *
 *  If a vector of non-zero columns on which elimination has been performed is required, consider
 *  using \ref Mtx::rrefp().
 *
 *  More information: https://en.wikipedia.org/wiki/Row_echelon_form
 *
 *  \param A input matrix to be reduced
 *  \param tol numerical precision tolerance to determine zero element, defaults to 0
 *  \return reduced row echelon form of the input matrix.
 */
template<typename T>
Matrix<T> rref(const Matrix<T>& A, T tol = 0) {
  auto BP = rrefp(A, tol);
  return BP.A;
}

/** \brief Matrix inverse using Gauss-Jordan elimination.
 *  
 *  Calculates an inverse of a square matrix recursively using Gauss-Jordan elimination. If the inverse doesn't exists, 
 *  e.g., because the input matrix was singular, an empty matrix is returned.
 * 
 *  More information: https://en.wikipedia.org/wiki/Gaussian_elimination
 * 
 *  Using this is function is generally not recommended, please refer to \ref Mtx::inv() instead.
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws singular_matrix_exception when input matrix is singular
 */
template<typename T>
Matrix<T> inv_gauss_jordan(const Matrix<T>& A) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");
  
  const unsigned N = A.rows(); 
  Matrix<T> AA(A);
  auto IA = eye<T>(N);
  
  bool found_nonzero;
  for (unsigned j = 0; j < N; j++) {
    found_nonzero = false;
    for (unsigned i = j; i < N; i++) {
      if (AA(i,j) != static_cast<T>(0)) {
        found_nonzero = true;
        for (unsigned k = 0; k < N; k++) {
          std::swap(AA(j,k), AA(i,k));
          std::swap(IA(j,k), IA(i,k));
        }
        if (AA(j,j) != static_cast<T>(1)) {
          T s = static_cast<T>(1) / AA(j,j);
          for (unsigned k = 0; k < N; k++) {
            AA(j,k) *= s;
            IA(j,k) *= s;
          }
        }
        for (unsigned l = 0; l < N; l++) {
          if (l != j) {
            T s = AA(l,j);
            for (unsigned k = 0; k < N; k++) {
              AA(l,k) -= s * AA(j,k);
              IA(l,k) -= s * IA(j,k);
            }
          }
        }
      }
      break;
    }
    // if a row full of zeros is found, the input matrix was singular
    if (!found_nonzero) throw singular_matrix_exception("Singular matrix in inv_gauss_jordan");
  }
  return IA;
}

/** \brief Matrix inverse for lower triangular matrix.
 *  
 *  Calculates an inverse of lower triangular matrix.
 * 
 *  This function provides more optimal performance than \ref Mtx::inv() for lower triangular matrices. However, validation of 
 *  triangular input matrix structure is not performed. It is up to the user to decide when this function can be used and, 
 *  if needed, perform required validations.
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T>
Matrix<T> inv_tril(const Matrix<T>& A) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  const unsigned N = A.rows();

  auto IA = zeros<T>(N);
 
  for (unsigned i = 0; i < N; i++) {
    if (A(i,i) == static_cast<T>(0.0)) throw singular_matrix_exception("Division by zero in inv_tril");

    IA(i,i) = static_cast<T>(1.0) / A(i,i);
    for (unsigned j = 0; j < i; j++) {
      T s = 0.0;
      for (unsigned k = j; k < i; k++)
        s += A(i,k) * IA(k,j);
      IA(i,j) = -s * IA(i,i) ;
    }
  }

  return IA;
}

/** \brief Matrix inverse for upper triangular matrix.
 *  
 *  Calculates an inverse of upper triangular matrix.
 * 
 *  This function provides more optimal performance than \ref Mtx::inv() for upper triangular matrices. However, validation of 
 *  triangular input matrix structure is not performed. It is up to the user to decide when this function can be used and, 
 *  if needed, perform required validations.
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T>
Matrix<T> inv_triu(const Matrix<T>& A) {
  if (!A.issquare()) throw std::runtime_error("Input matrix is not square");

  const unsigned N = A.rows();

  auto IA = zeros<T>(N); 

  for (int i = N - 1; i >= 0; i--) {
    if (A(i,i) == static_cast<T>(0.0)) throw singular_matrix_exception("Division by zero in inv_triu");

    IA(i, i) = static_cast<T>(1.0) / A(i,i);
    for (int j = N - 1; j > i; j--) {
      T s = static_cast<T>(0.0);
      for (int k = i + 1; k <= j; k++)
        s += A(i,k) * IA(k,j);
      IA(i,j) = -s * IA(i,i);
    }
  }

  return IA;
}

/** \brief Matrix inverse for Hermitian positive-definite matrix.
 *  
 *  Calculates an inverse of symmetric (for real input) or Hermitian (for complex input) positive definite matrix using
 *  Cholesky decomposition.
 * 
 *  This function provides more optimal performance than \ref Mtx::inv() for symmetric matrices. However, validation of
 *  input matrix structure is not performed. It is up to the user to decide when this function can be used and,
 *  if needed, perform required validations.
 * 
 *  More information: https://en.wikipedia.org/wiki/Gaussian_elimination
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T>
Matrix<T> inv_posdef(const Matrix<T>& A) {
  auto L = cholinv(A);
  return mult<T,true,false>(L,L);
}

/** \brief Matrix inverse for general square matrix.
 *  
 *  Calculates an inverse of square matrix using matrix.
 * 
 *  This function provides more optimal performance than \ref Mtx::inv() for upper triangular matrices. However, validation of 
 *  input matrix structure is not performed. It is up to the user to decide when this function can be used and, 
 *  if needed, perform required validations.
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T>
Matrix<T> inv_square(const Matrix<T>& A) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  // LU decomposition with pivoting
  auto LU = lup(A);
  auto IL = inv_tril(LU.L);
  auto IU = inv_triu(LU.U);

  return permute_rows(IU * IL, LU.P);
}

/** \brief Matrix inverse (universal).
 *  
 *  Calculates an inverse of a square matrix. If the size of the matrix is smaller than 4, inverse is calculated using 
 *  hard-coded formulas. For matrix sizes equal to 4 and more, determinant is calculated recursively using Gauss-Jordan 
 *  elimination.
 * 
 *  If the inverse doesn't exists, e.g., because the input matrix was singular, an empty matrix is returned.
 * 
 *  More information: https://en.wikipedia.org/wiki/Gaussian_elimination
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T>
Matrix<T> inv(const Matrix<T>& A) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  if (A.numel() == 0) {
    return Matrix<T>();
  } else if (A.rows() < 4) {
    T d = det(A);

    if (d == static_cast<T>(0.0)) throw singular_matrix_exception("Singular matrix in inv");

    Matrix<T> IA(A.rows(), A.rows());
    T invdet = static_cast<T>(1.0) / d;

    if (A.rows() == 1) {
      IA(0,0) = invdet;
    } else if (A.rows() == 2) {
      IA(0,0) =   A(1,1) * invdet;
      IA(0,1) = - A(0,1) * invdet;
      IA(1,0) = - A(1,0) * invdet;
      IA(1,1) =   A(0,0) * invdet;
    } else if (A.rows() == 3) {
      IA(0,0) = (A(1,1)*A(2,2) - A(2,1)*A(1,2)) * invdet;
      IA(0,1) = (A(0,2)*A(2,1) - A(0,1)*A(2,2)) * invdet;
      IA(0,2) = (A(0,1)*A(1,2) - A(0,2)*A(1,1)) * invdet;
      IA(1,0) = (A(1,2)*A(2,0) - A(1,0)*A(2,2)) * invdet;
      IA(1,1) = (A(0,0)*A(2,2) - A(0,2)*A(2,0)) * invdet;
      IA(1,2) = (A(1,0)*A(0,2) - A(0,0)*A(1,2)) * invdet;
      IA(2,0) = (A(1,0)*A(2,1) - A(2,0)*A(1,1)) * invdet;
      IA(2,1) = (A(2,0)*A(0,1) - A(0,0)*A(2,1)) * invdet;
      IA(2,2) = (A(0,0)*A(1,1) - A(1,0)*A(0,1)) * invdet;
    }

    return IA;
  } else {
    return inv_square(A);
  }
}

/** \brief Moore-Penrose pseudo-inverse.
 *  
 *  Calculates the Moore-Penrose pseudo-inverse \f$A^+\f$ of a matrix \f$A\f$.
 * 
 *  If \f$A\f$ has linearly independent columns, the pseudo-inverse is a left inverse, that is 
 *  \f$A^+ A = I\f$, and \f$A^+ = (A' A)^{-1} A'\f$.
 *  If \f$A\f$ has linearly independent rows, the pseudo-inverse is a right inverse, that is 
 *  \f$A A^+ = I\f$, and \f$A^+ = A' (A A')^{-1}\f$.
 * 
 *  More information: https://en.wikipedia.org/wiki/Moore%E2%80%93Penrose_inverse
 */
template<typename T>
Matrix<T> pinv(const Matrix<T>& A) {
  if (A.rows() > A.cols()) {
    auto AH_A = mult<T,true,false>(A, A);
    auto Linv = inv_posdef(AH_A);
    return mult<T,false,true>(Linv, A);
  } else {
    auto AA_H = mult<T,false,true>(A, A);
    auto Linv = inv_posdef(AA_H);
    return mult<T,true,false>(A, Linv);
  }
}

/** \brief Matrix trace.
 *  
 *  Calculates trace of a matrix by summing the elements on the diagonal:
 * 
 *  \f$ \textrm{tr})(A) = \displaystyle \sum_{n = 0}^{N-1} [A]_{n,n}\f$
 */
template<typename T>
T trace(const Matrix<T>& A) {
  T t = static_cast<T>(0);
  for (int i = 0; i < A.rows(); i++)
    t += A(i,i);
  return t;
}

/** \brief Condition number of a matrix.
 *  
 *  Calculates condition number of a matrix. The condition number of a matrix measures sensitivity of a  
 *  solution for system of linear equations to errors in the input data. 
 *  The condition number is calculated by:
 * 
 *  \f$ \textrm{cond} = \textrm{norm}(A) * \textrm{norm}(A^{-1})\f$
 * 
 *  Frobenius norm is used for the sake of calculations. See \ref Mtx::norm_fro().
 */
template<typename T>
double cond(const Matrix<T>& A) {
  try {
    auto A_inv = inv(A);
    return norm_fro(A) * norm_fro(A_inv);
  } catch (singular_matrix_exception& e) {
    return std::numeric_limits<double>::max();
  }
}

/** \brief Cholesky decomposition.
 *
 *  The Cholesky decomposition of a Hermitian positive-definite matrix \f$A\f$ is a decomposition of the 
 *  form \f$ A = LL^H \f$, where \f$L\f$ is a lower triangular matrix with real and positive diagonal entries, 
 *  and \f$^H\f$ denotes the conjugate transpose.
 * 
 *  Alternatively, the decomposition can be computed as \f$ A = U^H U \f$ with \f$U\f$ being upper-triangular matrix. 
 *  Selection between lower and upper triangular factor can be done via template parameter.
 * 
 *  Input matrix must be square and Hermitian. If the matrix is not Hermitian positive-definite or is ill-conditioned, 
 *  the result may be unreliable. Only the lower-triangular or upper-triangular and diagonal elements of the input 
 *  matrix are used for calculations. No checking is performed to verify if the input matrix is Hermitian.
 * 
 *  More information: https://en.wikipedia.org/wiki/Cholesky_decomposition
 * 
 *  \tparam is_upper if set to true, the result is provided for upper-triangular factor \f$U\f$. If set to false, 
 *  the result is provided for lower-triangular factor \f$L\f$ .
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T, bool is_upper = false>
Matrix<T> chol(const Matrix<T>& A) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  const unsigned N = A.rows(); 

  // Calculate lower or upper triangular, depending on template parameter.
  // Calculation is the same - the difference is in transposed row and column indexing.
  Matrix<T> C = is_upper ? triu(A) : tril(A);

  for (unsigned j = 0; j < N; j++) {
    if (C(j,j) == static_cast<T>(0.0)) throw singular_matrix_exception("Singular matrix in chol");

    C(j,j) = std::sqrt(C(j,j));

    for (unsigned k = j+1; k < N; k++)
      if (is_upper)
        C(j,k) /= C(j,j);
      else
        C(k,j) /= C(j,j);

    for (unsigned k = j+1; k < N; k++)
      for (unsigned i = k; i < N; i++)
        if (is_upper)
          C(k,i) -= C(j,i) * Util::cconj(C(j,k));
        else
          C(i,k) -= C(i,j) * Util::cconj(C(k,j));
  }

  return C;
}

/** \brief Inverse of Cholesky decomposition.
 *
 *  This function directly calculates the inverse of Cholesky decomposition \f$L^{-1}\f$ such that \f$ A = LL^H \f$.
 * 
 *  Input matrix must be square. If the matrix is not Hermitian positive-definite or is ill-conditioned, the result 
 *  may be unreliable.
 * 
 *  See \ref Mtx::chol() for reference on Cholesky decomposition.
 *
 *  More information: https://en.wikipedia.org/wiki/Cholesky_decomposition
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T>
Matrix<T> cholinv(const Matrix<T>& A) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  const unsigned N = A.rows();
  Matrix<T> L(A);
  auto Linv = eye<T>(N);

  for (unsigned j = 0; j < N; j++) {
    if (L(j,j) == static_cast<T>(0.0)) throw singular_matrix_exception("Singular matrix in cholinv");

    L(j,j) = static_cast<T>(1.0) / std::sqrt(L(j,j));

    for (unsigned k = j+1; k < N; k++)
      L(k,j) = L(k,j) * L(j,j);

    for (unsigned k = j+1; k < N; k++)
      for (unsigned i = k; i < N; i++)
        L(i,k) = L(i,k) - L(i,j) * Util::cconj(L(k,j));
  }

  for (unsigned k = 0; k < N; k++) {
    for (unsigned i = k; i < N; i++) {
      Linv(i,k) = Linv(i,k) * L(i,i);
      for (unsigned j = i+1; j < N; j++)
        Linv(j,k) = Linv(j,k) - L(j,i) * Linv(i,k);
    }
  }
 
  return Linv;
}

/** \brief LDL decomposition.
 *
 *  The LDL decomposition of a Hermitian positive-definite matrix \a A, is a decomposition of the form:
 * 
 *  \f$ A = L D L^H \f$
 * 
 *  where \f$L\f$ is a lower unit triangular matrix with ones at the diagonal, \f$L^H\f$ denotes the conjugate 
 *  transpose of \f$L\f$, and \f$D\f$ denotes diagonal matrix.
 * 
 *  Input matrix must be square. If the matrix is not Hermitian positive-definite or is ill-conditioned, the result 
 *  may be unreliable.
 * 
 *  More information: https://en.wikipedia.org/wiki/Cholesky_decomposition#LDL_decomposition
 * 
 *  \param A input positive-definite matrix to be decomposed 
 *  \return structure encapsulating calculated \a L and \a D
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T>
LDL_result<T> ldl(const Matrix<T>& A) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  const unsigned N = A.rows();

  LDL_result<T> res;

  // aliases
  auto& L = res.L;
  auto& d = res.d;

  L = eye<T>(N);
  d.resize(N);

  for (unsigned m = 0; m < N; m++) {
    d[m] = A(m,m);

    for (unsigned k = 0; k < m; k++)
      d[m] -= L(m,k) * Util::cconj(L(m,k)) * d[k];

    if (d[m] == static_cast<T>(0.0)) throw singular_matrix_exception("Singular matrix in ldl");

    for (unsigned n = m+1; n < N; n++) {
      L(n,m) = A(n,m);
      for (unsigned k = 0; k < m; k++)
        L(n,m) -= L(n,k) * Util::cconj(L(m,k)) * d[k];
      L(n,m) /= d[m];
    }
  }

  return res;
}

/** \brief Reduced QR decomposition based on Gram-Schmidt method. 
 *
 *  The QR decomposition is a decomposition of a matrix \f$A\f$ into a product \f$A = QR\f$ of an orthonormal 
 *  matrix \f$Q\f$ and an upper triangular matrix \f$R\f$.
 * 
 *  This function implements the reduced QR decomposition based on Gram-Schmidt method.
 * 
 *  More information: https://en.wikipedia.org/wiki/QR_decomposition
 *  \param A input matrix to be decomposed, size \a n x \a m
 *  \return structure encapsulating calculated \a Q of size \a n x \a m, and \a R of size \a m x \a m.
 * 
 *  \throws singular_matrix_exception when division by 0 is encountered during computation
 */
template<typename T>
QR_result<T> qr_red_gs(const Matrix<T>& A) {
  const int rows = A.rows();
  const int cols = A.cols();

  QR_result<T> res;

  //aliases
  auto& Q = res.Q;
  auto& R = res.R;

  Q = zeros<T>(rows, cols);
  R = zeros<T>(cols, cols);

  for (int c = 0; c < cols; c++) {
    Matrix<T> v = A.get_submatrix(0, rows-1, c, c);
    for (int r = 0; r < c; r++) {
      for (int k = 0; k < rows; k++)
        R(r,c) = R(r,c) + Util::cconj(Q(k,r)) * A(k,c);
      for (int k = 0; k < rows; k++)
        v(k) = v(k) - R(r,c) * Q(k,r);
    }

    R(c,c) = static_cast<T>(norm_fro(v));
    
    if (R(c,c) == static_cast<T>(0.0)) throw singular_matrix_exception("Division by 0 in QR GS");
    
    for (int k = 0; k < rows; k++)
      Q(k,c) = v(k) / R(c,c);
  }

  return res;
}

/** \brief Generate Householder reflection.
 *
 *  Generates Householder reflection for a given vector. The function returns vector \a v normalized to square root of 2.
 * 
 *  \param a column vector of size \a N x \a 1
 *  \return column vector with Householder reflection of \a a
 */
template<typename T>
Matrix<T> householder_reflection(const Matrix<T>& a) {
  if (a.cols() != 1) throw std::runtime_error("Input not a column vector");

  static const T ISQRT2 = static_cast<T>(0.707106781186547);

  Matrix<T> v(a);
  v(0) += Util::csign(v(0)) * norm_fro(v);
  auto vn = norm_fro(v) * ISQRT2;
  for (unsigned i = 0; i < v.numel(); i++)
    v(i) /= vn;
  return v;
}

/** \brief QR decomposition based on Householder method.
 *
 *  The QR decomposition is a decomposition of a matrix \f$A\f$ into a product \f$ A = QR \f$ of an orthonormal 
 *  matrix \f$Q\f$ and an upper triangular matrix \f$R\f$.
 * 
 *  This function implements QR decomposition based on Householder reflections method.
 * 
 *  More information: https://en.wikipedia.org/wiki/QR_decomposition
 * 
 *  \param A input matrix to be decomposed, size \a n x \a m
 *  \param calculate_Q indicates if \a Q to be calculated
 *  \return structure encapsulating calculated \a Q of size \a n x \a n and \a R of size \a n x \a m. 
 *          \a Q is calculated only when \a calculate_Q = True.
 */
template<typename T>
QR_result<T> qr_householder(const Matrix<T>& A, bool calculate_Q = true) {
  const unsigned rows = A.rows();
  const unsigned cols = A.cols();

  QR_result<T> res;

  //aliases
  auto& Q = res.Q;
  auto& R = res.R;

  R = Matrix<T>(A);

  if (calculate_Q)
    Q = eye<T>(rows);

  const unsigned N = (rows > cols) ? cols : rows;

  for (unsigned j = 0; j < N; j++) {
    auto v = householder_reflection(R.get_submatrix(j, rows-1, j, j));

    auto R1 = R.get_submatrix(j, rows-1, j, cols-1);
    auto WR = v * mult<T,true,false>(v, R1);
    for (unsigned c = j; c < cols; c++)
      for (unsigned r = j; r < rows; r++)
        R(r,c) -= WR(r-j,c-j);

    if (calculate_Q) {
      auto Q1 = Q.get_submatrix(0, rows-1, j, rows-1);
      auto WQ = mult<T,false,true>(Q1 * v, v);
      for (unsigned c = j; c < rows; c++)
        for (unsigned r = 0; r < rows; r++)
          Q(r,c) -= WQ(r,c-j);
    }
  }

  for (unsigned col = 0; col < R.cols(); col++)
    for (unsigned row = col+1; row < R.rows(); row++)
      R(row,col) = 0;

  return res;
}

/** \brief QR decomposition.
 *
 *  The QR decomposition is a decomposition of a matrix \f$A\f$ into a product \f$ A = QR \f$ of an orthonormal 
 *  matrix \f$Q\f$ and an upper triangular matrix \f$R\f$.
 * 
 *  Currently, this function is a wrapper around \ref Mtx::qr_householder(). Refer to qr_red_gs() for alternative 
 *  implementation.

 *  \param A input matrix to be decomposed 
 *  \param calculate_Q indicates if \a Q to be calculated
 *  \return structure encapsulating calculated \a Q of size \a n x \a n and \a R of size \a n x \a m. 
 *          \a Q is calculated only when \a calculate_Q = True.
 */
template<typename T>
inline QR_result<T> qr(const Matrix<T>& A, bool calculate_Q = true) {
  return qr_householder(A, calculate_Q);
}

/** \brief Hessenberg decomposition.
 *
 *  Finds the Hessenberg decomposition of \f$A=QHQ^*\f$. Hessenberg matrix \f$H\f$ has zero entries below the first subdiagonal.
 * 
 *  More information: https://en.wikipedia.org/wiki/Hessenberg_matrix
 * 
 *  \param A input matrix to be decomposed 
 *  \param calculate_Q indicates if \a Q to be calculated
 *  \return structure encapsulating calculated \a H and \a Q. \a Q is calculated only when \a calculate_Q = True.
 * 
 *  \throws std::runtime_error when the input matrix is not square
 */
template<typename T>
Hessenberg_result<T> hessenberg(const Matrix<T>& A, bool calculate_Q = true) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  Hessenberg_result<T> res;

  // aliases
  auto& H = res.H;
  auto& Q = res.Q;

  const unsigned N = A.rows();
  H = Matrix<T>(A);

  if (calculate_Q)
    Q = eye<T>(N);

  for (unsigned k = 1; k < N-1; k++) {
    auto v = householder_reflection(H.get_submatrix(k, N-1, k-1, k-1));

    auto H1 = H.get_submatrix(k, N-1, 0, N-1);
    auto W1 = v * mult<T,true,false>(v, H1);
    for (unsigned c = 0; c < N; c++)
      for (unsigned r = k; r < N; r++)
        H(r,c) -= W1(r-k,c);

    auto H2 = H.get_submatrix(0, N-1, k, N-1);
    auto W2 = mult<T,false,true>(H2 * v, v);
    for (unsigned c = k; c < N; c++)
      for (unsigned r = 0; r < N; r++)
        H(r,c) -= W2(r,c-k);

    if (calculate_Q) {
      auto Q1 = Q.get_submatrix(0, N-1, k, N-1);
      auto W3 = mult<T,false,true>(Q1 * v, v);
      for (unsigned c = k; c < N; c++)
        for (unsigned r = 0; r < N; r++)
          Q(r,c) -= W3(r,c-k);
    }
  }

  for (unsigned row = 2; row < N; row++)
    for (unsigned col = 0; col < row-2; col++)
      H(row,col) = static_cast<T>(0);

  return res;
}

/** \brief Wilkinson's shift for complex eigenvalues.
 *
 *  Computes Wilkinson's shift value \a mu for complex eigenvalues of input matrix. Wilkinson's shift is calculated 
 *  as eigenvalue of the bottom 2 x 2 principal minor closest to the corner entry of the matrix.
 * 
 *  Input must be a square matrix in Hessenberg form.
 * 
 *  \throws std::runtime_error when the input matrix is not square
 */
template<typename T>
std::complex<T> wilkinson_shift(const Matrix<std::complex<T>>& H, T tol = 1e-10) {
  if (! H.issquare()) throw std::runtime_error("Input matrix is not square");

  const unsigned n = H.rows();
  std::complex<T> mu;

  if (std::abs(H(n-1,n-2)) < tol) {
    mu = H(n-2,n-2);
  } else {
    auto trA = H(n-2,n-2) + H(n-1,n-1);
    auto detA = H(n-2,n-2) * H(n-1,n-1) - H(n-2, n-1) * H(n-1, n-2);
    mu = (trA + std::sqrt(trA*trA - 4.0*detA)) / 2.0;
  }

  return mu;
}

/** \brief Matrix eigenvalues of complex matrix. 
 *
 *  Computes eigenvalues of input square matrix using the QR method with shifts.
 * 
 *  \param A input complex matrix to be decomposed 
 *  \param tol numerical precision tolerance for stop condition 
 *  \param max_iter maximum number of iterations
 *  \return structure containing the result and status of eigenvalue calculation
 * 
 *  \throws std::runtime_error when the input matrix is not square
 */
template<typename T>
Eigenvalues_result<T> eigenvalues(const Matrix<std::complex<T>>& A, T tol = 1e-12, unsigned max_iter = 100) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");

  const unsigned N = A.rows();
  Matrix<std::complex<T>> H;
  bool success = false;

  QR_result<std::complex<T>> QR;

  // aliases
  auto& Q = QR.Q;
  auto& R = QR.R;

  // Transfer A to Hessenberg form to improve convergence (skip calculation of Q)
  H = hessenberg(A, false).H;

  for (unsigned iter = 0; iter < max_iter; iter++) {
    auto mu = wilkinson_shift(H, tol);

    // subtract mu from diagonal
    for (unsigned n = 0; n < N; n++) 
      H(n,n) -= mu;

    // QR factorization with shifted H    
    QR = qr(H);
    H = R * Q;
    
    // add back mu to diagonal
    for (unsigned n = 0; n < N; n++) 
      H(n,n) += mu;

    // Check for convergence
    if (std::abs(H(N-2,N-1)) <= tol) {
      success = true;
      break;
    }
  }

  Eigenvalues_result<T> res;
  res.eig = diag(H);
  res.err = std::abs(H(N-2,N-1));
  res.converged = success;

  return res;
}

/** \brief Matrix eigenvalues of real matrix. 
 *
 *  Computes eigenvalues of input square matrix using the QR method with shifts.
 * 
 *  \param A input real matrix to be decomposed 
 *  \param tol numerical precision tolerance for stop condition 
 *  \param max_iter maximum number of iterations
 *  \return structure containing the result and status of eigenvalue calculation
 */
template<typename T>
Eigenvalues_result<T> eigenvalues(const Matrix<T>& A, T tol = 1e-12, unsigned max_iter = 100) {
  auto A_cplx = make_complex(A);
  return eigenvalues(A_cplx, tol, max_iter);
}

/** \brief Solves the upper triangular system.
 *
 *  Return the matrix left division of \a U and \a B, where \a U is square and upper triangular. It is equivalent 
 *  to solving the system \f$ U \cdot X = B \f$ with respect to \f$ X \f$. The system is solved for each column of 
 *  \a B using backwards substitution.
 * 
 *  A minimum norm solution is computed if the coefficient matrix is singular.
 *
 *  \param U left side matrix of size \a N x \a N. Must be square and upper triangular
 *  \param B right hand side matrix of size \a N x \a M.
 *  \return solution matrix of size \a N x \a M.
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws std::runtime_error when number of rows is not equal between input matrices
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T>
Matrix<T> solve_triu(const Matrix<T>& U, const Matrix<T>& B) {
  if (! U.issquare()) throw std::runtime_error("Input matrix is not square");
  if (U.rows() != B.rows()) throw std::runtime_error("Unmatching matrix dimensions for solve");

  const unsigned N = U.rows();  
  const unsigned M = B.cols();

  if (U.numel() == 0)
    return Matrix<T>();

  Matrix<T> X(B);

  for (unsigned m = 0; m < M; m++) {
    // backwards substitution for each column of B
    for (int n = N-1; n >= 0; n--) {
      for (unsigned j = n + 1; j < N; j++)
        X(n,m) -= U(n,j) * X(j,m);

      if (U(n,n) == static_cast<T>(0.0)) throw singular_matrix_exception("Singular matrix in solve_triu");

      X(n,m) /= U(n,n);
    }
  }

  return X;
}

/** \brief Solves the lower triangular system.
 *
 *  Return the matrix left division of \a L and \a B, where \a L is square and lower triangular. It is equivalent 
 *  to solving the system \f$ L \cdot X = B \f$ with respect to \f$ X \f$. The system is solved for each column 
 *  of \a B using forwards substitution.
 * 
 *  A minimum norm solution is computed if the coefficient matrix is singular.
 *
 *  \param L left side matrix of size \a N x \a N. Must be square and lower triangular
 *  \param B right hand side matrix of size \a N x \a M.
 *  \return X solution matrix of size \a N x \a M.
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws std::runtime_error when number of rows is not equal between input matrices
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T>
Matrix<T> solve_tril(const Matrix<T>& L, const Matrix<T>& B) {
  if (! L.issquare()) throw std::runtime_error("Input matrix is not square");
  if (L.rows() != B.rows()) throw std::runtime_error("Unmatching matrix dimensions for solve");

  const unsigned N = L.rows();  
  const unsigned M = B.cols();  

  if (L.numel() == 0)
    return Matrix<T>();

  Matrix<T> X(B);

  for (unsigned m = 0; m < M; m++) {
    // forwards substitution for each column of B
    for (unsigned n = 0; n < N; n++) {
      for (unsigned j = 0; j < n; j++)
        X(n,m) -= L(n,j) * X(j,m);
      
      if (L(n,n) == static_cast<T>(0.0)) throw singular_matrix_exception("Singular matrix in solve_tril");
      
      X(n,m) /= L(n,n);
    }
  }

  return X;
}

/** \brief Solves the square system.
 *
 *  Return the matrix left division of \a A and \a B, where \a A is square. It is equivalent to solving the system 
 *  \f$A \cdot X = B\f$ with respect to \f$X\f$. The system is solved for each column of \a B using LU decomposition 
 *  followed by forward and backward propagation.
 * 
 *  A minimum norm solution is computed if the coefficient matrix is singular.
 *
 *  \param A left side matrix of size \a N x \a N. Must be square.
 *  \param B right hand side matrix of size \a N x \a M.
 *  \return solution matrix of size \a N x \a M.
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws std::runtime_error when number of rows is not equal between input matrices
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T>
Matrix<T> solve_square(const Matrix<T>& A, const Matrix<T>& B) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");
  if (A.rows() != B.rows()) throw std::runtime_error("Unmatching matrix dimensions for solve");

  if (A.numel() == 0)
    return Matrix<T>();

  Matrix<T> L;
  Matrix<T> U;
  std::vector<unsigned> P;

  // LU decomposition with pivoting
  auto lup_res = lup(A);

  auto y = solve_tril(lup_res.L, B);
  auto x = solve_triu(lup_res.U, y);

  return permute_rows(x, lup_res.P);
}

/** \brief Solves the positive definite (Hermitian) system.
 *
 *  Return the matrix left division of \a A and \a B, where \a A is positive definite matrix. It is equivalent to solving 
 *  the system \f$A \cdot X = B\f$ with respect to \f$X\f$. The system is solved for each column of \a B using Cholesky 
 *  decomposition followed by forward and backward propagation.
 * 
 *  A minimum norm solution is computed if the coefficient matrix is singular.
 *
 *  \param A left side matrix of size \a N x \a N. Must be square and positive definite.
 *  \param B right hand side matrix of size \a N x \a M.
 *  \return solution matrix of size \a N x \a M.
 * 
 *  \throws std::runtime_error when the input matrix is not square
 *  \throws std::runtime_error when number of rows is not equal between input matrices
 *  \throws singular_matrix_exception when the input matrix is singular (detected as division by 0 during computation)
 */
template<typename T>
Matrix<T> solve_posdef(const Matrix<T>& A, const Matrix<T>& B) {
  if (! A.issquare()) throw std::runtime_error("Input matrix is not square");
  if (A.rows() != B.rows()) throw std::runtime_error("Unmatching matrix dimensions for solve");

  if (A.numel() == 0)
    return Matrix<T>();

  // LU decomposition with pivoting
  auto L = chol(A);

  auto Y = solve_tril(L, B);
  return solve_triu(L.ctranspose(), Y);
}

/** \class Matrix 
 *   Matrix class definition.
 *  
 */ 
template<typename T>
class Matrix {
  public:
    /** \brief Default constructor.
     *  
     *  Constructs an empty matrix with zero capacity, taking \a rows = 0 and \a cols = 0.
     */
    Matrix();

    /** \brief Square matrix constructor.
     *  
     *  Constructs a square matrix of size \a size x \a size. The content of the matrix is left uninitialized.
     */
    Matrix(unsigned size);

    /** \brief Rectangular matrix constructor.
     *  
     *  Constructs a matrix of size \a nrows x \a ncols. The content of the matrix is left uninitialized.
     */
    Matrix(unsigned nrows, unsigned ncols);

    /** \brief Rectangular matrix constructor with fill.
     *  
     *  Constructs a matrix of size \a nrows x \a ncols. All of the matrix elements of are set to value \a x.
     */
    Matrix(T x, unsigned nrows, unsigned ncols);

    /** \brief Rectangular matrix constructor with initialization.
     *  
     *  Constructs a matrix of size \a nrows x \a ncols. The elements of the matrix are initialized using the elements 
     *  stored in the input \a array. 
     *  The elements of the matrix are filled in a column-major order.
     */
    Matrix(const T* array, unsigned nrows, unsigned ncols);

    /** \brief Rectangular matrix constructor with initialization.
     *  
     *  Constructs a matrix of size \a nrows x \a ncols. The elements of the matrix are initialized using the elements 
     *  stored in the input std::vector. 
     *  Size of the vector must be equal to the number of matrix elements given by \a nrows and \a ncols.
     * 
     *  The elements of the matrix are filled in a column-major order.
     *
     *  \throws std::runtime_error when the size of initialization vector is not consistent with matrix dimensions
     */
    Matrix(const std::vector<T>& vec, unsigned nrows, unsigned ncols);

    /** \brief Rectangular matrix constructor with initialization.
     *  
     *  Constructs a matrix of size \a nrows x \a ncols. The elements of the matrix are initialized using the elements 
     *  stored in the input std::initializer_list. 
     *  Number of elements in the list must be equal to the number of matrix elements given by \a nrows and \a ncols. 
     * 
     *  The elements of the matrix are filled in a column-major order.
     *
     *  \throws std::runtime_error when the size of initialization list is not consistent with matrix dimensions
     */
    Matrix(std::initializer_list<T> init_list, unsigned nrows, unsigned ncols);

    /** Copy constructor.
     */
    Matrix(const Matrix &);

    /** Destructor.
     */
    virtual ~Matrix();

    /** \brief Extract a submatrix.
     *  
     *  Constructs a submatrix using the specified range of row and column indices. The submatrix contains a copy of elements 
     *  placed between row indices indicated by \a row_first and \a row_last, and column indices \a col_first and \a col_last. 
     *  Both index ranges are inclusive.
     *
     *  \throws std::out_of_range when row or column index is out of range of matrix dimensions
     */
    Matrix<T> get_submatrix(unsigned row_first, unsigned row_last, unsigned col_first, unsigned col_last) const;

    /** \brief Embed a submatrix.
     *  
     *  Embed elements of the input submatrix at the specified range of row and column indices. The elements of input submatrix 
     *  are placed starting at row index incated by \a row_first and column indices \a col_first.
     * 
     *  \throws std::out_of_range when row or column index is out of range of matrix dimensions
     *  \throws std::runtime_error when input matrix is empty (i.e., it has zero elements)
     */
    void set_submatrix(const Matrix<T>& smtx, unsigned row_first, unsigned col_first);

    /** \brief Clears the matrix.
     *  
     *  De-allocates the memory reserved for matrix storage and sets the matrix size to 0.
     */
    void clear();

    /** \brief Matrix dimension reshape.
     *  
     *  Modifies the first and the second dimension of the matrix according to the input parameters.
     *  A number of elements in the reshaped matrix must be the preserved and not changed comparing to the state before the reshape.
     *
     *  \throws std::runtime_error when reshape attempts to change the number of elements
     */
    void reshape(unsigned rows, unsigned cols);

    /** \brief Resize the matrix.
     *  
     *  Clears the content of the matrix and changes it dimensions to be equal to the specified number of rows and columns.
     * 
     *  Remark that the content of the matrix is lost after calling the reshape method.
     */
    void resize(unsigned rows, unsigned cols);

    /** \brief Element exist check.
     *  
     *  Returns true if the element with specified coordinates exists within the matrix dimension range.
     * 
     *  For example, calling \a exist(4,0) on a matrix with dimensions \a 2 x \a 2 shall yield false.
     */
    bool exists(unsigned row, unsigned col) const;

    /** \brief Memory pointer.
     *  
     *  Returns a pointer to the selected element in the array used internally by the matrix. The matrix memory is arranged 
     *  in a column-major order.
     */
    T* ptr(unsigned row, unsigned col);

    /** \brief Memory pointer.
     *  
     *  Returns a pointer to the first element in the array used internally by the matrix. The matrix memory is arranged 
     *  in a column-major order.
     *
     *  \throws std::out_of_range when row or column index is out of range
     */
    T* ptr();

    /** Fill with a scalar.
     *  Set all the elements of the matrix to a specified value.
     */
    void fill(T value);

    /** \brief Fill column with a scalar.
     *  
     *  Set all the elements in a specified column of the matrix to a specified value.
     * 
     *  \throws std::out_of_range when column index is out of range
     */
    void fill_col(T value, unsigned col);

    /** \brief Fill row with a scalar.
     *  
     *  Set all the elements in a specified row of the matrix to a specified value.
     *  
     *  \throws std::out_of_range when row index is out of range
     */
    void fill_row(T value, unsigned row);

    /** \brief Emptiness check.
     *  
     *  Check if the matrix is empty, i.e., if both dimensions are equal zero and the matrix stores no elements.
     */
    bool isempty() const;

    /** \brief Squareness check.
     *  Check if the matrix is square, i.e., the width of the first and the second dimensions are equal.
     */
    bool issquare() const;

    /** \brief Matrix equality check.
     *  
     *  Returns true, if both matrices are the same size and all of the element are equal value.
     */
    bool isequal(const Matrix<T>&) const;

    /** \brief Matrix equality check with tolerance.
     *  
     *  Returns true, if both matrices are the same size and all of the element are equal in value under assumed tolerance.
     *  The tolerance check is performed for each element: \f$tol < |A_{i,j} - B_{i,j}|\f$.
     */
    bool isequal(const Matrix<T>&, T) const;

    /** \brief Matrix capacity.
     *  
     *  Returns the number of the elements stored within the matrix, i.e., a product of both dimensions.
     */
    unsigned numel() const;

    /** \brief Number of rows.
     *  
     *  Returns the number of rows of the matrix, i.e., the size of the first dimension.
     */
    unsigned rows() const;

    /** \brief Number of columns.
     *  
     *  Returns the number of columns of the matrix, i.e., the size of the second dimension.
     */
    unsigned cols() const;

    /** \brief Matrix shape.
     *  
     *  Returns std::pair with the \a first element providing the number of rows and the \a second element providing 
     *  the number of columns.
     */
    std::pair<unsigned,unsigned> shape() const;

    /** \brief Transpose a matrix.
     *  
     *  Returns a matrix that is a transposition of an input matrix.
     */
    Matrix<T> transpose() const;

    /** \brief Transpose a complex matrix.
     *  
     *  Returns a matrix that is a conjugate (Hermitian) transposition of an input matrix. Conjugate transpose 
     *  applies a conjugate operation to all elements in addition to matrix transposition.
     */
    Matrix<T> ctranspose() const;
    
    /** \brief Matrix sum (in-place).
     *  
     *  Calculates a sum of two matrices \f$A + B\f$. \f$A\f$ and \f$B\f$ must be the same size. 
     *  Operation is performed in-place by modifying elements of the matrix.
     *
     *  \throws std::runtime_error when matrix dimensions do not match
     */
    Matrix<T>& add(const Matrix<T>&);

    /** \brief Matrix subtraction (in-place).
     *  
     *  Calculates a subtraction of two matrices \f$A - B\f$. \f$A\f$ and \f$B\f$ must be the same size.
     * 
     *  Operation is performed in-place by modifying elements of the matrix.
     *
     *  \throws std::runtime_error when matrix dimensions do not match
     */
    Matrix<T>& subtract(const Matrix<T>&);

    /** \brief Matrix Hadamard product (in-place).
     *  
     *  Calculates a Hadamard product of two matrices \f$A \otimes B\f$. \f$A\f$ and \f$B\f$ must be the same size. 
     *  Hadamard product is calculated as an element-wise multiplication between the matrices.
     * 
     *  Operation is performed in-place by modifying elements of the matrix.
     *
     *  \throws std::runtime_error when matrix dimensions do not match
     */
    Matrix<T>& mult_hadamard(const Matrix<T>&);

    /** \brief Matrix sum with scalar (in-place).
     *  
     *  Adds a scalar \a s to each element of the matrix.
     * 
     *  Operation is performed in-place by modifying elements of the matrix.
     */
    Matrix<T>& add(T);

    /** \brief Matrix subtraction with scalar (in-place).
     *  
     *  Subtracts a scalar \f$s\f$ from each element of the matrix.
     * 
     *  Operation is performed in-place by modifying elements of the matrix.
     */
    Matrix<T>& subtract(T);

    /** \brief Matrix product with scalar (in-place).
     *  
     *  Multiplies each element of the matrix by a scalar \a s.
     * 
     *  Operation is performed in-place by modifying elements of the matrix.
     */
    Matrix<T>& mult(T);

    /** \brief Matrix division by scalar (in-place).
     *  
     *  Divides each element of the matrix by a scalar \a s.
     * 
     *  Operation is performed in-place by modifying elements of the matrix.
     */
    Matrix<T>& div(T);

    /** \brief Matrix assignment.
     *  
     *  Performs deep-copy of the matrix.
     */
    Matrix<T>& operator=(const Matrix<T>&);

    /** \brief Matrix fill operator.
     *  
     *  Assigns value of each element in the matrix to a given scalar. This method does not affect the shape and 
     *  capacity of the matrix. 
     */
    Matrix<T>& operator=(T);

    /** \brief Vector cast operator.
     *  
     *  Converts the matrix to a vector with \a nrows x \a ncols elements. Element order in the vector follow 
     *  column-major format.
     */
    explicit operator std::vector<T>() const;
    std::vector<T> to_vector() const;

    /** \brief Element access operator (1D)
     *  
     *  Access specific matrix element using singular index of the element. Follows column-major convention. 
     *
     *  \throws std::out_of_range when element index is out of range
     *          Thrown only when MATRIX_STRICT_BOUNDS_CHECK is enabled during compilation.
     */
    T& operator()(unsigned nel);
    T  operator()(unsigned nel) const;
    T& at(unsigned nel);
    T  at(unsigned nel) const;

    /** \brief Element access operator (2D)
     *  
     *  Access specific matrix element using row and column index of the element. 
     * 
     *  \throws std::out_of_range when row or column index is out of range of matrix dimensions.
     *          Thrown only when MATRIX_STRICT_BOUNDS_CHECK is enabled during compilation.
     */
    T& operator()(unsigned row, unsigned col);
    T  operator()(unsigned row, unsigned col) const;
    T& at(unsigned row, unsigned col);
    T  at(unsigned row, unsigned col) const;
 
    /** \brief Row addition.
     *  
     *  Adds values of elements in row \a from to the elements of row \a to.
     *  The elements in row \a from are unchanged.
     *
     *  \throws std::out_of_range when row index is out of range
     */
    void add_row_to_another(unsigned to, unsigned from);

    /** \brief Column addition.
     *  
     *  Adds values of elements in column \a from to the elements of column \a to.
     *  The elements in column \a from are unchanged.
     *
     *  \throws std::out_of_range when column index is out of range
     */
    void add_col_to_another(unsigned to, unsigned from);

    /** \brief Row multiplication.
     *  
     *  Multiply values of each element in row \a to by the elements of row \a from.
     *  The elements in row \a from are unchanged.
     *
     *  \throws std::out_of_range when row index is out of range
     */
    void mult_row_by_another(unsigned to, unsigned from);

    /** \brief Column multiplication.
     *  
     *  Multiply values of each element in column \a to by the elements of column \a from.
     *  The elements in column \a from are unchanged.
     *
     *  \throws std::out_of_range when column index is out of range
     */
    void mult_col_by_another(unsigned to, unsigned from);

    /** \brief Row swap.
     *  
     *  Swaps element values of two columns.
     * 
     *  \throws std::out_of_range when row index is out of range
     */
    void swap_rows(unsigned i, unsigned j);

    /** \brief Column swap.
     *  
     *  Swaps element values between two columns.
     * 
     *  \throws std::out_of_range when column index is out of range
     */
    void swap_cols(unsigned i, unsigned j);

    /** \brief Column to vector
     *  
     *  Stores elements from column \a col to a std::vector.
     * 
     *  \throws std::out_of_range when column index is out of range
     */
    std::vector<T> col_to_vector(unsigned col) const;

    /** \brief Row to vector
     *  
     *  Stores elements from row \a row to a std::vector.
     * 
     *  \throws std::out_of_range when row index is out of range
     */
    std::vector<T> row_to_vector(unsigned row) const;

    /** \brief Column from vector.
     *  
     *  Assigns values of elements of a column \a col to the values stored in the input vector. Size of the vector 
     *  must be equal to the number of rows of the matrix.
     *
     *  \throws std::runtime_error when std::vector size is not equal to number of rows
     *  \throws std::out_of_range when column index out of range
     */
    void col_from_vector(const std::vector<T>&, unsigned col);

    /** \brief Row from vector.
     *  
     *  Assigns values of elements of a row \a col to the values stored in the input vector. Size of the vector 
     *  must be equal to the number of columns of the matrix.
     *
     *  \throws std::runtime_error when std::vector size is not equal to number of columns
     *  \throws std::out_of_range when row index out of range
     */
    void row_from_vector(const std::vector<T>&, unsigned row);

  private:
    unsigned nrows;
    unsigned ncols;
    std::vector<T> data;
};

/*
 * Implementation of Matrix class methods 
 */ 

template<typename T>
Matrix<T>::Matrix() : nrows(0), ncols(0), data() { }

template<typename T>
Matrix<T>::Matrix(unsigned size) : Matrix(size, size) { }

template<typename T>
Matrix<T>::Matrix(unsigned rows, unsigned cols) : nrows(rows), ncols(cols) {
  data.resize(numel());
}

template<typename T>
Matrix<T>::Matrix(T x, unsigned rows, unsigned cols) : Matrix(rows, cols)  {
  fill(x);
}

template<typename T>
Matrix<T>::Matrix(const T* array, unsigned rows, unsigned cols) : Matrix(rows, cols)  {
  data.assign(array, array + numel());
}

template<typename T>
Matrix<T>::Matrix(const std::vector<T>& vec, unsigned rows, unsigned cols) : Matrix(rows, cols) {
  if (vec.size() != numel()) throw std::runtime_error("Size of initialization vector not consistent with matrix dimensions");

  data.assign(vec.begin(), vec.end());
}

template<typename T>
Matrix<T>::Matrix(std::initializer_list<T> init_list, unsigned rows, unsigned cols) : Matrix(rows, cols) { 
  if (init_list.size() != numel()) throw std::runtime_error("Size of initialization list not consistent with matrix dimensions");

  auto it = init_list.begin();

  for (unsigned row = 0; row < this->nrows; row++)
    for (unsigned col = 0; col < this->ncols; col++)
      this->at(row,col) = *(it++);
}

template<typename T>
Matrix<T>::Matrix(const Matrix & other) : Matrix(other.nrows, other.ncols) {
  this->data.assign(other.data.begin(), other.data.end());
}

template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other) {
  this->nrows = other.nrows;
  this->ncols = other.ncols;
  this->data.assign(other.data.begin(), other.data.end());
  return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator=(T s) {
  fill(s);
  return *this;
}

template<typename T>
inline Matrix<T>::operator std::vector<T>() const { 
  return data;
}

template<typename T>
inline void Matrix<T>::clear() {
  this->nrows = 0;
  this->ncols = 0;
  data.resize(0);
}

template<typename T>
void Matrix<T>::reshape(unsigned rows, unsigned cols) {
  if (this->numel() != rows * cols) throw std::runtime_error("Illegal attempt to change number of elements via reshape");

  this->nrows = rows;
  this->ncols = cols;
}

template<typename T>
void Matrix<T>::resize(unsigned rows, unsigned cols) {
  this->nrows = rows;
  this->ncols = cols;
  data.resize(nrows*ncols);
}

template<typename T>
Matrix<T> Matrix<T>::get_submatrix(unsigned row_base, unsigned row_lim, unsigned col_base, unsigned col_lim) const {
  if (row_base > row_lim) throw std::out_of_range("Row index of submatrix out of range");
  if (col_base > col_lim) throw std::out_of_range("Column index of submatrix out of range");
  if (row_lim >= this->rows()) throw std::out_of_range("Row index of submatrix out of range");
  if (col_lim >= this->cols()) throw std::out_of_range("Column index of submatrix out of range");

  unsigned num_rows = row_lim - row_base + 1;
  unsigned num_cols = col_lim - col_base + 1;
  Matrix<T> S(num_rows, num_cols);
  for (unsigned i = 0; i < num_rows; i++) {
    for (unsigned j = 0; j < num_cols; j++) {
      S(i,j) = at(row_base + i, col_base + j);
    }
  }
  return S;
}

template<typename T>
void Matrix<T>::set_submatrix(const Matrix<T>& S, unsigned row_base, unsigned col_base) {
  if (this->isempty()) throw std::runtime_error("Invalid attempt to set submatrix in empty matrix");
  
  const unsigned row_lim = row_base + S.rows() - 1;
  const unsigned col_lim = col_base + S.cols() - 1;
  
  if (row_base > row_lim) throw std::out_of_range("Row index of submatrix out of range");
  if (col_base > col_lim) throw std::out_of_range("Column index of submatrix out of range");
  if (row_lim >= this->rows()) throw std::out_of_range("Row index of submatrix out of range");
  if (col_lim >= this->cols()) throw std::out_of_range("Column index of submatrix out of range");

  unsigned num_rows = row_lim - row_base + 1;
  unsigned num_cols = col_lim - col_base + 1;
  for (unsigned i = 0; i < num_rows; i++)
    for (unsigned j = 0; j < num_cols; j++)
      at(row_base + i, col_base + j) = S(i,j);
}

template<typename T>
inline T & Matrix<T>::operator()(unsigned nel) {
  return at(nel);
}

template<typename T>
inline T & Matrix<T>::operator()(unsigned row, unsigned col) {
  return at(row, col);
}

template<typename T>
inline T Matrix<T>::operator()(unsigned nel) const {
  return at(nel);
}

template<typename T>
inline T Matrix<T>::operator()(unsigned row, unsigned col) const {
  return at(row, col);
}

template<typename T>
inline T & Matrix<T>::at(unsigned nel) {
#ifdef MATRIX_STRICT_BOUNDS_CHECK
  if (!(nel < numel())) throw std::out_of_range("Element index out of range");
#endif

  return data[nel];
}

template<typename T>
inline T & Matrix<T>::at(unsigned row, unsigned col) {
#ifdef MATRIX_STRICT_BOUNDS_CHECK
  if (!(row < rows() && col < cols())) throw std::out_of_range("Element index out of range");
#endif

  return data[nrows * col + row];
}

template<typename T>
inline T Matrix<T>::at(unsigned nel) const {
#ifdef MATRIX_STRICT_BOUNDS_CHECK
  if (!(nel < numel())) throw std::out_of_range("Element index out of range");
#endif

  return data[nel];
}

template<typename T>
inline T Matrix<T>::at(unsigned row, unsigned col) const {
#ifdef MATRIX_STRICT_BOUNDS_CHECK
  if (!(row < rows())) throw std::out_of_range("Row index out of range");
  if (!(col < cols())) throw std::out_of_range("Column index out of range");
#endif

  return data[nrows * col + row];
}

template<typename T>
inline void Matrix<T>::fill(T value) {
  for (unsigned i = 0; i < numel(); i++)
    data[i] = value;
}

template<typename T>
inline void Matrix<T>::fill_col(T value, unsigned col) {
  if (!(col < cols())) throw std::out_of_range("Column index out of range");

  for (unsigned i = col * nrows; i < (col+1) * nrows; i++)
    data[i] = value;
}

template<typename T>
inline void Matrix<T>::fill_row(T value, unsigned row) {
  if (!(row < rows())) throw std::out_of_range("Row index out of range");

  for (unsigned i = 0; i < ncols; i++)
    data[row + i * nrows] = value;
}

template<typename T>
inline bool Matrix<T>::exists(unsigned row, unsigned col) const {
  return (row < nrows && col < ncols);
}

template<typename T>
inline T* Matrix<T>::ptr(unsigned row, unsigned col) {
  if (!(row < rows())) throw std::out_of_range("Row index out of range");
  if (!(col < cols())) throw std::out_of_range("Column index out of range");

  return data.data() + nrows * col + row;
}

template<typename T>
inline T* Matrix<T>::ptr() {
  return data.data();
}

template<typename T>
inline bool Matrix<T>::isempty() const {
  return (nrows == 0) || (ncols == 0);
}

template<typename T>
inline bool Matrix<T>::issquare() const {
  return (nrows == ncols) && !isempty();
}

template<typename T>
bool Matrix<T>::isequal(const Matrix<T>& A) const {
  bool ret = true;
  if (nrows != A.rows() || ncols != A.cols()) {
    ret = false;
  } else {
    for (unsigned i = 0; i < numel(); i++) {
      if (at(i) != A(i)) {
        ret = false;
        break;
      }
    }
  }
  return ret;
}

template<typename T>
bool Matrix<T>::isequal(const Matrix<T>& A, T tol) const {
  bool ret = true;
  if (rows() != A.rows() || cols() != A.cols()) {
    ret = false;
  } else {
    auto abs_tol = std::abs(tol); // workaround for complex
    for (unsigned i = 0; i < A.numel(); i++) {
      if (abs_tol < std::abs(at(i) - A(i))) {
        ret = false;
        break;
      }
    }
  }
  return ret;
}

template<typename T>
inline unsigned Matrix<T>::numel() const { 
  return nrows * ncols; 
}

template<typename T>
inline unsigned Matrix<T>::rows() const { 
  return nrows;
}

template<typename T>
inline unsigned Matrix<T>::cols() const { 
  return ncols; 
}

template<typename T>
inline std::pair<unsigned,unsigned> Matrix<T>::shape() const {
  return std::pair<unsigned,unsigned>(nrows,ncols);
}

template<typename T>
inline Matrix<T> Matrix<T>::transpose() const {
  Matrix<T> res(ncols, nrows);
  for (unsigned c = 0; c < ncols; c++)
    for (unsigned r = 0; r < nrows; r++)
      res(c,r) = at(r,c);
  return res;
}

template<typename T>
inline Matrix<T> Matrix<T>::ctranspose() const {
  Matrix<T> res(ncols, nrows);
  for (unsigned c = 0; c < ncols; c++)
    for (unsigned r = 0; r < nrows; r++)
      res(c,r) = Util::cconj(at(r,c));
  return res;
}

template<typename T>
Matrix<T>& Matrix<T>::add(const Matrix<T>& m) {
  if (!(m.rows() == rows() && m.cols() == cols())) throw std::runtime_error("Unmatching matrix dimensions for iadd");

  for (unsigned i = 0; i < numel(); i++)
    data[i] += m(i);
  return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::subtract(const Matrix<T>& m) {
  if (!(m.rows() == rows() && m.cols() == cols())) throw std::runtime_error("Unmatching matrix dimensions for isubtract");

  for (unsigned i = 0; i < numel(); i++)
    data[i] -= m(i);
  return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::mult_hadamard(const Matrix<T>& m) {
  if (!(m.rows() == rows() && m.cols() == cols())) throw std::runtime_error("Unmatching matrix dimensions for ihprod");

  for (unsigned i = 0; i < numel(); i++)
    data[i] *= m(i);
  return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::add(T s) {
  for (auto& x : data) 
    x += s;
  return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::subtract(T s) {
  for (auto& x : data) 
    x -= s;
  return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::mult(T s) {
  for (auto& x : data) 
    x *= s;
  return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::div(T s) {
  for (auto& x : data) 
    x /= s;
  return *this;
}

template<typename T>
void Matrix<T>::add_row_to_another(unsigned to, unsigned from) {
  if (!(to < rows() && from < rows())) throw std::out_of_range("Row index out of range");

  for (unsigned k = 0; k < cols(); k++)
    at(to, k) += at(from, k);
}

template<typename T>
void Matrix<T>::add_col_to_another(unsigned to, unsigned from) {
  if (!(to < cols() && from < cols())) throw std::out_of_range("Column index out of range");

  for (unsigned k = 0; k < rows(); k++)
    at(k, to) += at(k, from);
}

template<typename T>
void Matrix<T>::mult_row_by_another(unsigned to, unsigned from) {
  if (!(to < rows() && from < rows())) throw std::out_of_range("Row index out of range");

  for (unsigned k = 0; k < cols(); k++)
    at(to, k) *= at(from, k);
}

template<typename T>
void Matrix<T>::mult_col_by_another(unsigned to, unsigned from) {
  if (!(to < cols() && from < cols())) throw std::out_of_range("Column index out of range");

  for (unsigned k = 0; k < rows(); k++)
    at(k, to) *= at(k, from);
}

template<typename T>
void Matrix<T>::swap_rows(unsigned i, unsigned j) {
  if (!(i < rows() && j < rows())) throw std::out_of_range("Row index out of range");

  for (unsigned k = 0; k < cols(); k++)
    std::swap(at(i,k), at(j,k));
}

template<typename T>
void Matrix<T>::swap_cols(unsigned i, unsigned j) {
  if (!(i < cols() && j < cols())) throw std::out_of_range("Column index out of range");

  for (unsigned k = 0; k < rows(); k++)
    std::swap(at(k,i), at(k,j));
}

template<typename T>
inline std::vector<T> Matrix<T>::to_vector() const {
  return data;
}

template<typename T>
inline std::vector<T> Matrix<T>::col_to_vector(unsigned col) const {
  std::vector<T> vec(rows());
  for (unsigned i = 0; i < rows(); i++)
    vec[i] = at(i,col);
  return vec;
}

template<typename T>
inline std::vector<T> Matrix<T>::row_to_vector(unsigned row) const {
  std::vector<T> vec(cols());
  for (unsigned i = 0; i < cols(); i++)
    vec[i] = at(row,i);
  return vec;
}

template<typename T>
inline void Matrix<T>::col_from_vector(const std::vector<T>& vec, unsigned col) {
  if (vec.size() != rows()) throw std::runtime_error("Vector size is not equal to number of rows");
  if (col >= cols()) throw std::out_of_range("Column index out of range");

  for (unsigned i = 0; i < rows(); i++)
    data[col*rows() + i] = vec[i];
}

template<typename T>
inline void Matrix<T>::row_from_vector(const std::vector<T>& vec, unsigned row) {
  if (vec.size() != cols()) throw std::runtime_error("Vector size is not equal to number of columns");
  if (row >= rows()) throw std::out_of_range("Row index out of range");

  for (unsigned i = 0; i < cols(); i++)
    data[row + i*rows()] = vec[i];
}

template<typename T>
Matrix<T>::~Matrix() { }

} // namespace Matrix_hpp

#endif // __MATRIX_HPP__