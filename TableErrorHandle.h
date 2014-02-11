#ifndef _TABLE_ERROR_HANDLE_H_
#define _TABLE_ERROR_HANDLE_H_

typedef enum {MEM_OUT, GENERAL_ERROR} TableErrors;

/* Receive an error and report it. Do all the errors report through this function. */
void ReportError(TableErrors tableError);


#endif // _TABLE_ERROR_HANDLE_H_
