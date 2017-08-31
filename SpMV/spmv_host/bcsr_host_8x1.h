/*
 *  Copyright 2008-2009 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */



#pragma once

////////////////////////////////////////////////////////////////////////////////
//! CPU SpMV kernels
////////////////////////////////////////////////////////////////////////////////

#include "../sparse_formats.h"

template <typename IndexType, typename ValueType>
void spmv_bcsr_serial_host_8x1(const bcsr_matrix<IndexType, ValueType>& bcsr, 
                          const ValueType * x,  
                          ValueType * y)
{
  IndexType num_rows = bcsr.num_rows;
  IndexType num_cols = bcsr.num_cols;
  IndexType num_nonzeros = bcsr.num_nonzeros;
  IndexType row_block_size = bcsr.row_block_size;
  IndexType col_block_size = bcsr.col_block_size;
  IndexType num_block_rows = bcsr.num_block_rows;
  IndexType num_rows_left = bcsr.num_rows_left;
  ValueType *bAx_tmp = bcsr.bAx;
  IndexType *bAj_tmp = bcsr.bAj;
  ValueType *yp = y;

  IndexType I, K, i, j, j0;
#ifdef TESTING
  printf("Printing x .... \n");
  FILE *fp = fopen("x_value","w");
  fprintf(fp, "Vector x:\n");
  for (IndexType tmp=0; tmp<bcsr.num_rows; tmp++)
  {
    fprintf(fp, "%.2lf  ", x[tmp]);
  }
  fprintf(fp, "\n");
  fclose(fp);
#endif
  
//  if (bcsr.num_rows_left == 0)
//  {
    for (I=0; I<num_block_rows; I++, yp += 8)
    {
      register ValueType _y0 = 0, _y1 = 0, _y2 = 0, _y3 = 0, _y4 = 0, _y5 = 0, _y6 = 0, _y7 = 0;
      for (K=bcsr.bAp[I]; K<bcsr.bAp[I+1]; K++, bAj_tmp ++, bAx_tmp += 8*1 )
      {
        j0 = bAj_tmp[0];
        const ValueType *xp = x + j0;
        register ValueType _x0 = xp[0];
        _y0 += bAx_tmp[0] * _x0;
        _y1 += bAx_tmp[1] * _x0;
        _y2 += bAx_tmp[2] * _x0;
        _y3 += bAx_tmp[3] * _x0;
        _y4 += bAx_tmp[4] * _x0;
        _y5 += bAx_tmp[5] * _x0;
        _y6 += bAx_tmp[6] * _x0;
        _y7 += bAx_tmp[7] * _x0;

      }
      yp[0] += _y0;
      yp[1] += _y1;
      yp[2] += _y2;
      yp[3] += _y3;
      yp[4] += _y4;
      yp[5] += _y5;
      yp[6] += _y6;
      yp[7] += _y7;
    }
//  }
#if 0
  else
  {
    for (I=0; I<num_block_rows-1; I++, yp += 2 )
    {
      register ValueType _y0 = 0, _y1 = 0;
      for (K=bcsr.bAp[I]; K<bcsr.bAp[I+1]; K++, bAj_tmp++, bAx_tmp += 2*2 )
      {
        j0 = bAj_tmp[0];
        const ValueType *xp = x + j0;
        register ValueType _x0 = xp[0], _x1 = xp[1];
        _y0 += bAx_tmp[0] * _x0;
        _y1 += bAx_tmp[2] * _x0;
        _y0 += bAx_tmp[1] * _x1;
        _y1 += bAx_tmp[3] * _x1;
      }
      yp[0] += _y0;
      yp[1] += _y1;
    }
    for (K=bcsr.bAp[num_block_rows-1]; K<bcsr.bAp[num_block_rows]; K++)
    {
      j0 = bcsr.bAj[K];
      for (i=0; i<num_rows_left; i++)
      {
        for (j=0; j<col_block_size; j++)
        {
          y[(num_block_rows-1)*row_block_size+i] += bAx_tmp[bcsr.bAp[num_block_rows-1]*row_block_size*col_block_size + (K-bcsr.bAp[num_block_rows-1])*num_rows_left*col_block_size+i*col_block_size+j] * x[j0+j];
        }
      }
    }
  }
#endif
#ifdef TESTING
  printf("Printing y .... \n");
  fp = fopen("y_value","w");
  fprintf(fp, "Vector y:\n");
  for (IndexType tmp=0; tmp<bcsr.num_cols; tmp++)
  {
    fprintf(fp, "%.2lf  ", y[tmp]);
  }
  fprintf(fp, "\n");
  fclose(fp);
#endif
  
}

