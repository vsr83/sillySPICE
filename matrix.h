#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>

using namespace std;

class Matrix {
 private:
  double *data;
 public:
  int rows, cols;

  // Jos funktion argumenttiolio (const Matrix &m1) on const, silloin voin 
  // kutsua vain olion niitä funktioita, jotka on merkitty const. Edelleen 
  // näillä funktioilla ei voi muuttaa luokan muuttujia.

  double   value      (const int row, const int col) const;
  void     set        (const int row, const int col, const double value);
  void     addto      (const int row, const int col, const double value);
  void     disp       ();

  Matrix * transpose  ();
  Matrix * submatrix  (const int row1, const int row2, 
		       const int col1, const int col2);
  Matrix * submatrix  (vector <int> subrows, vector <int> subcols);
  Matrix * add        (const Matrix &m2);
  Matrix * subtract   (const Matrix &m2);
  Matrix * mul_scalar (const double scalar);
  Matrix * mul_right  (const Matrix &m2);
  Matrix * mul_left   (const Matrix &m1);

  void     LU         (Matrix &L, Matrix &U, Matrix &P);
  double  *LU_solve   (Matrix &L, Matrix &U, Matrix &P, double *b); 

  void     perm_rows  (int *inds_rows);
  void     perm_cols  (int *inds_cols);
  void     swap_rows  (int row1, int row2);
  void     swap_cols  (int col1, int col2);

  double&         operator() (const int row, const int col);
  friend ostream& operator<< (ostream &out, Matrix &mat);
  friend istream& operator>> (istream &in, Matrix &mat);

  Matrix(const Matrix &m);
  Matrix(int _rows, int _cols);
  Matrix(int _rows, int _cols, double *_data);
  ~Matrix();

  Matrix & operator= (const Matrix &arg);
  Matrix & operator+=(const Matrix &arg);
  Matrix & operator-=(const Matrix &arg);
};

#endif
