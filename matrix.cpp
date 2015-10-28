/* sillySPICE - A SPICE-like Circuit Solver
   Copyright (C) 2015 Ville Räisänen <vsr at vsr.name>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <algorithm>
#include <assert.h>

#include "matrix.h"
#include <math.h>
#include <vector>

using namespace std;

Matrix::Matrix(int _rows, int _cols) {
  rows = _rows;
  cols = _cols;
  data = new double[_rows*_cols];

  for (int ind=0;ind<rows*cols;ind++) {
    data[ind] = 0;
  }
}

Matrix::Matrix(int _rows, int _cols, double *_data) {
  rows = _rows;
  cols = _cols;
  data = new double[_rows*_cols];
  copy(_data, _data+_rows*_cols, data);
}

Matrix::Matrix(const Matrix &m) {
  rows = m.rows;
  cols = m.cols;
  data = new double[rows*cols];
  copy(m.data, m.data+rows*cols, data);  
}

Matrix::~Matrix() {
  delete [] data;
}

double
Matrix::value(const int row, const int col) const {
  assert(row*cols + col < rows*cols);
  return data[row*cols + col];
}

void
Matrix::set(int row, int col, double value) {
  assert(row*cols + col < rows*cols);
  data[row*cols + col] = value;
}

void
Matrix::addto(int row, int col, double value) {
  assert(row*cols + col < rows*cols);
  data[row*cols + col] += value;
}


Matrix *
Matrix::transpose() {
  Matrix *mnew = new Matrix(cols, rows);

  for (int ind_col = 0; ind_col < cols; ind_col++) {
    for (int ind_row = 0; ind_row < rows; ind_row++) {
      mnew->data[ind_row + ind_col*rows] = data[ind_col + ind_row*cols];
    }
  }

  return mnew;
}

Matrix *
Matrix::submatrix(const int row1, const int row2, 
		  const int col1, const int col2) {
  assert(row1<=row2);
  assert(col1<=col2);
  assert(row1>=0);
  assert(col1>=0);
  assert(row2<rows);
  assert(col2<cols);

  int num_rows=row2-row1+1, num_cols=col2-col1+1;
  Matrix *subm = new Matrix(num_rows, num_cols);

  for (int ind_row = 0; ind_row < num_rows; ind_row++) {
    for (int ind_col = 0; ind_col < num_cols; ind_col++) {
      int ind1 = ind_row*num_cols + ind_col;
      int ind2 = col1 + ind_col + (row1 + ind_row)*cols;
      subm->data[ind1] = data[ind2];
    }
  }
  return subm;
}

Matrix *
Matrix::submatrix(vector <int> subrows, vector <int> subcols) {
    int ncols = subcols.size();
    int nrows = subrows.size();

    int row, col;
    Matrix *mnew = new Matrix(subrows.size(), subcols.size());

    for (int ind_row=0;ind_row < nrows; ind_row++) {
        row = subrows[ind_row];
        assert(row >= 0 && row < rows);
        for (int ind_col=0;ind_col < ncols; ind_col++) {
            col = subcols[ind_col];
            assert(col >= 0 && col < cols);
            mnew->data[ind_col + ind_row*ncols] = data[col + row*cols];
        }
    }
    return mnew;
}

Matrix *
Matrix::add(const Matrix &m2) {
  assert(rows == m2.rows);
  assert(cols == m2.cols);
  Matrix *mnew = new Matrix(rows, cols, data);

  // Ystävyys on vaatimus vain ERITYYPPISTEN luokkien välillä eikä 
  // yhden luokan instanssien (olioiden) välillä. Täten voimme kirjoittaa:

  for (int ind=0; ind < rows*cols; ind++) {
    mnew->data[ind] += m2.data[ind];
  }

  return mnew;
}

Matrix *
Matrix::subtract(const Matrix &m2) {
  assert(rows == m2.rows);
  assert(cols == m2.cols);
  Matrix *mnew = new Matrix(rows, cols, data);

  for (int ind=0; ind < rows*cols; ind++) {
    mnew->data[ind] -= m2.data[ind];
  }
  return mnew;
}


// Naive matrix multiplication for dense matrices. 
// Note that this has O(n^3) time complexity and is therefore very 
// slow when large matrices are multiplied.
Matrix *
Matrix::mul_right(const Matrix &m2) {
  int new_rows, new_cols;

  assert(cols == m2.rows);

  new_rows = rows;
  new_cols = m2.cols;
  Matrix *mnew = new Matrix(new_rows, new_cols);

  int ind, ind2;
  for (int ind_row = 0; ind_row < new_rows; ind_row++) {
    ind2 = ind_row*cols;
    #ifdef DEBUG
    cout << ind_row << endl;
    #endif

    for (int ind_col = 0; ind_col < new_cols; ind_col++) {
      ind = ind_col + ind_row*new_cols;

      for (int ind_prod = 0; ind_prod < cols; ind_prod++) {
        mnew->data[ind] += data[ind_prod + ind2]
                         * m2.data[ind_prod*m2.cols + ind_col];
      }
    }
  }
  return mnew;
}

Matrix *
Matrix::mul_scalar(const double scalar) {
  Matrix * mnew = new Matrix(rows, cols, data);
  for (int ind=0;ind < rows*cols;ind++) {
    mnew->data[ind] *= scalar;
  }
  return mnew;
}


Matrix *
Matrix::mul_left(const Matrix &m2) {
  int new_rows, new_cols;

  assert(rows == m2.cols);

  new_cols = cols;
  new_rows = m2.rows;

  Matrix *mnew = new Matrix(new_rows, new_cols);

  int ind;
  for (int ind_row = 0; ind_row < new_rows; ind_row++) {
    for (int ind_col = 0; ind_col < new_cols; ind_col++) {
      ind = ind_col + ind_row*new_cols;
      for (int ind_prod = 0; ind_prod < rows; ind_prod++) {
        mnew->data[ind] += m2.data[ind_prod + ind_row*m2.cols]
                         * data[ind_prod*cols + ind_col];
      }
    }
  }
  return mnew;
}

void
Matrix::perm_rows(int *inds_rows) {
  double *newdata = new double[rows*cols];

  for (int ind_row = 0; ind_row < rows; ind_row++) {
    int newrow = inds_rows[ind_row];
    assert(newrow < rows && newrow >= 0);

    for (int ind_col = 0; ind_col < cols; ind_col++) {
      newdata[ind_col + newrow*cols] = data[ind_col + ind_row*cols];
    }
  }
  delete [] data;
  data = newdata;
}

void
Matrix::perm_cols(int *inds_cols) {
  double *newdata = new double[rows*cols];

  for (int ind_col = 0; ind_col < cols; ind_col++) {
    int newcol = inds_cols[ind_col];
    assert (newcol < cols && newcol >= 0);
    for (int ind_row = 0; ind_row < rows; ind_row++) {
      newdata[newcol + ind_row*cols] = data[ind_col + ind_row*cols];
    }
  }
  delete [] data;
  data = newdata;
}

void 
Matrix::swap_rows(int row1, int row2) {
  assert(row1>= 0 && row1 < rows);
  assert(row2>= 0 && row2 < rows);
  assert(row1 != row2);

  for (int ind_col = 0; ind_col < cols; ind_col++) {
    swap(data[ind_col + row1*cols], data[ind_col + row2*cols]);
  }
}

void
Matrix::swap_cols(int col1, int col2) {
  assert(col1>= 0 && col1 < cols);
  assert(col2>= 0 && col2 < cols);
  assert(col1 != col2);

  for (int ind_row = 0; ind_row < rows; ind_row++) {
    swap(data[col1 + ind_row*cols], data[col2 + ind_row*cols]);
  }
}

void 
Matrix::disp() {
  cout << rows << "x" << cols << endl;
  for (int ind_row = 0; ind_row < rows; ind_row++) {
    for (int ind_col = 0; ind_col < cols; ind_col++) {
      cout << data[ind_row*cols + ind_col] << " ";
    }
    cout << endl;
  }
}

ostream&
operator<< (ostream &out, Matrix &mat) {
  out << mat.rows << "x" << mat.cols << endl;
  for (int ind_row = 0; ind_row < mat.rows; ind_row++) {
    for (int ind_col = 0; ind_col < mat.cols; ind_col++) {
      out << mat.data[ind_row*mat.cols + ind_col] << " ";
    }
    out << endl;
  }
  return out;
}

istream&
operator>> (istream &in, Matrix &mat) {
  for (int ind_row = 0; ind_row < mat.rows; ind_row++) {
    for (int ind_col = 0; ind_col < mat.cols; ind_col++) {
      in >> mat.data[ind_row*mat.cols + ind_col];
    }
  }
  return in;
}


Matrix &
Matrix::operator=(const Matrix &arg) {
  rows = arg.rows;
  cols = arg.cols;
  delete [] data;
  data = new double[arg.rows*arg.cols];
  copy(arg.data, arg.data+rows*cols, data);  
  return *this;
}

Matrix & 
Matrix::operator+=(const Matrix &arg) {
  assert(rows == arg.rows);
  assert(cols == arg.cols);

  for (int ind=0; ind<rows*cols; ind++) {
    data[ind] += arg.data[ind];
  }
  return *this;
}

Matrix &
Matrix::operator-=(const Matrix &arg) {
  assert(rows == arg.rows);
  assert(cols == arg.cols);

  for (int ind=0; ind<rows*cols; ind++) {
    data[ind] -= arg.data[ind];
  }
  return *this;
}

double&
Matrix::operator()(const int row, const int col) {
  assert(row >= 0 && row < rows);
  assert(col >= 0 && col < cols);

  return data[col + row*cols];
}

// LU Decomposition of a square matrix with partial pivoting.
// See Demmel - Applied Numerical Linear Algebra. p. 41-42.
//
// Note that this is a solver for dense matrices. Thus, the code
// is very slow when applied to large sparse systems.

void
Matrix::LU(Matrix &L, Matrix &U, Matrix &P) {
  assert(rows == cols);
  assert(rows > 0);

  int *perm = new int[rows];
  Matrix A(*this);

  L = Matrix(rows, rows);
  U = Matrix(rows, rows);

  for (int i = 0; i < rows; i++) {
    perm[i] = i;
  }

  for (int i = 0; i < rows-1; i++) {
    int min     = 0;
    bool minset = false;
    int destrow = 0;

    #ifdef DEBUG
    cout << i << endl;
    #endif 
    // Find element with the maximum absolute value in the column i below row i-1.
    
    for (int j = i; j < rows; j++) {
      int    ind = i + j*cols;
      double val = fabs(A.data[i + j*cols]);

      if (val > min || !minset) {
        destrow = j;
        min = val;
        minset = true;
      }
    }
    assert(minset);
    if (i != destrow) {
      A.swap_rows(i, destrow);
      L.swap_rows(i, destrow);

      int tmp = perm[i];
      perm[i] = perm[destrow];
      perm[destrow] = tmp;
    }
    
    for (int j = i+1; j < rows; j++) {
      L.data[i + j*cols] = A.data[i + j*cols]/ A.data[i + i*cols];
    }
    for (int j = i; j < rows; j++) {
      U.data[j + i*cols] = A.data[j + i*cols];
    }

    double ldata;
    for (int j=i+1; j < rows; j++) {
      ldata = L.data[i+j*rows];
      
      for (int k=i; k < rows; k++) {
        A.data[k + j*rows] -= ldata * U.data[k + i*rows];
      }
    }
    L.data[i*cols + i] = 1;
  }
  
  L.data[rows*cols-1] = 1;
  U.data[rows*cols-1] = A.data[rows*cols-1];

  for (int ind_P=0; ind_P < rows; ind_P++) {
    P.data[cols*ind_P + perm[ind_P]] = 1;
  }

  delete [] perm;
}

double *
Matrix::LU_solve(Matrix &L, Matrix &U, Matrix &P, double *b) {
  int n = L.rows;
  double *x = new double[n];
  double *y = new double[n];

  for (int ind=0; ind < n; ind++) {
    x[ind] = y[ind] = 0;
  }

  double *bperm = new double[n];
  copy(b, b+n, bperm);

  Matrix B(n, 1, bperm);
  Matrix *tmpB = P.mul_right(B);
  copy(tmpB->data, tmpB->data+n, bperm);
  
  delete tmpB;

  // Forward substitution for the lower triangular matrix L.
  double tmpb;
  for (int i = 0; i < n; i++) {
    tmpb = bperm[i];
    for (int j = 0; j < i; j++) {
      tmpb -= L.data[j + i*n]*y[j];
    }
    y[i] = tmpb/L.data[i + i*n];
  }
  delete [] bperm;

  // Backward substitution for the upper triangular matrix U.
  double tmpy; 
  for (int i=0; i < n; i++) {
    int row = n - i - 1;
    tmpy = y[row];
    for (int j=0; j < i;j++) {
      tmpy -= U.data[row*n + n-j-1]*x[n-j-1];
    }
    x[row] = tmpy/U.data[row + row*n];
  }
  delete [] y;
  return x;
}

#ifdef DISP_TEST
int 
main(int argc, char ** argv) {
  // double data[] = {};
  Matrix m(3, 3);

  //cout << "Insert 3x3 matrix: ";
  //cin >> m;
  //cout << m;

  m(1,1) = 1;
  cout << m(1,1);
  cout << m;
  //m << 1,2,3,4,5,6,7,8,9;
}
#endif

#ifdef LU_TEST

int
main(int argc, char **argv) {
  for (int indrep = 0; indrep<1000; indrep++) {
  int ms = 1000;
  double *datahuge = new double[ms*ms];
  double *bhuge = new double[ms];

  for (int ind=0; ind < ms; ind++) {
    bhuge[ind] = rand()%10;
  }
  for (int ind=0; ind < ms*ms; ind++) {
    datahuge[ind] = rand()%10;
  }
  Matrix mhuge = Matrix(ms, ms, datahuge);
  Matrix L = Matrix(ms, ms);
  Matrix U = Matrix(ms, ms);
  Matrix P = Matrix(ms, ms);

  cout << endl << "LU Decomposition" << endl;

  mhuge.LU(L, U, P);

  cout << "Solution by Backward and Forward Substitution" << endl;
  double * xhuge;
  xhuge = mhuge.LU_solve(L, U, P, bhuge);

  Matrix Xhuge = Matrix(ms, 1, xhuge);
  Matrix *Ax = mhuge.mul_right(Xhuge);
  
  double resmax = 0;
  for (int ind=0; ind < ms; ind++) {
    double res = fabs(Ax->value(ind, 0) - bhuge[ind]);
    if (res > resmax) {
      resmax = res;
    }
  }
  cout << "Maximum residual: " << resmax << endl;

  delete Ax;
  delete [] xhuge;
  delete [] datahuge;
  delete [] bhuge;
  }
}

#endif
