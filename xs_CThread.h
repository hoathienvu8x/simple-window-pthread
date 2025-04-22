// =================================================================================================================
// xs_CThread.h - copyright Sree Kotay 2006-2013
// =================================================================================================================
#ifndef _xs_CTHREAD_H_
#define _xs_CTHREAD_H_

// =================================================================================================================
// basic emuation
// =================================================================================================================
#if _MSC_VER>=1200
    #include <windows.h>
    #include <process.h>
    #define XS_THREAD_CALL          __stdcall
    typedef HANDLE                  pthread_mutex_t;
    typedef HANDLE                  xs_threadhandle;
    typedef unsigned int            (XS_THREAD_CALL *xs_ThreadProc)(void *);
    #define XS_THREAD_RETURN        unsigned int

    #define XS_TLS                  _declspec( thread )
    #define XS_runthread(th, p, d)  th=(HANDLE)_beginthread(p, 0, d)
    #define XS_endthread(th)        TerminateThread((HANDLE)th, 0)
#elif __GNUC__>3 || (__GNUC__==3 && __GNUC_MINOR__>3)
    #include <pthread.h>
    #define XS_THREAD_CALL          
    typedef pthread_t               xs_threadhandle;
    typedef void                    *(*xs_ThreadProc)(void *);
    #define XS_THREAD_RETURN        void*

    #define XS_TLS                  __thread
    #define XS_runthread(th,p,d)    pthread_create(&th, NULL, p, d)
    #define XS_endthread(th)        pthread_cancel(th)
#else 
    ////uh..... out'o'luck you are....
#endif

// =================================================================================================================
// Windows emuation of pthread library
// from mongoose webserver -- http://code.google.com/p/mongoose/
//
//   Copyright (c) 2004-2013 Sergey Lyubka
//   
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//   
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//   
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
//   =================================================================================================================
#if defined(_WIN32) && !defined(__SYMBIAN32__) && !defined(_MONGOOSE_WEBSERVER_)
typedef struct {HANDLE signal, broadcast;} pthread_cond_t;

static int pthread_mutex_init(pthread_mutex_t *mutex, void *unused) {
  unused = NULL;
  *mutex = CreateMutex(NULL, FALSE, NULL);
  return *mutex == NULL ? -1 : 0;
}

static int pthread_mutex_destroy(pthread_mutex_t *mutex) {
  return CloseHandle(*mutex) == 0 ? -1 : 0;
}

static int pthread_mutex_lock(pthread_mutex_t *mutex) {
  return WaitForSingleObject(*mutex, INFINITE) == WAIT_OBJECT_0? 0 : -1;
}

static int pthread_mutex_unlock(pthread_mutex_t *mutex) {
  return ReleaseMutex(*mutex) == 0 ? -1 : 0;
}

static int pthread_cond_init(pthread_cond_t *cv, const void *unused) {
  unused = NULL;
  cv->signal = CreateEvent(NULL, FALSE, FALSE, NULL);
  cv->broadcast = CreateEvent(NULL, TRUE, FALSE, NULL);
  return cv->signal != NULL && cv->broadcast != NULL ? 0 : -1;
}

static int pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex) {
  HANDLE handles[] = {cv->signal, cv->broadcast};
  ReleaseMutex(*mutex);
  WaitForMultipleObjects(2, handles, FALSE, INFINITE);
  return WaitForSingleObject(*mutex, INFINITE) == WAIT_OBJECT_0? 0 : -1;
}

static int pthread_cond_signal(pthread_cond_t *cv) {
  return SetEvent(cv->signal) == 0 ? -1 : 0;
}

static int pthread_cond_broadcast(pthread_cond_t *cv) {
  return PulseEvent(cv->broadcast) == 0 ? -1 : 0;
}
#else
static int pthread_mutex_init(pthread_mutex_t *mutex, void *unused);
static int pthread_mutex_destroy(pthread_mutex_t *mutex);
static int pthread_mutex_lock(pthread_mutex_t *mutex);
static int pthread_mutex_unlock(pthread_mutex_t *mutex);
static int pthread_cond_init(pthread_cond_t *cv, const void *unused);
static int pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex);
static int pthread_cond_signal(pthread_cond_t *cv);
static int pthread_cond_broadcast(pthread_cond_t *cv);
#endif //pthread emulation




#endif // for entire file
