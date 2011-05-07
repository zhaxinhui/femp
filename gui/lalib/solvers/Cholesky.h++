#ifndef LALIB_SOLVERS_CHOLESKY_HPP
#define LALIB_SOLVERS_CHOLESKY_HPP


#include <cmath>
#include <iostream>	//TODO remove after debugging

#include "SolverReturnCodes.h++"
#include "substitution.h++"

#include "../Matrix.h++"
#include "../output.h++"

namespace lalib
{

/**
Generic cholesky decomposition for dense matrices, works on all matrix types
**/
template<typename scalar, template<typename> class MatrixStoragePolicy, template<typename> class LMatrixStoragePolicy, template<typename> class VectorStoragePolicy>
ReturnCode cholesky(Matrix<scalar, MatrixStoragePolicy> &A, Vector<scalar, VectorStoragePolicy> &x, Vector<scalar, VectorStoragePolicy> &b, Matrix<scalar, LMatrixStoragePolicy> &L)
{
	assert(A.rows() == A.columns());
	assert(A.columns() == b.size());

	Vector<scalar> S;
	S.resize(A.columns());

	// A=LL^t, factor L
	for(size_t j = 0; j < A.columns(); j++)
	{
		for(size_t i = j; i < A.rows(); i++)
		{
			S(i) = A.value(i,j);
			for(size_t k = 0; k < j; k++)
			{
				S(i) -= L.value(i,k)*L.value(j,k);
			}
		}

		L(j,j) = sqrt(S(j));

		for(size_t i = j+1; i < A.rows(); i++)
		{
			L(i,j) = S(i)/L.value(j,j);
		}
	}
	// */


	/*
	// A=LL^t, factor L
	for(size_t j = 0; j < A.columns(); j++)
	{
		L(j,j) = sqrt(A.value(j,j));
		for(size_t i = j+1; i < L.rows(); i++)
		{
			L(i,j) = A.value(i,j)/L.value(j,j);
		}

		for(size_t i = j+1; i < A.rows(); i++)
		{
			for(size_t k = j+1; k < A.columns(); k++)	
			{
				A(i,k) -= L.value(i,j)*L.value(k,j);
			}
		}
	}
	// */

	ReturnCode code;
	// Ly = b
	code = forward_substitution(L,x,b);
	if(code != OK)
		return code;

	//Lx=y
	code = back_substitution(L,x,x);
	if(code != OK)
		return code;


	return OK;
}


/**
Generic cholesky decomposition for dense matrices, works on all matrix types
**/
template<typename scalar, template<typename> class MatrixStoragePolicy, template<typename> class VectorStoragePolicy>
ReturnCode cholesky(Matrix<scalar, MatrixStoragePolicy> &A, Vector<scalar, VectorStoragePolicy> &x, Vector<scalar, VectorStoragePolicy> &b, Matrix<scalar, SparseCRS> &L)
{
	assert(A.rows() == A.columns());
	assert(A.columns() == b.size());

	Vector<scalar> S;
	S.resize(A.columns());

	size_t *p;
	scalar *lp;
	size_t *q;
	scalar *lq;

	// A=LL^t, factor L
	for(size_t j = 0; j < A.columns(); j++)
	{
		for(size_t i = j; i < A.rows(); i++)
		{
			S(i) = A.value(i,j);
			scalar test = S(i);
			/*
			for(size_t k = 0; k < j; k++)
			{
				S(i) -= L.value(i,k)*L.value(j,k);
			}
			// */
			
			p = &L.data.column_index[L.data.row_pointer[i]] ;
			lp = &L.data.values[L.data.row_pointer[i]];
			q = &L.data.column_index[L.data.row_pointer[j]] ;
			lq = &L.data.values[L.data.row_pointer[j]] ;

			while( (*p < j) && (*q < j) )
			{
				if(*p == *q)
				{
					S(i) -= (*lp)*(*lq);
					scalar test = S(i);
					p++;
					if(p >= &L.data.column_index[L.data.row_pointer[i+1]])
						break;
					lp++;

					q++;
					if(q >= &L.data.column_index[L.data.row_pointer[j+1] ])
						break;
					lq++;
				}
				else if(*p < *q)
				{
					p++;
					if(p >= &L.data.column_index[ L.data.row_pointer[i+1] ])
						break;
					lp++;
				}
				else
				{
					q++;
					if(q >= &L.data.column_index[ L.data.row_pointer[j+1] ])
						break;
					lq++;
				}
			}
			// */

		}

		L(j,j) = sqrt(S(j));

		for(size_t i = j+1; i < A.rows(); i++)
		{
			L(i,j) = S(i)/L.value(j,j);
		}
	}
	//std::cout << "L:\n" << L << "\nend L" << std::endl;
	// */


	ReturnCode code;
	// Ly = b
	code = forward_substitution(L,x,b);
	if(code != OK)
		return code;

	//Lx=y
	code = back_substitution(L,x,x);
	if(code != OK)
		return code;

	return OK;
}

}

#endif

