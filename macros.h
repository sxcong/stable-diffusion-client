#ifndef MACROS_H
#define MACROS_H

// 安全删除指针
#define SAFE_DELETE(p) {if(p) {delete (p); (p) = nullptr;}}
// 安全删除指针数组
#define SAFE_DELETE_ARRAY(p) {if(p) {delete [] (p); (p) = nullptr;}}
// 安全关闭句柄
#define SAFE_CLOSE_HANDLE(h) {if(h) {CloseHandle(h); (h) = NULL;}}


#endif // MACROS_H
