// This is core/vnl/vnl_imag.h
#ifndef vnl_imag_h_
#define vnl_imag_h_
//:
// \file
// \brief Functions to return the imaginary parts of complex arrays, vectors, matrices
//
// \verbatim
// Modifications
// Peter Vanroose - 2 July 2002 - part of vnl_complex_ops.h moved here
// \endverbatim

#include <vcl_complex.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_diag_matrix.h>
#include <vnl/vnl_diag_matrix_fixed.h>
#include <vnl/vnl_sym_matrix.h>

//: Return array I of imaginary parts of complex array C.
template <class T>
void
vnl_imag(vcl_complex<T> const* C, T* I, unsigned int n);

// - vnl_vector
// - vnl_vector_fixed
// - vnl_matrix
// - vnl_matrix_fixed
// - vnl_diag_matrix
// - vnl_diag_matrix_fixed
// - vnl_sym_matrix

//: Vector of imaginary parts of vnl_vector<vcl_complex<T> >.
// \relatesalso vnl_vector
template <class T>
vnl_vector<T>
vnl_imag(vnl_vector<vcl_complex<T> > const& C);

//: Vector of imaginary parts of vnl_vector_fixed<vcl_complex<T>, N >.
// \relatesalso vnl_vector_fixed
template <class T, unsigned int N>
vnl_vector_fixed<T,N>
vnl_imag(vnl_vector_fixed<vcl_complex<T>, N > const& C)
{
  vnl_vector_fixed<T,N> R;
  typename vnl_vector_fixed<vcl_complex<T>,N >::const_iterator cIt = C.begin();
  typename vnl_vector_fixed<T,N>::iterator rIt = R.begin();
  for (; cIt != C.end(); ++cIt, ++rIt)
    *rIt = vcl_imag(*cIt);
  return R;
}

//: Matrix of imaginary parts of vnl_matrix<vcl_complex<T> >.
// \relatesalso vnl_matrix
template <class T>
vnl_matrix<T>
vnl_imag(vnl_matrix<vcl_complex<T> > const& C);

//: Matrix of imaginary parts of vnl_matrix_fixed<vcl_complex<T>,NRow,NCol >.
// \relatesalso vnl_matrix_fixed
template <class T, unsigned int NRow, unsigned int NCol>
vnl_matrix_fixed<T,NRow,NCol>
vnl_imag(vnl_matrix_fixed<vcl_complex<T>,NRow,NCol > const& C)
{
  vnl_matrix_fixed<T,NRow,NCol> R;
  typename vnl_matrix_fixed<vcl_complex<T>,NRow,NCol >::const_iterator cIt = C.begin();
  typename vnl_matrix_fixed<T,NRow,NCol>::iterator rIt = R.begin();
  for (; cIt != C.end(); ++cIt, ++rIt)
    *rIt = vcl_imag(*cIt);
  return R;
}

//: Matrix of imaginary parts of vnl_diag_matrix<vcl_complex<T> >.
// \relatesalso vnl_diag_matrix
template <class T>
vnl_diag_matrix<T>
vnl_imag(vnl_diag_matrix<vcl_complex<T> > const& C);

//: Matrix of imaginary parts of vnl_diag_matrix_fixed<vcl_complex<T> >.
// \relatesalso vnl_diag_matrix_fixed
template <class T, unsigned int N>
vnl_diag_matrix_fixed<T,N>
vnl_imag(vnl_diag_matrix_fixed<vcl_complex<T>,N > const& C)
{
  vnl_diag_matrix_fixed<T,N> R;
  typename vnl_diag_matrix_fixed<vcl_complex<T>,N >::const_iterator cIt = C.begin();
  typename vnl_diag_matrix_fixed<T,N>::iterator rIt = R.begin();
  for (; cIt != C.end(); ++cIt, ++rIt)
    *rIt = vcl_imag(*cIt);
  return R;
}

//: Matrix of imaginary parts of vnl_sym_matrix<vcl_complex<T> >.
// \relatesalso vnl_sym_matrix
template <class T>
vnl_sym_matrix<T>
vnl_imag(vnl_sym_matrix<vcl_complex<T> > const& C);


#endif // vnl_imag_h_
