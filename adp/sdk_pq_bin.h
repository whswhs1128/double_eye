#ifndef SDK_PQ_BIN_H_
#define SDK_PQ_BIN_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "sdk_def.h"

typedef enum{
    PQ_BIN_EXPORT = 0,
    PQ_BIN_IMPORT,
    PQ_BIN_BUTT
}PQ_BIN_OPERATE;

/*!
*******************************************************************************
** \brief picture quality parameters operate,include import to board and export from board
**
** \param[in]  operateType             0,export   1,import
**
** \param[in]  pPqBinPath              import file path
**
** \param[out]  pPqBinPath             export file path
**
** \return
** - #SDK_OK   success
**   other     failed
**
**
*******************************************************************************
*/
int sdk_pq_bin_operate(PQ_BIN_OPERATE operateType,char* pPqBinPath);

#ifdef __cplusplus
};
#endif
#endif //SDK_PQ_BIN_H_

