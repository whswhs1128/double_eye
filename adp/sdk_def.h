

#ifndef _SDK_DEF_H_
#define _SDK_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned char       SDK_U8;         /*!< 8 bit unsigned integer. */
typedef unsigned short      SDK_U16;        /*!< 16 bit unsigned integer. */
typedef unsigned int        SDK_U32;        /*!< 32 bit unsigned integer. */
typedef unsigned long long  SDK_U64;        /*!< 64 bit unsigned integer. */
typedef char                SDK_S8;         /*!< 8 bit signed integer. */
typedef signed short        SDK_S16;        /*!< 16 bit signed integer. */
typedef signed int          SDK_S32;        /*!< 32 bit signed integer. */
typedef signed long long    SDK_S64;        /*!< 64 bit unsigned integer. */
typedef signed int          SDK_ERR;        /*!< error code type .*/
typedef unsigned int        SDK_HANDLE;     /*!< 32 bit unsigned integer. */
typedef char                SDK_CHAR;       /*!< char */


/*! common handle type */
typedef void*               SDK_HandleT;

/*! semaphore handle type */
typedef unsigned int        SDK_SemHandleT;

/*! thread handle type */
typedef unsigned int        SDK_ThreadHandleT;

/*! mutex handle type */
typedef unsigned int        SDK_MutexHandleT;

/*! cond handle type */
typedef unsigned int        SDK_ConHandleT;


#define SDK_OK                                        0

/*!  Bad parameter passed.*/
#define SDK_ERR_BAD_PARAMETER                        -1

/*! Memory allocation failed.*/
#define SDK_ERR_OUT_OF_MEMORY                        -2

/*! Device already initialised.*/
#define SDK_ERR_ALREADY_INITIALIZED                  -3

/*! Device not initialised.*/
#define SDK_ERR_NOT_INITIALIZED                      -4

/*! Feature or function is not available.*/
#define SDK_ERR_FEATURE_NOT_SUPPORTED                -5

/*! Timeout occured.*/
#define SDK_ERR_TIMEOUT                              -6

/*! The device is busy, try again later.*/
#define SDK_ERR_DEVICE_BUSY                          -7

/*! Invalid handle was passed.*/
#define SDK_ERR_INVALID_HANDLE                       -8

/*! Semaphore could not be created.*/
#define SDK_ERR_SEMAPHORE_CREATE                     -9

/*! The driver's used version is not supported.*/
#define SDK_ERR_UNSUPPORTED_VERSION                 -10

/*! Mailqueue coult not be created.*/
#define SDK_ERR_MAILQUEUE_CREATE                    -11

/*! Device/handle is not open.*/
#define SDK_ERR_NOT_OPEN                            -12

/*! Device/handle is already open.*/
#define SDK_ERR_ALREADY_OPEN                        -13

/*! Thread could not be created.*/
#define SDK_ERR_THREAD_CREATE                       -14

/*! Error from driver.*/
#define SDK_ERR_FROM_DRIVER                         -15

/*! Alead running.*/
#define SDK_ERR_ALREAD_RUNNING                      -16

/*! Out of the max size.*/
#define SDK_ERR_OUT_OF_SIZE                         -17

/*! File is written failed.*/
#define SDK_ERR_WRITE_FAILED                        -18

/*! File is read failed.*/
#define SDK_ERR_READ_FAILED                         -19

/*! Device/handle is not MAP.*/
#define SDK_ERR_MAP                                 -20

/*! operation is not permitted.*/
#define SDK_ERR_OPERATION_NOT_PERMIT                 -21

/*! Dsp encode time out*/
#define SDK_ERR_DSP_TIME_OUT                        -22


#if defined(__HIGHC__)
    #undef NULL
    #define NULL 0
#else

#ifndef NULL
    #define NULL 0  /*!< NULL define to 0. */
#endif
#endif


typedef enum
{
    SDK_FALSE = 0,  /*!< Logical false. */
    SDK_TRUE  = 1   /*!< Logical true. */
}SDK_BOOL;

#define GK_SUCCESS     0

#ifdef __cplusplus
};
#endif
#endif
